#pragma once

#include <string>

class FieldType {
public:
    enum class Kind { 
        REFERENCE,
        INTEGER, 
        NUMERIC, 
        STRING,
        TEXT, 
        BOOLEAN,
        DATETIME
    };

    // Статические предустановленные типы
    static const FieldType INTEGER;
    static const FieldType NUMERIC;
    static const FieldType BOOLEAN;
    static const FieldType STRING;
    static const FieldType TEXT;
    static const FieldType DATETIME;
    static FieldType NUMERIC_P(int precision, int scale);
    static FieldType STRING_P(int size);
    static FieldType REFERENCE(std::string another_table); 
    template<typename RecordClass>
    static FieldType REFERENCE() { return REFERENCE(RecordClass::getRecordClass()->getName()); }
    // Операторы сравнения
    bool operator==(const FieldType& other) const;
    bool operator!=(const FieldType& other) const;
    
    Kind getKind() const;

    std::string toSLQ() const;
    std::string getAnotherTable() const;

private:
    std::string name;
    std::string another_table;
    Kind kind;
    int size;       // Для VARCHAR(n)
    int precision;  // Для NUMERIC(p, s)
    int scale;      // Для NUMERIC(p, s)

    // Конструкторы
    FieldType(Kind kind, std::string name, int size = 0, int precision = 0, int scale = 0, const std::string& another_table = "");
};


