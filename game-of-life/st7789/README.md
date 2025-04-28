```
moon install
make set-target esp32c3
make update-deps
make build
make flash monitor
```

## Release Build

In `./src/main/moon.pkg.json`, change `-Og` to `-O2` or `-O3` in `["link"]["native"]["cc-flags"]` for better optimization.

Execute `idf.py menuconfig` and set `Optimization Level` to `Optimize for performance` in `Compiler options`.
