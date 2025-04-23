#pragma once

#include "BaseObject.hpp"

template<typename CurrentClass, typename RecordOwner>
class IStruct : public BaseObject<CurrentClass>
{
    using Base = BaseObject<CurrentClass>;
public:

    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction) {
        std::string recOwnerName = RecordOwner::getRecordClass()->getName();
        std::string recOwnerId = recOwnerName+"_id";
        Base::initStatic(doAction,"(index INT NOT NULL, " +
                        recOwnerId + " INT NOT NULL, "
                        "PRIMARY KEY (index, "+recOwnerId+"), "
                        "FOREIGN KEY ("+recOwnerId+") REFERENCES "+recOwnerName+"(id) ON DELETE CASCADE);","subt");
        auto& rc = Base::recordClass; 
        rc->addField({"index", FieldType::INTEGER, Nullable::NOT_NULL("-1"),true});
        rc->addField({recOwnerId, FieldType::REFERENCE<RecordOwner>(), Nullable::NOT_NULL("-1"),true});
    }
    IStruct() : Base() {}

    void Edited() { edited = true; }

    inline bool isEdited() const { return edited; }

    void Moved() { moved = true; }

    inline bool isMoved() const { return moved; }

    void resetState()
    {
        edited = false;
        moved = false;
    }

private:
    bool edited = false;
    bool moved = false;
};

class SubtableBase {
public:
    virtual ~SubtableBase() = default;
    virtual void Post(Transaction& tr) = 0;
    virtual void Cancel() = 0;
    virtual void Clear() = 0;
    virtual size_t Size() const = 0;
};

