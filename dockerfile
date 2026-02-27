# Этапы сборки (multi-stage build)
FROM ubuntu:22.04 AS builder

# Предотвращаем интерактивные запросы при установке пакетов
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем зависимости для сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Создаем рабочую директорию
WORKDIR /app

# Копируем исходный код
COPY CMakeLists.txt .
COPY src/ ./src/
COPY tests/ ./tests/

# Собираем приложение
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Финальный образ
FROM ubuntu:22.04

# Устанавливаем минимальные зависимости для запуска
RUN apt-get update && apt-get install -y \
    && rm -rf /var/lib/apt/lists/*

# Создаем непривилегированного пользователя для безопасности
RUN useradd -m -s /bin/bash monitor

# Копируем собранное приложение из builder
COPY --from=builder /app/build/bin/system_monitor /usr/local/bin/

# Переключаемся на непривилегированного пользователя
USER monitor

# Запускаем приложение (по умолчанию однократный вывод)
ENTRYPOINT ["/usr/local/bin/system_monitor"]
CMD []