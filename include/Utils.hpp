#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

/**
 * @brief Converts a string to lowercase.
 *
 * This function takes an input string and converts all its characters
 * to lowercase using the std::tolower function.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in lowercase.
 */
inline std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * @brief Converts a string to uppercase.
 *
 * This function takes an input string and converts all its characters
 * to uppercase using the std::toupper function.
 *
 * @param str The input string to be converted.
 * @return A new string with all characters in uppercase.
 */
inline std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

/**
 * @brief Объединяет элементы вектора строк в одну строку с указанным разделителем.
 *
 * Функция принимает вектор строк и объединяет их, вставляя между элементами заданный разделитель.
 * Если вектор пуст, функция возвращает пустую строку.
 *
 * @param elements Вектор строк, которые нужно объединить.
 * @param delimiter Разделитель, вставляемый между элементами.
 * @return std::string Строка, содержащая объединённые элементы с разделителем.
 *
 * @note Если `elements` содержит только один элемент, результат будет равен этому элементу без разделителя.
 * @example
 * @code
 * std::vector<std::string> words = {"Hello", "world", "!"};
 * std::string result = join(words, " ");
 * // result == "Hello world !"
 * @endcode
 */
inline std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
    std::ostringstream oss;
    if (elements.size()) oss << elements[0];
    for (size_t i = 1; i < elements.size(); ++i) 
        oss << delimiter << elements[i];
    return oss.str();
}

/**
 * @brief Выполняет сопоставление строки с шаблоном с поддержкой `*`, `?` и экранирования `\`.
 *
 * @param text Строка, которую нужно проверить на соответствие шаблону.
 * @param mask Шаблон с поддержкой:
 *             - `*` — любое количество символов (включая 0).
 *             - `?` — ровно один любой символ.
 *             - `\*`, `\?`, `\\` — экранированные версии `*`, `?`, `\` соответственно.
 * @return `true`, если `text` соответствует `mask`, иначе `false`.
 *
 * @note Символ `\` используется для экранирования `*` и `?`, а `\\` обозначает сам `\`.
 *
 * **Примеры использования:**
 * @code
 * match("hello", "h*o");        // true  (любой набор символов между 'h' и 'o')
 * match("hello", "h?llo");      // true  ('?' заменяет 'e')
 * match("C:\\Windows", "C:\\\\*");  // true (экранированный '\')
 * match("data\\text", "data\\\\text"); // true (правильно обрабатывает '\\')
 * match("file.txt", "*.txt");   // true (совпадение по расширению)
 * match("test.cpp", "test\\.*"); // false ('\.' означает сам '.')
 * @endcode
 */
inline bool match(const std::string& text, const std::string& mask) {
    int n = text.size(), m = mask.size();
    int i = 0, j = 0, starPos = -1, matchPos = -1;
    while (i < n) {
        bool escape = false;
        while (j < m && mask[j] == '\\') {
            escape = !escape;
            ++j;
        }
        if (j >= m) return false;
        char mc = mask[j];
        if (!escape && mc == '*') { 
            starPos = j++;
            matchPos = i;
        } else if (!escape && mc == '?') { 
            ++i; ++j;
        } else if (escape || mc == text[i]) { 
            ++i; ++j;
        } else if (starPos != -1) { 
            j = starPos + 1;
            i = ++matchPos;
        } else { 
            return false;
        }
    }
    while (j < m && mask[j] == '*') ++j;
    return j == m;
}