template<typename CurrentClass, typename RecordOwner, typename IStructType>
class Subtable : public SubtableBase
{
    using IStructPtr = std::shared_ptr<IStructType>;

public:
    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction) {
        IStructType::initStatic(doAction);
    }

    Subtable(RecordOwner*own):owner(own){
        #ifdef DEBUG
            std::cout<<"Construct SUBTABLE '" << IStructType::getRecordClass()->getName()<<"'. ";
            std::cout <<"Owner RECORD "<< own->getRecordClass()->getName() <<" id: "<<own->Id->str()<< std::endl;
        #endif

        Pull();  // Загружаем данные из БД
    }

    /// Добавление новой записи в конец или на указанный индекс
    IStructPtr& Add(size_t index = -1) {
        index = (index == static_cast<size_t>(-1) || index >= records.size()) ? records.size() : index;
        records.insert(records.begin() + index, std::make_shared<IStructType>());
        records[index]->Moved();
        return records[index];
    }

    /// Удаление записи по индексу с последующим сдвигом
    void Remove(size_t index) {
        if (index >= records.size()) {
            throw std::out_of_range("Индекс за пределами таблицы!");
        }
        if(index+1 != records.size())
        {
            records[index+1]->Moved();
        }
        records.erase(records.begin() + index);
    }

    void Move(size_t fromIndex, size_t toIndex) {
        if (fromIndex >= records.size() || toIndex >= records.size()) {
            throw std::out_of_range("Индекс за пределами таблицы!");
        }
        auto record = records[fromIndex];
        records.erase(records.begin() + fromIndex);
        records.insert(records.begin() + toIndex, record);
        for (size_t i = 0; i < records.size(); ++i) {
            records[i]->setIndex(i);
        }
    }

    void Post(Transaction& tr) {
        std::string tableName = IStructType::getRecordClass()->getName();
        std::string ownerIdField = owner->getRecordClass()->getName() + "_id";
        std::vector<std::string> queries;
        if (countRowDB > records.size())
        {
            queries.push_back("DELETE FROM " + tableName +
                            " WHERE " + ownerIdField + " = " + owner->Id->str() +
                            " AND index >= " + std::to_string(records.size()));
        }
        else if (countRowDB < records.size())
        {
            int n = records.size();
            int count = n - countRowDB;
            std::string values = "VALUES (" + std::to_string(n-1) + ", " + owner->Id->str() + ")" ;
            for(size_t i = 1; i < count; ++i)
            {
                values += ", (" + std::to_string(i) + ", " + owner->Id->str() + ")";
            }
            queries.push_back("INSERT INTO " + tableName + " (index, " + ownerIdField + ") " + values + ";");
        }

        for (size_t i = 0; i < records.size(); ++i) {
            IStructPtr& row = records[i];

            if (row->isMoved())
            {
                if(IStructType::getRecordClass()->getFields().size() == 0) continue;
                for(size_t j = i; j < records.size(); ++j)
                {
                    IStructPtr& row = records[j];
                    std::ostringstream updateQuery;
                    updateQuery << "UPDATE " << tableName << " SET ";
                    
                    for (const auto& field : IStructType::getRecordClass()->getFields()) {
                        std::string name = field.getName();
                        if (name == "index" || name == ownerIdField) continue;
                        updateQuery << name << " = " << row->getFieldValueByName(name)->str();
                        updateQuery << ", ";
                    }
                    if (updateQuery.tellp() > 0) {
                        updateQuery.seekp(-2, std::ios_base::end);
                    }
                    updateQuery << " WHERE index = " << j 
                                << " AND " << ownerIdField << " = " << owner->Id->str();
        
                    queries.push_back(updateQuery.str());
                }

                break;
            }
            else if (row->isEdited() && IStructType::getRecordClass()->getFields().size() != 0) {
                std::ostringstream updateQuery;
                updateQuery << "UPDATE " << tableName << " SET ";
                
                for (const auto& field : IStructType::getRecordClass()->getFields()) {
                    std::string name = field.getName();
                    if (name == "index" || name == ownerIdField) continue;
                    updateQuery << name << " = " << row->getFieldValueByName(name)->str();
                    updateQuery << ", ";
                }
                if (updateQuery.tellp() > 0) {
                    updateQuery.seekp(-2, std::ios_base::end);
                }
                updateQuery << " WHERE index = " << i 
                            << " AND " << ownerIdField << " = " << owner->Id->str();
    
                queries.push_back(updateQuery.str());
                
            }
            row->resetState();
        }
        tr.Add(queries);
    }

    void Cancel()
    {
        Pull();
    }

    /// Перегруженный оператор [], выбрасывает исключение если индекс отсутствует
    IStructPtr& Item(size_t index) {
        if (index >= records.size()) {
            throw std::out_of_range("Запись с таким индексом отсутствует!");
        }
        return records[index];
    }

    /// Перегруженный оператор [], выбрасывает исключение если индекс отсутствует
    IStructPtr& operator[](size_t index) {
        if (index >= records.size()) {
            throw std::out_of_range("Запись с таким индексом отсутствует!");
        }
        return records[index];
    }

    /// Получение всех записей
    std::vector<IStructPtr> GetAll() {
        return records;
    }

    /// Очистка всех записей
    void Clear() {
        records.clear();
    }

    /// Количество записей
    size_t Size() const {
        return records.size();
    }

private:
    RecordOwner* owner;
    std::vector<IStructPtr> records;
    size_t countRowDB = 0;

    void Pull() {
        std::string tableName = IStructType::getRecordClass()->getName();
        std::string ownerIdField = owner->getRecordClass()->getName() + "_id";
    
        // SQL-запрос на получение всех полей
        std::string query = "SELECT * FROM " + tableName +
                            " WHERE " + ownerIdField + " = " + owner->Id->str() +
                            " ORDER BY index";
    
        //auto results = InfoBase::getInstance().execute_query(query);
        records.clear();
    
        // for (const auto& rowDB : results) {
        //     auto row = std::make_shared<IStructType>();
    
        //     // Заполняем все поля
        //     for (const auto& field : IStructType::getRecordClass()->getFields()) {
        //         std::string name = field.getName();
        //         row->setValue(name, convertFieldVariant(field, rowDB[name]));
        //     }
    
        //     records.push_back(row);
        // }
        countRowDB = records.size();
    }
};