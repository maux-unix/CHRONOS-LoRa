# Canonical Huffman-Reed Solomon Coding Scheme (CHRONOS) for LoRa Image Transmission

This project is the implementation for my bachelor thesis titled ""

## Prerequisites

1. Raspberry Pi 4B
2. Waveshare SX1262 LoRa Module for Raspberry Pi

## Requirements

1. LLVM/Clang Tools (clang with C23 extensions, clang-format, clang-doc,
   llvm-ar)
2. libgpiod (you can installed from package manager of Raspberry Pi OS)
3. libraw

## Building the Project

Building the project is so simple, just run this command:

```cmd
clang build.c -o build
```

If you want to rebuild the entire project, just run this command:

```cmd
./build
```

## Copyright

This project is licensed under BSD-3-Clause license. See LICENSE file for
copyrights/legal things.

## Third-parties' Softwares & Copyrights

1. **[nob.h][nob_link]**: a sane build system by Alexey Kutepov,
   see [here][nob_license] for copyrigths/legal things

2. **[libgpiod][libgpiod_link]**: C library and tools for interacting with GPIO,
   see [here][libgpiod_license] for copyrigths/legal things. This software is
   linked dynamically with this project.



[nob_link]: https://github.com/tsoding/nob.h
[libgpiod_link]: https://github.com/brgl/libgpiod
[nob_license]: https://raw.githubusercontent.com/tsoding/nob.h/refs/heads/main/LICENSE
[libgpiod_license]: https://raw.githubusercontent.com/brgl/libgpiod/refs/heads/master/COPYING
