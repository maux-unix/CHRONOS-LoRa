/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "chronos.h"
#include <_strings.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <arm_acle.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int
main(int argc, char **argv)
{
//     Crc32c_Span span = { .data = "Maulana", .length = sizeof(span.data) - 1 };
//     uint32_t crc = crc32c_bitwise_encode(span, .previous_crc32c = 0);
//     printf("0x%08X\n", crc);

    uint32_t crc2 = 0xFFFFFFFF;
    const char *data = "Maulana";

    for (size_t i = 0; data[i] != '\0'; i++) {
        crc2 = __crc32cb(crc2, (uint8_t)data[i]);
    }

    crc2 ^= 0xFFFFFFFF;

    printf("0x%08X\n", crc2);
    printf("sizeof = %zu\n", sizeof(crc2));

    int width;
    int height;
    int channels;

    unsigned char *img = stbi_load("um80.jpg", &width, &height, &channels, 0);
    if (img == NULL) {
        printf("[ERROR] Cannot load the image\n");
        exit(1);
    }

    printf("[INFO] width: %d px\n", width);
    printf("[INFO] height: %d px\n", height);
    printf("[INFO] channels: %d\n", channels);

    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;

    unsigned char *gray_img = malloc(gray_img_size);
    if (gray_img == NULL) {
        printf("[ERROR] Cannot allocate memory for the gray image");
        exit(1);
    }

    for (unsigned char *p = img, *pg = gray_img; p != img + img_size;
    p += channels, pg += gray_channels) {
        uint8_t gray = (uint8_t)((p[0] + p[1] + p[2]) / 3);
        pg[0] = gray;
        if (channels == 4) pg[1] = pg[3];
    }

    if (!stbi_write_png("um80_gray.png", width, height, gray_channels, gray_img,
    width * gray_channels)) {
        printf("[ERROR] Failed to write image\n");
    }
    
    stbi_image_free(img);
    free(gray_img);
    // size_t size = width * height * channels;
    // uint8_t *data_img = malloc(size);

    // for (size_t y = 0; y < height; ++y) {
    //     for (size_t x = 0; x < width; ++x) {
    //         size_t idx = (y * width + x) * channels;
    //         data_img[idx] = 255;
    //         data_img[idx + 1] = 0;
    //         data_img[idx + 2] = 0;
    //     }
    // }

    // // size_t stride_bytes = width * channels * sizeof(uint8_t);

    // int sucess = stbi_write_bmp("test1.bmp", (int)width, (int)height,
    // (int)channels, data);

    // if (sucess) {
    //     printf("YES");
    // } else {
    //     printf("NO");
    // }
    
    return 0;
}
