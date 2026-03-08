/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * [COMPILE]
 * [LINK]
 * [RUN]:
 * [BRIEF]:
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CRC32C_USE_UINT8
#include "chronos.h"

#define CRC32C_IMPLEMENTATION
#include "crc32c.h"

/**
 * @brief the entrypoint for transmitter program
 * @param argc the argument counter
 * @param **argv the arguments
 */
int
main(int argc, char **argv)
{
    // Contoh CRC32C
    Crc32c_Span span = { .data = "Maulana", .length = strlen(span.data) };
    uint32_t crc = crc32c_encode(span);
    printf("[INFO] final-crc = 0x%08X\n", crc);

    // Contoh RGB -> Grayscale
    chronos_rgb_to_grayscale("um80.jpg", "output_gray.jpg");

    // Contoh RGB -> YCoCg-R
    chronos_rgb_to_ycocg(argv);
    return 0;
}
