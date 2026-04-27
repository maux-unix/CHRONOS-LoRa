# CHRONOS-LoRa

## Prerequisites (Hardware)

In this project, we used 2 pairs of the components below, one as a transmitter
and one as a receiver.

1. Raspberry Pi 4B, installed with Raspberry Pi OS (see [here][rpi4b_link])
2. Waveshare SX1262 LoRa Module for Raspberry Pi (see [here][lora_link])

## Requirements & Dependencies (Software)

Some of these can be found in your OS' package manager, or you can build it
from source. You can download binaries/source code from the links in
the Third-parties section.

### Required

These are required for building the project.

1. LLVM/Clang Tools version >= 19.1 (clang, clang-format, clang-tidy, lld)
2. GraphicsMagick version >= 1.4 (graphicsmagick, libgraphicsmagick++1-dev)
3. Ninja Build System version >= 1.12.1 (ninja)
4. WiringPi GPIO Tools version >= 3.18 (WiringPi, liblgpio-dev)

Run this command from the root project to install them:

```cmd
sudo apt install clang clang-format clang-tidy lld ninja graphicsmagick \
libgraphicsmagick++1-dev liblgpio-dev \
./thirdparty/WiringPi/wiringpi_3.18_arm64.deb -y
```

### Optional

These are optional things that you might want for further development experience.

1. LLVM/Clang LSP and Debugger Tools version >= 19.1 (clangd, lldb)
2. Doxygen Documentation Tools version >= 1.9.8 (doxygen doxygen-awesome-css graphviz)

Run this command to install them:

```cmd
sudo apt install clangd lldb doxygen doxygen-awesome-css graphviz -y
```

## Building the Project

Building/rebuilding the project is so simple, just run this command:

```cmd
ninja -v
```

If you want to clean the entire project, just run this command:

```cmd
ninja clean
```

## Documentation

Run this command to compile the documentation:

```cmd
doxygen
```

The project's documentation is in the ``` docs/ ``` folder, created
by Doxygen. You can open it with your web browser by opening
``` docs/html/index.html ``` file.

## Copyright

This project is licensed under BSD-3-Clause license. See [LICENSE](./LICENSE)
file for copyrights/legal things.

## Third-parties

1. **[GraphicsMagick][gm_link]**: Swiss army knife of image processing. Image
   processing tools collection by the GraphicsMagick Group, see
   [COPYRIGHT][gm_license] file for copyrights/legal things.

2. **[WiringPi][wp_link]**: The arguably fastest GPIO Library
   for the Raspberry Pi, see [COPYING.LESSER][wp_license] file
   for copyrights/legal things. This one is linked dynamically for compliancy.

3. **[libcorrect][lc_link]**: C library for Convolutional codes and
   Reed-Solomon, see [LICENSE][lc_license] file for copyrights/legal things.

4. **[RadioLib][rl_link]**: Universal wireless communication library for
   embedded devices, see [license.txt][rl_license] file for copyrights/legal
   things.


[rpi4b_link]: https://www.raspberrypi.com/products/raspberry-pi-4-model-b/
[lora_link]: https://www.waveshare.com/sx1262-lorawan-hat.htm
[gm_link]: https://sourceforge.net/projects/graphicsmagick/files/
[gm_license]: http://www.graphicsmagick.org/Copyright.html
[wp_link]: https://github.com/WiringPi/WiringPi
[wp_license]: ./thirdparty/WiringPi/COPYING.LESSER
[lc_link]: https://github.com/quiet/libcorrect.git
[lc_license]: ./thirdparty/libcorrect/LICENSE
[rl_link]: https://github.com/jgromes/RadioLib.git
[rl_license]: ./thirdparty/RadioLib/license.txt
