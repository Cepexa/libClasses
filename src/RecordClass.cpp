#include <typeinfo>
#include <set>
#include <algorithm>

#include "RecordClass.hpp"

RecordClass::RecordClass(std::string name) : name(to_lower(name)){}

void RecordClass::addField(const Field& field) {
    fields.emplace(field);
}

std::set<Field> RecordClass::getFields() const { return fields; }
std::string RecordClass::getName() const { return name; }