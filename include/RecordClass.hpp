#pragma once

#include <set>

#include "Field.hpp"

class RecordClass {
public:
    RecordClass(std::string name);
    
    void addField(const Field& field);
    
    std::set<Field> getFields() const;
    
    std::string getName() const;
    
private:
    std::string name;
    std::set<Field> fields;
};