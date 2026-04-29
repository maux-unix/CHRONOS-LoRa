/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHRONOS_HPP_
#define CHRONOS_HPP_

#include "canonical_huffman.hpp"
#include "crc32c.hpp"
#include "reed_solomon.hpp"

#include <magick/api.h>
#include <magick/error.h>
#include <magick/image.h>
#include <wand/magick_wand.h>

typedef struct {
    uint8_t block_id;
    uint8_t frag_id;
    uint8_t total_frags;
    uint8_t len;
    uint8_t data[FRAG_SIZE];
} Chronos_Packet_T;

/**
 * @struct CHRONOS' GraphicsMagick Process Function
 * @brief This is a type for a process function used in GraphicsMagick
 *        to simplify the code
 */
typedef Image *(*Chronos_Process_Function)(Image *, ExceptionInfo *);

/**
 * @brief Convert an image's colorspace, without changing file format,
 *        from RGB to Grayscale
 * @param *in_filename The input's filename
 * @param *out_filename The output's filename
 */
void chronos_rgb_to_grayscale(const char *in_filename,
    const char *out_filename);

void chronos_rgb_to_ycocg(char **argv);

/**
 * @brief Convert any GraphicsMagick supported image format to PPM image format
 * @param *in_filename The input's filename
 * @param *out_filename The output's filename (must include .PPM or .ppm)
 */
void chronos_convert_to_ppm(const char *in_filename, const char *out_filename);

void chronos_resize_image(const char *in_filename, const char *out_filename);

#endif /* CHRONOS_H_*/
