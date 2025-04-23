#pragma once

#include <string>
#include <stdexcept>

/**
 * @class Nullable
 * @brief Определяет возможность хранения NULL-значений для полей БД.
 */
class Nullable {
public:
    /**
     * @brief Глобальный объект для обозначения NULLABLE полей.
     */
    static const Nullable NULLABLE;

    /**
     * @brief Создаёт объект с ограничением NOT NULL и значением по умолчанию.
     * @param Значение, используемое при отсутствии пользовательского ввода.
     * @return Объект Nullable с установленным ограничением NOT NULL.
     * @throw std::invalid_argument Если defaultValue пустая строка.
     */ 
    static Nullable NOT_NULL(const std::string&);

    /**
     * @brief Проверяет, установлено ли ограничение NOT NULL.
     * @return true, если поле не может быть NULL, иначе false.
     */
    bool isNotNull() const;

    /**
     * @brief Получает значение по умолчанию.
     * @return Значение по умолчанию для NOT NULL поля.
     * @throw std::logic_error Если значение по умолчанию не задано.
     */
    const std::string& getDefaultValue() const;
    
    /**
     * @brief Оператор сравнения.
     * @param other Другой объект Nullable.
     * @return true, если объекты эквивалентны, иначе false.
     */
    bool operator==(const Nullable& other) const;
    
    /**
     * @brief Оператор неравенства.
     * @param other Другой объект Nullable.
     * @return true, если объекты различны, иначе false.
     */
    bool operator!=(const Nullable& other) const;

private:
    std::string defaultValue; ///< Значение по умолчанию для NOT NULL поля.
    bool notNullConstraint; ///< Флаг ограничения NOT NULL (true - NOT NULL, false - NULLABLE).

    /**
     * @brief Приватный конструктор для создания NOT NULL объекта.
     * @param defaultValue Значение по умолчанию.
     */
    explicit Nullable(const std::string&);

    /**
     * @brief Приватный конструктор для NULLABLE объекта.
     */
    Nullable();
};

