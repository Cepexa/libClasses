/**
 * @file DateTime.h
 * @brief Класс для работы с датами и временем.
 * 
 * Поддерживает парсинг, арифметику, сравнение и форматирование дат.
 */

#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>

/**
 * @class DateTime
 * @brief Класс, представляющий дату и время.
 * 
 * Позволяет выполнять базовые операции с датами, такие как сложение, вычитание, сравнение,
 * а также преобразование в строковые форматы.
 */
class DateTime {
public:
    using Clock = std::chrono::system_clock;
    using Days = std::chrono::sys_days;
    using TimePoint = std::chrono::time_point<Clock>;

    /** @brief Конструктор по умолчанию. Устанавливает дату в начальное значение. */
    DateTime();

    /**
     * @brief Конструктор с параметрами.
     * @param year Год
     * @param month Месяц
     * @param day День
     * @param hour Часы (по умолчанию 0)
     * @param minute Минуты (по умолчанию 0)
     * @param second Секунды (по умолчанию 0)
     * @param millisecond Миллисекунды (по умолчанию 0)
     */
    DateTime(int year, int month, int day, 
             int hour = 0, int minute = 0, int second = 0, int millisecond = 0);

    /**
     * @brief Конструктор из строки.
     * @param datetimeStr Строка формата "yyyy-mm-dd HH:MM:SS.ms".
     */
    explicit DateTime(const std::string& datetimeStr);

    /** @brief Создаёт объект с текущей датой и временем. */
    static DateTime Now();

    /// @name Методы получения компонентов даты/времени
    /// @{
    static int Year(const DateTime& dt); 
    static int Month(const DateTime& dt); 
    static int Day(const DateTime& dt);
    static int Hour(const DateTime& dt); 
    static int Minute(const DateTime& dt); 
    static int Second(const DateTime& dt); 

    /// @}

    /// @name Арифметические операции с датами
    /// @{
    DateTime operator+(int days) const;
    DateTime operator-(int days) const;
    int operator-(const DateTime& other) const;
    /// @}

    /// @name Операторы сравнения
    /// @{
    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;
    /// @}

    /// @name Методы добавления времени
    /// @{
    DateTime& addYears(int years);
    DateTime& addMonths(int months);
    DateTime& addDays(int days);
    DateTime& addHours(int h);
    DateTime& addMinutes(int m);
    DateTime& addSeconds(int s);
    DateTime& addMilliseconds(int ms);
    /// @}

    /**
     * @brief Преобразование даты в строку с заданным форматом.
     * @param withoutTime Если true, возвращает только дату без времени.
     * @param format Формат строки (по умолчанию "dd.mm.yyyy HH:MM:SS").
     * @return Отформатированная строка с датой и временем.
     */
    std::string toString(bool withoutTime = false, const std::string& format = "dd.mm.yyyy HH:MM:SS") const;

    /** @brief Преобразование в SQL-совместимый формат. */
    std::string toSQL() const;
    
    /**
     * @brief Оператор вывода в поток.
     * @param os Поток вывода
     * @param dt Объект DateTime
     * @return Поток вывода
     */
    friend std::ostream& operator<<(std::ostream& os, DateTime& dt);

private:
    TimePoint time_point_;

    /** @brief Приватный конструктор из TimePoint. */
    explicit DateTime(TimePoint tp); 

    /** @brief Возвращает структуру std::tm для текущего объекта. */
    std::tm getTm(const TimePoint& tp) const;
    std::tm getTm() const;

    /** @brief Коррекция даты. */
    template <typename Duration>
    DateTime& adjustDate(Duration d) {
        auto days = std::chrono::floor<std::chrono::days>(time_point_);
        days += std::chrono::duration_cast<std::chrono::days>(d);
        time_point_ = TimePoint(days);
        return *this;
    }

    /** @brief Коррекция времени. */
    template <typename Duration>
    DateTime& adjustTime(Duration d) {
        time_point_ += d;
        return *this;
    }
};

