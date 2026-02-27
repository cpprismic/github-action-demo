Создаем директорию для сборки
```bash
mkdir build && cd build
```

Конфигурируем проект
```bash
cmake ..
```

Собираем
```bash
make
```

Запускаем приложение
```bash
./bin/system_monitor
```

Запускаем в непрерывном режиме
```bash
./bin/system_monitor -c 3
```

Запускаем тесты
```bash
ctest --output-on-failure
# или
./bin/unit_tests
```

## Инструкция по работе

Сборка образа:
```bash
# Сборка
docker-compose build

# Или через Docker напрямую
docker build -t system-monitor .
```

Запуск контейнера:
```bash
# Через docker-compose (рекомендуется для разработки)
docker-compose up -d

# Через Docker
docker run -d --name system-monitor \
  -v /proc:/host/proc:ro \
  -v /sys:/host/sys:ro \
  -v /etc:/host/etc:ro \
  system-monitor -c 5

# Просмотр логов
docker-compose logs -f
# или
docker logs -f system-monitor

# Остановка
docker-compose down
# или
docker stop system-monitor
```

Проверка работы:
```bash
# Однократный запуск
docker run --rm system-monitor

# Непрерывный режим (Ctrl+C для остановки)
docker run --rm system-monitor -c 2
```