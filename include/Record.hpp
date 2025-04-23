#pragma once

#include <atomic>

#include "Subtable.hpp"

template<typename CurrentClass>
class Record : public BaseObject<CurrentClass>{
    using Base = BaseObject<CurrentClass>;
    using SelfPtr = std::shared_ptr<CurrentClass>;
    FieldValue<int>         *Deleted;
public:
    FieldValue<const int>   *Id;
    FieldValue<std::string> *Code;
    FieldValue<std::string> *Name;
    
    static SelfPtr Create(const std::string& code = "") {
        SelfPtr record = SelfPtr(new CurrentClass(++Base::count));//std::make_shared<CurrentClass>(++count);
        record->Code->Value(code==""?record->Id->str():code);
        #ifdef DEBUG
            std::cout<<"Create RECORD "<< Base::recordClass->getName() <<" id: "<<record->Id->str()<< std::endl;
        #endif      
        return record;
    }

    static SelfPtr Open(int id) {
        #ifdef DEBUG
            std::cout<<"Open RECORD "<< Base::recordClass->getName() <<" id: "<<id<< std::endl;
        #endif           
        auto record = SelfPtr(new CurrentClass(id));
        record->loadFromDB("SELECT * FROM " + Base::recordClass->getName() + 
        " WHERE id = " + std::to_string(id) + ";");
        return record;
    }

    static SelfPtr Open(const std::string& code) {
        std::string query = "SELECT id FROM " + Base::recordClass->getName() + 
                            " WHERE code = '" + code + "' ORDER BY id DESC LIMIT 1;";
        //pqxx::result res = InfoBase::getInstance().execute_query(query);
        //if (res.empty()) 
        //    throw std::runtime_error("Record with code '" + code + "' not found");   
        //return Open(res[0]["id"].as<int>());
        return Open(1);
    }

    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction) {
        Base::initStatic(doAction);
        auto& rc = Base::recordClass; 
        rc->addField({"id", FieldType::INTEGER, Nullable::NOT_NULL("-1"),true});
        rc->addField({"code", FieldType::STRING, Nullable::NOT_NULL("'error'")});
        rc->addField({"name", FieldType::STRING});
        rc->addField({"deleted", FieldType::INTEGER});
    }

    void Post() override
    {
        int del = Deleted->Value();
        if(del)
        {
            throw std::runtime_error("Record id=" + std::to_string(del) + " deleted");  
        }
        checkCodeExists();
        Transaction trans;
        if (isNewRecord()) {          
            trans.Add(insertRecord());
        } else {
            trans.Add(updateRecord());
        }
        for (auto& subt : subtables)
        {
            subt->Post(trans);
        }
        trans.Apply();
        Base::Post();
    }

    void Cancel() override
    {
        for (auto& subt : subtables)
        {
            subt->Cancel();
        }
        Base::Cancel();
    }

    void Delete()
    {
        std::string sql = "UPDATE " + Base::recordClass->getName() +
                        " SET " + Deleted->Name() + " = " + Id->str() +
                        " WHERE id = " + Id->str();
        //InfoBase::getInstance().execute_query(sql);
        Deleted->Value(Id->Value());
    }

    void Undelete()
    {
        checkCodeExists();
        std::string sql = "UPDATE " + Base::recordClass->getName() +
                        " SET " + Deleted->Name() + " = 0" + 
                        " WHERE id = " + Id->str();
        //InfoBase::getInstance().execute_query(sql);
        Deleted->Value(0);
    }

    template<typename SubtType>
    std::shared_ptr<SubtType> InitSubtable()
    {
        std::shared_ptr<SubtType> s(new SubtType(reinterpret_cast<CurrentClass*>(this)));
        subtables.push_back(s);       
        return s;
    }

protected:
    std::vector<std::shared_ptr<SubtableBase>> subtables;

    Record(int id) : BaseObject<CurrentClass>() {

        #ifdef DEBUG
            std::cout<<"Construct RECORD " << Base::recordClass->getName() << " id: "<<id<< std::endl;
        #endif

        Id      = Base::template InitField<const int>("id", id);
        Code    = Base::template InitField<std::string>("code");
        Name    = Base::template InitField<std::string>("name"); 
        Deleted = Base::template InitField<int>        ("deleted");
    }
    
    bool isNewRecord() const {
        return Base::savedData.empty() || Base::savedData.at("id")->str() == "0";
    }

    std::string insertRecord() {
        std::vector<std::string> columns, values;
        
        for (const auto& [name, field] : Base::currentData) {
            columns.push_back(name);
            values.push_back(field->str());
        }

        if (columns.empty()) {
            throw std::runtime_error("No data to insert");
        }

        std::string sql = "INSERT INTO " + Base::recordClass->getName() +
                        " (" + join(columns, ", ") + ")" +
                        " VALUES (" + join(values, ", ") + ")";
        
        return sql;
    }

    std::string updateRecord() {
        std::vector<std::string> updates;
        
        for (const auto& [name, field] : Base::currentData) {
            if (name == "id") continue;
            if (!Base::savedData[name]->equals(field.get())) {
                updates.push_back(name + " = " + field->str());
            }
        }

        if (updates.empty()) {
            #ifdef DEBUG
                std::cerr << "No changes to save" << std::endl;
            #endif
            return "" ;
        }

        std::string sql = "UPDATE " + Base::recordClass->getName() +
                        " SET " + join(updates, ", ") +
                        " WHERE id = " + Id->str();
        
        return sql;
    }

    void checkCodeExists()
    {
        if(Code->Value()=="")
        {
            throw std::runtime_error("Field 'code' cannot be empty.");
        }
        // auto results = InfoBase::getInstance().execute_query("Select id from " + Base::recordClass->getName() +
        //                                     " WHERE deleted = 0 and id <> " + Id->str() + " and code = " + Code->str());
        // for(const auto& res : results)
        // {
        //     throw std::runtime_error("Record with code "+Code->str()+" already exists (id: " + std::to_string(res["id"].template as<int>()) +")");
        // }
    }
};


