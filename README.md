# moonbit-esp32-example

## Install Toolchains

### Install MoonBit Toolchains

```
$ curl -fsSL https://cli.moonbitlang.cn/install/unix.sh | bash -s 0.1.20250423+2af95d42c
```

### Install ESP IDF Toolchains

Refer to the official ESP-IDF documentation for installation instructions:

https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c3/get-started/index.html

For ESP32-C3 QEMU simulator setup, see:

https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c3/api-guides/tools/qemu.html

After installation, verify your setup by running these commands:

```bash
$ source ~/esp/v5.4.1/esp-idf/export.sh

$ idf.py --version          
ESP-IDF v5.4.1

$ qemu-system-riscv32 --version
QEMU emulator version 9.0.0 (esp_develop_9.0.0_20240606)
Copyright (c) 2003-2024 Fabrice Bellard and the QEMU Project developers
```

## Run the Example

For example, to run the Game of Life example on QEMU, follow these steps:

```bash
$ cd game-of-life/qemu

# This will install the moonbit-esp32 package
# and execute the `postadd` script for configuration.
# This also generates a Makefile to help build the project.
# Note: If Makefile already exists, it will not be overwritten. Delete it if you want to regenerate it.
$ moon install

# Set the target to esp32c3.
$ make set-target esp32c3

# Install ESP-IDF dependencies.
$ make update-deps

# Build the project.
$ make build

# Run the QEMU simulator.
$ make qemu
```

See `game-of-life/qemu/README.md` for optimized configuration settings.

For other examples, follow the similar steps as above.
