#pragma once

#include <unordered_map>

#include "RecordClass.hpp"
#include "FieldValue.hpp"
#include "Transaction.hpp"

enum class RecordStatus {
    Normal,   // Обычное состояние, без изменений
    New,      // Новая запись, требующая INSERT
    Modified, // Измененная запись, требующая UPDATE
    Deleted,  // Запись помечена как удаленная, требует DELETE
};

template<typename CurrentClass>
class BaseObject {
public:

    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction,
        const std::string& sql_create = "(id INTEGER NOT NULL PRIMARY KEY);",const std::string prefix = "r") {
        auto& rc = recordClass;
        if (!rc) {
            rc = std::make_shared<RecordClass>(prefix+std::string{typeid(CurrentClass).name()}); 
            count = doAction(rc,sql_create);   
        #ifdef DEBUG
            std::cout << "initStatic table name: "<<rc->getName()<<std::endl; 
        #endif
        }
    }

    void setValue(const std::string& fieldName, const Variant& value) {
        if (auto it = currentData.find(fieldName); it != currentData.end()) {            
            it->second->setVariant(value);
        }
    }

    template<typename T>
    T& getValue(std::string& fieldName) const {
        if (auto it = currentData.find(fieldName); it != currentData.end()) {
            if (auto field = dynamic_cast<const FieldValue<T>*>(it->second.get())) {
                return field->Value();
            }
            throw std::runtime_error("Type mismatch for field: " + fieldName);
        }
        throw std::runtime_error("Field not found: " + fieldName);
    }

    std::shared_ptr<FieldValueBase> getFieldValueByName(const std::string& s)
    {
        return currentData[s];
    }

    virtual void Post() {
        savedData = cloneData(currentData);
    }

    virtual void Cancel() {
        currentData = cloneData(savedData);
    }

    ~BaseObject() = default;

    BaseObject(const CurrentClass&) = delete;

    CurrentClass& operator=(const CurrentClass&) = delete;

    BaseObject(CurrentClass&& other) noexcept
        : currentData(std::move(other.currentData)),
          savedData(std::move(other.savedData)) {}

    CurrentClass& operator=(CurrentClass&& other) noexcept {
        if (this != &other) {
            currentData = std::move(other.currentData);
            savedData = std::move(other.savedData);
        }
        return *this;
    }

    static std::shared_ptr<RecordClass> getRecordClass(){
        return recordClass;
    }

protected:
    static std::shared_ptr<RecordClass> recordClass;
    static std::atomic<int> count;

    using FieldValueMap = std::unordered_map<std::string, std::shared_ptr<FieldValueBase>>;
    FieldValueMap currentData;
    FieldValueMap savedData;

    BaseObject(){}

    template<typename SimpleType>
    FieldValue<SimpleType>* InitField(const std::string& name,const SimpleType& val = SimpleType{}, const std::string& desc = "")
    {   
        currentData[to_lower(name)] = std::make_shared<FieldValue<SimpleType>>(name,val,desc);
        return reinterpret_cast<FieldValue<SimpleType>*>(currentData[to_lower(name)].get());
    }

    template<typename RefType>
    Reference<RefType>* InitReference(const std::string& name)
    {
        currentData[to_lower(name)] = std::make_shared<FieldValue<ReferenceBase>>(name);       
        return &((reinterpret_cast<FieldValue<Reference<RefType>>*>(currentData[to_lower(name)].get()))->Value());
    }

    void loadFromDB(const std::string& query) {
        // pqxx::result res = InfoBase::getInstance().execute_query(query);
        // if (res.empty()) { throw std::runtime_error("Record not found"); }
        
        // for (const auto& field : recordClass->getFields()) {
        //     const std::string& name = field.getName();
        //     if(!field.isPrimaryKey())
        //         setValue(name, convertFieldVariant(field, res[0][name]));
        // }
        // savedData = cloneData(currentData);
    }

    static FieldValueMap cloneData(const FieldValueMap& data) {
        FieldValueMap clone;
        for (const auto& [name, field] : data) 
            clone[name] = field->clone();
        return clone;
    }
};

template<typename CurrentClass>
std::shared_ptr<RecordClass> BaseObject<CurrentClass>::recordClass = nullptr;

template<typename CurrentClass>
std::atomic<int> BaseObject<CurrentClass>::count = 0;
