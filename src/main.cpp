#include <iostream>
#include <csignal>
#include <atomic>

#include "system_monitor.h"
#include "utils.h"

SystemMonitor* g_monitor = nullptr;

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    if (g_monitor) {
        g_monitor->stop();
    }
}

int main(int argc, char* argv[]) {
    bool continuous_mode = false;
    int interval = 2;
    
    if (!utils::parseArguments(argc, argv, continuous_mode, interval)) {
        return 1;
    }
    
    SystemMonitor monitor;
    g_monitor = &monitor;
    
    // Устанавливаем обработчик сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    if (continuous_mode) {
        monitor.runContinuousMonitoring(interval);
    } else {
        SystemInfo info = monitor.getCurrentInfo();
        monitor.printInfo(info);
    }
    
    return 0;
}