#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

namespace utils {

// Форматирование времени
std::string formatUptime(double hours);
std::string formatTimestamp();

// Парсинг чисел из строк
bool parseUint64(const std::string& str, uint64_t& result);
bool parseDouble(const std::string& str, double& result);

// Работа со строками
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);

// Валидация аргументов командной строки
bool parseArguments(int argc, char* argv[], bool& continuous_mode, int& interval);

} // namespace utils