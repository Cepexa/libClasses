#pragma once

#include "Subtable.hpp"

template<typename CurrentClass, typename RecordOwner>
class IStructExample : public IStruct<CurrentClass, RecordOwner>
{
using Base = IStruct<CurrentClass, RecordOwner>;

public:
    FieldValue<std::string>* Req;

    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction) 
    {
        // Вызываем тот же метод из родителя; 
        Base::initStatic(doAction);  
        // Добавляем поля в БД.
        auto& rc = Base::recordClass;
        rc->addField({"req", FieldType::STRING});
    }

    IStructExample() : Base() 
    {
        Req = Base::template InitField<std::string>("req");
    }
};

template<typename RecordOwner>
class SubtableExample : public Subtable<SubtableExample<RecordOwner>, RecordOwner, IStructExample<SubtableExample<RecordOwner>, RecordOwner>>
{
using Base = Subtable<SubtableExample<RecordOwner>, RecordOwner, IStructExample<SubtableExample<RecordOwner>, RecordOwner>>;

public:    
    SubtableExample(RecordOwner* own) : Base(own){}

};