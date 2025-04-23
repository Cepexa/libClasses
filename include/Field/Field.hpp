#pragma once

#include "FieldType.hpp"
#include "Nullable.hpp"
#include "Utils.hpp"

class Field {
public:

    Field(std::string name, FieldType type, Nullable nullable = Nullable::NULLABLE, bool pk = false);

    std::string getName() const;
    FieldType getType() const;
    bool isPrimaryKey() const;

    // Оператор сравнения
    bool operator==(const Field& other) const;
    bool operator!=(const Field& other) const;
    bool operator<(const Field& other) const;

    std::string getAlterSQL() const;
    std::string getSQLDefinition(bool first = false) const;

private:
    std::string name;
    FieldType type;
    Nullable nullable; 
    bool PRIMARY_KEY;
};