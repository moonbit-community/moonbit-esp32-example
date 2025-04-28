```
idf.py update-dependencies
idf.py set-target esp32c3
idf.py build
idf.py qemu --graphics monitor
```

## Release Build

Execute `idf.py menuconfig` and set `Optimization Level` to `Optimize for performance` in `Compiler options`.