#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>
#include <thread>
#include <atomic>

#include "system_monitor.h"
#include "utils.h"

SystemMonitor::SystemMonitor() 
    : last_cpu_read_(std::chrono::steady_clock::now())
    , last_idle_time_(0)
    , last_total_time_(0) {
    // Инициализируем первое чтение CPU
    last_idle_time_ = 0;
    last_total_time_ = 0;
    calculateCpuUsage(); // Первый вызов для установки базовых значений
}

SystemMonitor::~SystemMonitor() {
    stop();
}

void SystemMonitor::stop() {
    running_ = false;
}

std::string SystemMonitor::readFile(const std::string& path) {
    // Проверяем, не запущены ли мы в контейнере
    static const std::string proc_prefix = []() {
        if (access("/host/proc/stat", F_OK) == 0) {
            return "/host";
        }
        return "";
    }();
    
    std::string full_path = proc_prefix + path;
    std::ifstream file(full_path);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string SystemMonitor::readHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
}

double SystemMonitor::readUptime() {
    std::string content = readFile("/proc/uptime");
    if (content.empty()) {
        return 0.0;
    }
    
    double uptime_seconds;
    std::istringstream iss(content);
    iss >> uptime_seconds;
    return uptime_seconds / 3600.0; // Конвертируем в часы
}

uint64_t SystemMonitor::readProcStat(uint64_t& idle) {
    std::string content = readFile("/proc/stat");
    if (content.empty()) {
        return 0;
    }
    
    std::istringstream iss(content);
    std::string cpu;
    uint64_t user, nice, system, idle_time, iowait, irq, softirq, steal;
    
    iss >> cpu >> user >> nice >> system >> idle_time >> iowait >> irq >> softirq >> steal;
    
    if (cpu != "cpu") {
        return 0;
    }
    
    idle = idle_time + iowait;
    return user + nice + system + idle_time + iowait + irq + softirq + steal;
}

double SystemMonitor::calculateCpuUsage() {
    uint64_t idle, total;
    
    // Читаем текущие значения
    total = readProcStat(idle);
    
    // Если это первый вызов, просто сохраняем значения
    if (last_total_time_ == 0 || last_idle_time_ == 0) {
        last_total_time_ = total;
        last_idle_time_ = idle;
        last_cpu_read_ = std::chrono::steady_clock::now();
        return 0.0;
    }
    
    // Вычисляем разницу
    uint64_t total_diff = total - last_total_time_;
    uint64_t idle_diff = idle - last_idle_time_;
    
    // Обновляем сохраненные значения
    last_total_time_ = total;
    last_idle_time_ = idle;
    last_cpu_read_ = std::chrono::steady_clock::now();
    
    if (total_diff == 0) {
        return 0.0;
    }
    
    // Использование CPU = (total_diff - idle_diff) / total_diff * 100
    return (static_cast<double>(total_diff - idle_diff) / total_diff) * 100.0;
}

void SystemMonitor::readMemoryInfo(uint64_t& total, uint64_t& free, uint64_t& available) {
    total = free = available = 0;
    
    std::string content = readFile("/proc/meminfo");
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        std::vector<std::string> parts = utils::split(line, ' ');
        if (parts.size() < 2) continue;
        
        // Удаляем двоеточие из первой части
        std::string key = parts[0];
        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }

        uint64_t value = 0;
        for (size_t i = 1; i < parts.size(); ++i) {
            if (utils::parseUint64(parts[i], value)) {
                break; // Нашли число
            }
        }
        
        if (value > 0) {
            if (key == "MemTotal") total = value;
            else if (key == "MemFree") free = value;
            else if (key == "MemAvailable") available = value;
        }
    }
}

SystemInfo SystemMonitor::getCurrentInfo() {
    SystemInfo info;
    
    info.hostname = readHostname();
    info.uptime_hours = readUptime();
    info.cpu_usage_percent = calculateCpuUsage();
    
    readMemoryInfo(info.total_memory_kb, info.free_memory_kb, info.available_memory_kb);
    
    if (info.total_memory_kb > 0) {
        info.memory_usage_percent = 100.0 * (info.total_memory_kb - info.available_memory_kb) / info.total_memory_kb;
    } else {
        info.memory_usage_percent = 0.0;
    }
    
    return info;
}

std::string SystemInfo::toString() const {
    std::stringstream ss;
    ss << "=== System Information ===\n";
    ss << "Timestamp: " << utils::formatTimestamp() << "\n";
    ss << "Hostname: " << hostname << "\n";
    ss << "Uptime: " << utils::formatUptime(uptime_hours) << "\n";
    ss << "CPU Usage: " << std::fixed << std::setprecision(1) << cpu_usage_percent << "%\n";
    ss << "Memory Usage: " << std::fixed << std::setprecision(1) << memory_usage_percent << "% ";
    ss << "(" << (total_memory_kb - available_memory_kb) / 1024 << "MB / " 
       << total_memory_kb / 1024 << "MB)\n";
    ss << "==========================";
    return ss.str();
}

void SystemMonitor::printInfo(const SystemInfo& info) {
    std::cout << info.toString() << std::endl;
}

void SystemMonitor::runContinuousMonitoring(int interval_seconds) {
    std::cout << "Starting continuous monitoring (Ctrl+C to stop)..." << std::endl;
    
    while (running_) {
        SystemInfo info = getCurrentInfo();
        printInfo(info);
        std::cout << std::endl;
        
        for (int i = 0; i < interval_seconds && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    std::cout << "Monitoring stopped." << std::endl;
}