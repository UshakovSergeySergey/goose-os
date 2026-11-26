# GooseOS

Минимальное 64-битное ядро операционной системы на C++20 для архитектуры `x86_64`, использующее загрузчик **Limine** (`v12.11.1`, ревизия протокола `6`).

## Структура проекта

* `src/main.cpp` — точка входа в ядро (`kmain`) и базовая работа с видеопамятью (Framebuffer).
* `linker.ld` — скрипт линковщика (Higher Half Kernel, раскладка сегментов по страницам 4 КБ).
* `limine.conf` — конфигурация загрузчика.
* `scripts/setup.sh` — скрипт загрузки и сборки SDK Limine в `third-party/limine`.
* `CMakeLists.txt` — конфигурация кросс-компиляции (`x86_64-elf`) и упаковки загрузочного `.iso`.

## Зависимости (Ubuntu / Debian)

Для сборки и запуска потребуются:
* `clang`, `lld`, `cmake`
* `xorriso` (для создания гибридного ISO-образа)
* `qemu-system-x86` (для запуска в виртуальной машине)
* `curl`, `tar`, `make`, `gcc` (для первичной настройки Limine)

```bash
sudo apt update && sudo apt install -y clang lld cmake xorriso qemu-system-x86 curl tar make gcc
```

## Первичная настройка и сборка

1. **Скачать и подготовить загрузчик Limine** (выполняется один раз после клонирования):
   ```bash
   ./scripts/setup.sh
   ```

2. **Сконфигурировать сборку CMake** (выполняется один раз или после изменения `CMakeLists.txt`):
   ```bash
   cmake -B build
   ```

3. **Собрать образ и запустить в QEMU:**
   ```bash
   cmake --build build --target run
   ```

## Цели сборки (CMake Targets)

| Команда | Описание |
| :--- | :--- |
| `cmake --build build` | Цель по умолчанию (`all`): компилирует ядро и собирает `build/goose-os.iso`. |
| `cmake --build build --target kernel.elf` | Компилирует и линкует только исполняемый бинарник ядра `build/kernel.elf`. |
| `cmake --build build --target iso` | Собирает гибридный загрузочный образ `build/goose-os.iso` (BIOS + UEFI). |
| `cmake --build build --target run` | Собирает актуальный `.iso` и запускает его в эмуляторе QEMU. |
| `cmake --build build --target clean` | Очищает скомпилированные объектные файлы и бинарник ядра. |

## Запись на USB-флешку

Для создания загрузочной флешки (режим побитовой записи `dd` для BIOS и UEFI):

1. Определите имя устройства вашей флешки (например, `/dev/sdb`):
   ```bash
   lsblk -o NAME,SIZE,MODEL,TRAN
   ```

2. Запишите собранный образ с помощью скрипта (все данные на флешке будут удалены):
   ```bash
   sudo ./scripts/make-bootable-usb.sh build/goose-os.iso /dev/sdX
   ```

> **Примечание:** для запуска на реальном ПК в настройках UEFI необходимо отключить **Secure Boot**.
