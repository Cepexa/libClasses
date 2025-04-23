#include "Nullable.hpp"
#include "Utils.hpp"

const Nullable Nullable::NULLABLE = Nullable();

Nullable Nullable::NOT_NULL(const std::string& defaultValue) {
    return Nullable(defaultValue);
}

bool Nullable::isNotNull() const { return notNullConstraint; }

const std::string& Nullable::getDefaultValue() const {
    if (!notNullConstraint) {
        throw std::logic_error("No default value for NULL field!");
    }
    return defaultValue;
}

bool Nullable::operator==(const Nullable& other) const {
    return notNullConstraint == other.notNullConstraint && 
        (!notNullConstraint || to_lower(defaultValue) == to_lower(other.defaultValue));
}
bool Nullable::operator!=(const Nullable& other) const {
    return !(*this == other);
}

Nullable::Nullable(const std::string& defaultValue) : defaultValue(defaultValue), notNullConstraint(true) {}

Nullable::Nullable() : notNullConstraint(false) {}