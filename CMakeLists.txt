cmake_minimum_required(VERSION 3.16)
project(SystemMonitor VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

# Стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции сборки
option(BUILD_TESTS "Build tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Пути для include
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Установка путей для выходных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Добавляем исходники
add_executable(system_monitor
    src/main.cpp
    src/core/system_monitor.cpp
    src/utils/utils.cpp
)

# Подключаем заголовочные файлы
target_include_directories(system_monitor PRIVATE
    src/core
    src/utils
)

# Компиляция с предупреждениями
if(MSVC)
    target_compile_options(system_monitor PRIVATE /W4)
else()
    target_compile_options(system_monitor PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Сборка тестов
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Ищем Doxygen (опционально, не требует установки)
find_package(Doxygen QUIET)

# Опция для сборки документации (по умолчанию OFF)
option(BUILD_DOCS "Build Doxygen documentation" OFF)

if(BUILD_DOCS AND DOXYGEN_FOUND)
    # Создаём копию Doxyfile в папке сборки (чтобы не менять оригинал)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    
    # Добавляем цель для генерации документации
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
    
    # Добавляем короткий псевдоним
    add_custom_target(doc DEPENDS docs)
    
    message(STATUS "Doxygen found: targets 'docs' and 'doc' added")
    
elseif(BUILD_DOCS AND NOT DOXYGEN_FOUND)
    message(WARNING "BUILD_DOCS is ON but Doxygen not found. Install with: sudo apt install doxygen graphviz")
endif()