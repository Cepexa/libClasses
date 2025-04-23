#pragma once

#include "Record.hpp"
#include "SubtableExample.hpp"

class RecordTest2 : public Record<RecordTest2>
{
using Base = Record<RecordTest2>;
public:
    RecordTest2(int id) : Base(id){}
};

class RecordTest : public Record<RecordTest>
{
using Base = Record<RecordTest>;  // Алиас для базового класса

public:
    Reference<RecordTest2>* TestRef;
    Reference<RecordTest2>* Testref_арара;
    FieldValue<double>* Num;
    FieldValue<std::string>* Req;
    FieldValue<bool>* boblic;
    FieldValue<DateTime>* CreateDate;
    FieldValue<std::string>* BigStr;

    using SubtEx = SubtableExample<RecordTest>;
    using PSubtEx = std::shared_ptr<SubtEx>;
    PSubtEx pSubEx;

    static void initStatic(std::function<int(std::shared_ptr<RecordClass>, const std::string&)> doAction) {
        // Вызываем тот же метод из родителя; 
        Base::initStatic(doAction);  
        // Добавляем поля в БД.
        auto& rc = Base::recordClass;
        rc->addField({"testref", FieldType::REFERENCE<RecordTest2>()});
        rc->addField({"testref_а", FieldType::REFERENCE<RecordTest2>()});
        rc->addField({"num", FieldType::NUMERIC_P(27,2)});
        rc->addField({"req", FieldType::STRING,Nullable::NOT_NULL("'пакос'")});
        rc->addField({"bigstr", FieldType::TEXT});
        rc->addField({"bublic", FieldType::BOOLEAN,Nullable::NOT_NULL("TRUE")});
        rc->addField({"create_date", FieldType::DATETIME});

        SubtEx::initStatic(doAction);
    }

    RecordTest(int id):Base(id)
    {
        //Инициализация ссылки на RecordTest2
        TestRef       = Base::template InitReference<RecordTest2>("testref");
        Testref_арара = Base::template InitReference<RecordTest2>("testref_а");
        Num           = Base::template InitField<double>("num");
        Req           = Base::template InitField<std::string>("req");
        BigStr        = Base::template InitField<std::string>("bigstr");
        boblic        = Base::template InitField<bool>("bublic");
        CreateDate    = Base::template InitField<DateTime>("create_date");

        pSubEx        = Base::template InitSubtable<SubtEx>();
    }
};