#include "FieldType.hpp"

// Определение статических типов
const FieldType FieldType::INTEGER  = FieldType(Kind::INTEGER,  "INTEGER");
const FieldType FieldType::NUMERIC  = FieldType(Kind::NUMERIC,  "NUMERIC", 0, 40, 15);
const FieldType FieldType::BOOLEAN  = FieldType(Kind::BOOLEAN,  "BOOLEAN");
const FieldType FieldType::STRING   = FieldType(Kind::STRING,   "VARCHAR", 255);
const FieldType FieldType::TEXT     = FieldType(Kind::TEXT,     "TEXT");
const FieldType FieldType::DATETIME = FieldType(Kind::DATETIME, "TIMESTAMPTZ", 3);

FieldType FieldType::NUMERIC_P(int precision, int scale) { return FieldType(Kind::NUMERIC, "NUMERIC", 0, precision, scale); }
FieldType FieldType::STRING_P(int size) { return FieldType(Kind::STRING, "VARCHAR", size); }
FieldType FieldType::REFERENCE(std::string another_table) { return FieldType(Kind::REFERENCE, "INTEGER",0,0,0,another_table); }

// Операторы сравнения
bool FieldType::operator==(const FieldType& other) const {
    return kind == other.kind && size == other.size && precision == other.precision && scale == other.scale;
}
bool FieldType::operator!=(const FieldType& other) const { return !(*this == other); }

FieldType::Kind FieldType::getKind() const {
    return kind;
}

std::string FieldType::toSLQ() const {
    return name + (
        (size)? ("("+ std::to_string(size)+")") : 
        (precision || scale) ? "("+std::to_string(precision)+ ", "+std::to_string(scale) +")" : "");
}

std::string FieldType::getAnotherTable() const{
    return another_table;
}

// Конструкторы
FieldType::FieldType(Kind kind, std::string name, int size, int precision, int scale, const std::string& another_table)
    : name(std::move(name)), kind(kind), size(size), precision(precision), scale(scale), another_table(another_table) {}