#pragma once

#include <string>
#include <chrono>
#include <atomic>

struct SystemInfo {
    std::string hostname;
    double uptime_hours;
    double cpu_usage_percent;
    double memory_usage_percent;
    uint64_t total_memory_kb;
    uint64_t free_memory_kb;
    uint64_t available_memory_kb;
    
    std::string toString() const;
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();
    
    // Запрещаем копирование
    SystemMonitor(const SystemMonitor&) = delete;
    SystemMonitor& operator=(const SystemMonitor&) = delete;
    
    // Основные методы
    SystemInfo getCurrentInfo();
    void printInfo(const SystemInfo& info);
    void runContinuousMonitoring(int interval_seconds);

    void stop();
    
private:
    std::chrono::steady_clock::time_point last_cpu_read_;
    uint64_t last_idle_time_;
    uint64_t last_total_time_;

    std::atomic<bool> running_{true};
    
    // Внутренние методы для чтения системной информации
    std::string readHostname();
    double readUptime();
    double calculateCpuUsage();
    void readMemoryInfo(uint64_t& total, uint64_t& free, uint64_t& available);
    uint64_t readProcStat(uint64_t& idle);
    
    // Вспомогательные методы
    std::string readFile(const std::string& path);
};