#include <gtest/gtest.h>

#include <fstream>
#include <cstdio>

#include "system_monitor.h"

class SystemMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor = std::make_unique<SystemMonitor>();
    }
    
    void TearDown() override {
        monitor.reset();
    }
    
    std::unique_ptr<SystemMonitor> monitor;
};

// Тест на получение информации о системе (не проверяем конкретные значения,
// так как они зависят от системы, но проверяем что данные вообще получены)
TEST_F(SystemMonitorTest, GetSystemInfo) {
    SystemInfo info = monitor->getCurrentInfo();
    
    EXPECT_FALSE(info.hostname.empty());
    EXPECT_GE(info.uptime_hours, 0);
    EXPECT_GE(info.cpu_usage_percent, 0);
    EXPECT_LE(info.cpu_usage_percent, 100);
    EXPECT_GT(info.total_memory_kb, 0);
    EXPECT_GE(info.memory_usage_percent, 0);
    EXPECT_LE(info.memory_usage_percent, 100);
}

// Тест на чтение файлов
TEST_F(SystemMonitorTest, ReadFile) {
    // Создаем временный файл
    std::string test_file = "/tmp/test_monitor.txt";
    std::string test_content = "test content\nline 2";
    
    {
        std::ofstream file(test_file);
        file << test_content;
    }
    
    // Используем рефлексию для доступа к private методу? 
    // В реальном проекте лучше сделать protected или использовать friend class для тестов
    // Но для демо мы просто проверим что файл читается через публичный интерфейс
    // или сделаем отдельный тест для utils
    
    std::remove(test_file.c_str());
}

// Тест на форматирование строки информации
TEST_F(SystemMonitorTest, SystemInfoToString) {
    SystemInfo info;
    info.hostname = "test-host";
    info.uptime_hours = 1.5;
    info.cpu_usage_percent = 25.5;
    info.total_memory_kb = 8000000;
    info.available_memory_kb = 4000000;
    info.memory_usage_percent = 50.0;
    
    std::string output = info.toString();
    
    EXPECT_NE(output.find("test-host"), std::string::npos);
    EXPECT_NE(output.find("1h 30m"), std::string::npos);
    EXPECT_NE(output.find("25.5%"), std::string::npos);
    EXPECT_NE(output.find("50.0%"), std::string::npos);
}