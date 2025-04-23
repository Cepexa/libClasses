#pragma once

#include <variant>
#include <string>

#include "Reference.hpp"
#include "DateTime.hpp"

using Variant = std::variant<int, double, bool, DateTime, std::string, ReferenceBase>;

class FieldValueBase {
public:
    virtual ~FieldValueBase() = default;
    virtual std::string Name() const = 0;
    virtual std::string str() const = 0;
    virtual void setVariant(const Variant& value) = 0;
    virtual std::shared_ptr<FieldValueBase> clone() const = 0;
    virtual bool equals(const FieldValueBase* other) const = 0;
};

template<typename T>
class FieldValue : public FieldValueBase {
private:
    std::string name;
    T value;
    std::string description;

public:
    T& Value() { return value; }

    void Value(const T& newValue) { value = newValue; }

    explicit FieldValue(const std::string& fieldName, T val = T{}, const std::string& desc = "") 
        : name(to_lower(fieldName)), value(val), description(desc){}

    std::string Name() const override { return name; }

    std::string str() const override {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "TRUE" : "FALSE";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "'" + value + "'";
        } else if constexpr (std::is_same_v<T, DateTime>) {
            return value.toSQL();
        } else if constexpr (std::is_class_v<T> && std::is_same_v<decltype(std::declval<T>().id()), int>) {
            return value.id() ? std::to_string(value.id()) : "NULL";
        } else {
            return "Unsupported type";
        }
    }

    void setVariant(const Variant& v) override {
        if constexpr (std::is_same_v<T, int> ||
                      std::is_same_v<T, double> ||  
                      std::is_same_v<T, bool> || 
                      std::is_same_v<T, DateTime> || 
                      std::is_same_v<T, std::string>) 
        {
            value = std::get<T>(v);
        } 
        else 
        {
            if constexpr (std::is_constructible_v<T, int>) {
                value = static_cast<T>(std::get<int>(v));
            } else {
                throw std::bad_variant_access();
            }
        }
    }

    std::shared_ptr<FieldValueBase> clone() const override {
        return std::make_shared<FieldValue<T>>(name, value);
    }

    bool equals(const FieldValueBase* other) const override {
        auto derived = dynamic_cast<const FieldValue<T>*>(other);
        return derived && name == derived->name && value == derived->value;
    }
};


template<typename T>
class FieldValue<const T> : public FieldValueBase {
private:
    std::string name;
    T value;
    std::string description;

public:
    const T& Value() const { return value; }

    explicit FieldValue(const std::string& fieldName,const T& val, const std::string& desc = "") 
        : name(to_lower(fieldName)), value(val), description(desc){}

    std::string Name() const override { return name; }

    std::string str() const override {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "TRUE" : "FALSE";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "'" + value + "'";
        } else if constexpr (std::is_same_v<T, DateTime>) {
            return value.toSQL();
        } else if constexpr (std::is_class_v<T> && std::is_same_v<decltype(std::declval<T>().id()), int>) {
            return value.id() ? std::to_string(value.id()) : "NULL";
        } else {
            return "Unsupported type";
        }
    }

    void setVariant(const Variant& value) override {
        throw std::runtime_error("Unsupported method setVariant");
    }

    std::shared_ptr<FieldValueBase> clone() const override {
        return std::make_shared<FieldValue<const T>>(name, value);
    }

    bool equals(const FieldValueBase* other) const override {
        auto derived = dynamic_cast<const FieldValue<const T>*>(other);
        return derived && name == derived->name && value == derived->value;
    }
};