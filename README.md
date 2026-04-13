# CHRONOS-LoRa

This project is the implementation for my bachelor thesis titled ""

## Background and Similar Works

## Prerequisites (Hardware)

In this project, we used 2 pairs of the components below, one as a transmitter
and one as a receiver.

1. Raspberry Pi 4B (see [here][rpi4b_link])
2. Waveshare SX1262 LoRa Module for Raspberry Pi (see [here][lora_link])

## Requirements & Dependencies

Some of these can be found in your OS' package manager, or you can build it
from source. You can download binaries/source code from the links in 
the Third-parties section.

1. LLVM/Clang Tools version 20.1+ (clang, clang-format, clang-doc, llvm-ar,
   lld, lldb)
2. POSIX coreutils (find, sed, echo, cat, sh)
3. WiringPi version 3.18
4. GraphicsMagick version 1.3+

## Building the Project

Building the project is so simple, just run this command:

```cmd
clang nob.c -o nob && ./nob
```

If you want to rebuild the entire project, just run this command:

```cmd
./nob
```

> [!IMPORTANT]
> The build process might spit out errors from clang-doc, please ignore them.
> That is because I only want to generate docs for CHRONOS-LoRa. If you want to
> generate docs for all of the components, add "-p build/" to clang-doc

## Testing

## Documentation

The project's documentation is in the ``` docs/ ``` folder, created
by the build system. You can open it with your web browser by opening
``` docs/html/index.html ``` file.

## Copyright

This project is licensed under BSD-3-Clause license. See [LICENSE](./LICENSE)
file for copyrights/legal things.

## Third-parties

1. **[nob.h][nob_link]**: a sane build system for C projects by Alexey Kutepov,
   see [LICENSE][nob_license] file for copyrigths/legal things.

2. **[GraphicsMagick][gm_link]**: Swiss army knife of image processing. Image
   processing tools collection by the GraphicsMagick Group, see
   [COPYRIGHT][gm_license] file for copyrights/legal things.

3. **[WiringPi][wp_link]**: The arguably fastest GPIO Library
   for the Raspberry Pi, see [COPYING.LESSER][wp_license] file
   for copyrights/legal things. This one is linked dynamically for compliancy.

4. **[libcorrect][lc_link]**: C library for Convolutional codes and 
   Reed-Solomon, see [LICENSE][lc_license] file for copyrights/legal things.

5. **[RadioLib][rl_link]**: Universal wireless communication library for
   embedded devices, see [license.txt][rl_license] file for copyrights/legal 
   things.

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
[lc_link]: https://github.com/quiet/libcorrect.git
[lc_license]: https://raw.githubusercontent.com/quiet/libcorrect/refs/heads/master/LICENSE
[rl_link]: https://github.com/jgromes/RadioLib.git
[rl_license]: https://raw.githubusercontent.com/jgromes/RadioLib/refs/heads/master/license.txt
