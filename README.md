# CHRONOS-LoRa

## Prerequisites (Hardware)

In this project, we used 2 pairs of the components below, one as a transmitter
and one as a receiver.

1. Raspberry Pi 4B, installed with Raspberry Pi OS (see [here][rpi4b_link])
2. Waveshare SX1262 LoRa Module for Raspberry Pi (see [here][lora_link])

## Requirements & Dependencies (Software)

This repository has vendored most of the dependencies, the rest of them are
available in Raspberry Pi OS's package manager.

1.  Required (for building):

    ```cmd
    sudo apt install clang clang-format clang-tidy lld ninja-build \
    graphicsmagick libgraphicsmagick++1-dev liblgpio-dev \
    ./thirdparty/WiringPi/wiringpi_3.18_arm64.deb -y
    ```

2. Optional (for further development):

    ```cmd
    sudo apt install clangd lldb doxygen doxygen-awesome-css graphviz -y
    ```

## Building the Project

1. Building/rebuilding:

    ```cmd
    ninja -v
    ```

2. Cleanup:

    ```cmd
    ninja clean
    ```

## Documentation

1. Local (Offline):
   ```cmd
   doxygen && xdg-open docs/html/index.html
   ```

2. Website (Online): Just open the [GitHub Pages][ghp_link]

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
[ghp_link]: https://maux-unix.github.io/CHRONOS-LoRa/
