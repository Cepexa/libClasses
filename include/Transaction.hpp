#pragma once

#include <iostream>
#include <string>
#include <vector>

class Transaction {
private:
    //std::unique_ptr<pqxx::work> txn;
    //std::vector<std::string> queries;
    bool active = true;  // Флаг активности транзакции

public:
    // Запрещаем копирование
    Transaction(const Transaction&) = delete;
    
    Transaction& operator=(const Transaction&) = delete;

    // Разрешаем перемещение
    Transaction(Transaction&&) = default;
    
    Transaction& operator=(Transaction&&) = default;

    // Конструктор создаёт транзакцию и резервирует место для запросов
    explicit Transaction(size_t expectedQueries = 20);

    void StartNewTransaction();

    // Добавление одного запроса
    void Add(const std::string& query);
    
    // Добавление нескольких запросов
    void Add(const std::vector<std::string>& newQueries);
    
    // Отмена всех накопленных запросов
    void Cancel();

    // Применение транзакции
    bool Apply();

    // ♻️ Начинаем новую
    void Restart();

    // Откат транзакции в деструкторе, если не применена
    ~Transaction();
};
