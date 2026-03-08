# CHRONOS-LoRa

This project is the implementation for my bachelor thesis titled ""

## Prerequisites

1. Raspberry Pi 4B (see [here][rpi4b_link])
2. Waveshare SX1262 LoRa Module for Raspberry Pi (see [here][lora_link])

## Requirements

1. LLVM/Clang Tools version 22.1+ (clang, clang-format, clang-doc, llvm-ar,
   lld, lldb)
2. POSIX coreutils (find, sed, echo, cat, sh)
3. WiringPi version 3.18
4. GraphicsMagick version 1.3.46

## Building the Project

Building the project is so simple, just run this command:

```cmd
clang nob.c -o nob && ./nob
```

If you want to rebuild the entire project, just run this command:

```cmd
./nob
```

> [!INFO]
> The build process might spit out errors from clang-doc, please ignore them.
> That is because I only want to generate docs for CHRONOS-LoRa. If you want to
> generate docs for all of the components, add "-p build/" to clang-doc

## Documentation

The project's documentation is in the ``` docs/ ``` folder. You can open it with
your web browser by opening ``` docs/html/index.html ``` file.

## Copyright

This project is licensed under BSD-3-Clause license. See [LICENSE](./LICENSE) file for
copyrights/legal things.

## Third-parties' Softwares & Copyrights

1. **[nob.h][nob_link]**: a sane build system for C projects by Alexey Kutepov,
   see [LICENSE][nob_license] for copyrigths/legal things.

2. **[GraphicsMagick][gm_link]**: Swiss army knife of image processing. Image
   processing tools collection by the GraphicsMagick Group, see
   [COPYRIGHT][gm_license] for copyrights/legal things.

3. **[WiringPi][wp_link]**: The arguably fastest GPIO Library
   for the Raspberry Pi, see [COPYING.LESSER][wp_license]
   for copyrights/legal things. This one is linked dynamically for compliancy.

## Citing CHRONOS-LoRa

CHRONOS-LoRa was initially developed by Maulana M. Ali. If you find CHRONOS-LoRa
useful in your research and wish to cite it, please use the following BibTex
entry:

```text
@software{chronoslora2026,
    author = {Maulana M. Ali},
    title = {{CHRONOS-LoRa}}
    url =
}
```

[rpi4b_link]: https://www.raspberrypi.com/products/raspberry-pi-4-model-b/
[lora_link]: https://www.waveshare.com/sx1262-lorawan-hat.htm
[nob_link]: https://github.com/tsoding/nob.h
[nob_license]: https://raw.githubusercontent.com/tsoding/nob.h/refs/heads/main/LICENSE
[gm_link]: https://sourceforge.net/projects/graphicsmagick/files/
[gm_license]: http://www.graphicsmagick.org/Copyright.html
[wp_link]: https://github.com/WiringPi/WiringPi
[wp_license]: https://raw.githubusercontent.com/WiringPi/WiringPi/refs/heads/master/COPYING.LESSER
