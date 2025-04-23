#include "Field.hpp"

Field::Field(std::string name, FieldType type, Nullable nullable, bool pk)
    : name(std::move(to_lower(name))), type(type), nullable(nullable), PRIMARY_KEY(pk) {}

std::string Field::getName() const { return name; }
    
FieldType Field::getType() const { return type; }

bool Field::isPrimaryKey() const { return PRIMARY_KEY; }
    
std::string Field::getSQLDefinition(bool first) const {
    std::string sql = name + " " + type.toSLQ();
    if (nullable.isNotNull()) {
        sql += " NOT NULL";
        if (nullable.getDefaultValue()!="") sql += " DEFAULT " + nullable.getDefaultValue();
    }
    if (type == FieldType::REFERENCE(type.getAnotherTable())) 
        sql += (first? "" : ", FOREIGN KEY ("+name+")") + " REFERENCES " +type.getAnotherTable()+ "(id)";
    return sql;
}

std::string Field::getAlterSQL() const {
    std::string sql = "ADD COLUMN " + name + " " + type.toSLQ();
    if (nullable.isNotNull()) sql += " NOT NULL";
    return sql;
}

bool Field::operator==(const Field& other) const {
        return name == other.name &&
                type == other.type &&
                nullable == other.nullable &&
                PRIMARY_KEY == other.PRIMARY_KEY;
    }

bool Field::operator!=(const Field& other) const {
        return !(*this == other);
    }

bool Field::operator<(const Field& other) const {
        return name < other.name;  // Упорядочивание по имени
    }
