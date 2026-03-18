/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(CHRONOS_H_)
#define CHRONOS_H_

#if !defined(CHRONOS_DEF)
#define CHRONOS_DEF
#endif /* CHRONOS_DEF */

#include "canonical_huffman.h"
#include "crc32c.h"
#include "reed_solomon.h"

#include <magick/api.h>
#include <magick/error.h>
#include <magick/image.h>
#include <wand/magick_wand.h>

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
CHRONOS_DEF void chronos_rgb_to_grayscale(const char *in_filename,
    const char *out_filename);

CHRONOS_DEF void chronos_rgb_to_ycocg(char **argv);

/**
 * @brief Convert any GraphicsMagick supported image format to PPM image format
 * @param *in_filename The input's filename
 * @param *out_filename The output's filename (must include .PPM or .ppm)
 */
CHRONOS_DEF void chronos_convert_to_ppm(const char *in_filename,
    const char *out_filename);

CHRONOS_DEF void chronos_resize_image(const char *in_filename,
    const char *out_filename);

#endif /* CHRONOS_H_*/
