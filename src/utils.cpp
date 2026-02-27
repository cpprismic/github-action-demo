#include <sstream>
#include <cctype>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "utils.h"

namespace utils {

std::string formatUptime(double hours) {
    int days = static_cast<int>(hours / 24);
    int remaining_hours = static_cast<int>(hours) % 24;
    int minutes = static_cast<int>((hours - static_cast<int>(hours)) * 60);
    
    std::stringstream ss;
    if (days > 0) {
        ss << days << "d ";
    }
    ss << remaining_hours << "h " << minutes << "m";
    return ss.str();
}

std::string formatTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool parseUint64(const std::string& str, uint64_t& result) {
    if (str.empty()) {
        return false;
    }
    
    // Проверяем, что первый символ - цифра (не минус)
    size_t start = 0;
    while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }
    
    if (start >= str.length() || !std::isdigit(static_cast<unsigned char>(str[start]))) {
        return false; // Первый непробельный символ не цифра (например, минус)
    }
    
    // Проверяем, что все символы до конца строки (или до пробела) - цифры
    size_t pos = start;
    while (pos < str.length() && std::isdigit(static_cast<unsigned char>(str[pos]))) {
        pos++;
    }
    
    // Проверяем, что после цифр идут только пробелы (или конец строки)
    while (pos < str.length()) {
        if (!std::isspace(static_cast<unsigned char>(str[pos]))) {
            return false; // Найден нецифровой и непробельный символ
        }
        pos++;
    }
    
    try {
        // Используем только цифровую часть строки
        std::string numStr = str.substr(start, pos - start);
        result = std::stoull(numStr);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseDouble(const std::string& str, double& result) {
    try {
        result = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    
    return (start < end) ? std::string(start, end) : std::string();
}

bool parseArguments(int argc, char* argv[], bool& continuous_mode, int& interval) {
    continuous_mode = false;
    interval = 2;
    
    if (argc < 2) {
        return true;
    }
    
    std::string arg1 = argv[1];
    
    if (arg1 == "--help" || arg1 == "-h") {
        std::cout << "Usage: system_monitor [OPTIONS]\n";
        std::cout << "Monitor system resources\n\n";
        std::cout << "Options:\n";
        std::cout << "  -c, --continuous [N]  Run in continuous mode with N second interval (default: 2)\n";
        std::cout << "  -h, --help            Show this help message\n";
        return false;
    }

    if (arg1 == "--continuous" || arg1 == "-c") {
        continuous_mode = true;
        
        if (argc > 2) {
            std::string arg2 = argv[2];
            double interval_double;
            if (parseDouble(arg2, interval_double)) {
                if (interval_double <= 0) {
                    std::cerr << "Error: Interval must be positive\n";
                    return false;
                }
                interval = static_cast<int>(interval_double);
            } else {
                std::cerr << "Error: Invalid interval value\n";
                return false;
            }
        }
        return true;
    }
    
    // Если аргумент не распознан
    std::cerr << "Error: Unknown option: " << arg1 << "\n";
    return false;
}

} // namespace utils