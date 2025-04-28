# Benchmark

## Toolchain versions used for the benchmark:

```
$ curl -fsSL https://cli.moonbitlang.cn/install/unix.sh | bash -s 0.1.20250423+2af95d42c
$ moon version --all
moon 0.1.20250423 (5a215d2 2025-04-23) ~/.moon/bin/moon
moonc v0.1.20250423+2af95d42c ~/.moon/bin/moonc
moonrun 0.1.20250423 (5a215d2 2025-04-23) ~/.moon/bin/moonrun

$ idf.py --version                     
ESP-IDF v5.4.1
```

## Benchmark Method

To ensure a fair comparison, the following settings were used:
-   **Algorithm:** The same algorithm was used for all implementations.
-   **MoonBit:** The optimization level was set to `O3` in the `moon.pkg.json` file.
-   **C:** The optimization level was set to `O3` in `main/CMakeLists.txt`.
-   **ESP-IDF:** In `idf.py menuconfig`, the compiler optimization level (`Component config` -> `Compiler options` -> `Optimization Level`) was set to `Optimize for performance (-O2)`.

The benchmark measures the average execution time over the first 100 ticks of the simulation.

## Benchmark Results

The following table shows the average time per tick in microseconds (µs):

|          | MoonBit (µs) | C (µs)   |
| :------- | :----------- | :------- |
| QEMU     | 30272.07     | 30185.2  |
| ST7789   | 36922.81     | 36481.1  |
