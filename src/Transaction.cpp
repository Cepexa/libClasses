#include "Transaction.hpp"

Transaction::Transaction(size_t expectedQueries) {
    StartNewTransaction();
    //queries.reserve(expectedQueries);  // Резервируем место под запросы
}

void Transaction::StartNewTransaction() {
    //txn = std::make_unique<pqxx::work>(InfoBase::getInstance().conn);
    active = true;
}

void Transaction::Add(const std::string& query) {
    //queries.push_back(query);
}

void Transaction::Add(const std::vector<std::string>& newQueries) {
    //queries.insert(queries.end(), newQueries.begin(), newQueries.end());
}

void Transaction::Cancel() {
    //queries.clear();
}

bool Transaction::Apply() {
    if (!active) {
        std::cerr << "Ошибка: Транзакция уже завершена. Начните новую." << std::endl;
        return false;
    }
    try {
        // for (const auto& query : queries) {
        //     txn->exec(query);
        // }
        // txn->commit();
        // queries.clear();  // Очищаем список запросов
        active = false;
        //std::cout << "Транзакция успешно применена. Запросов: " << queries.size() << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка транзакции: " << e.what() << std::endl;
        return false;
    }
}

void Transaction::Restart() {
    // if (active) {
    //     txn->abort();  // Откатить если не применена
    // }
    // queries.clear();
    StartNewTransaction();  // Начинаем новую транзакцию
}

Transaction::~Transaction() {
    if (active) {
        //txn->abort();
    }
}