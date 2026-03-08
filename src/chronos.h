/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(CHRONOS_H_)
#define CHRONOS_H_

#include <stdint.h>

#if !defined(CHRONOS_DEF)
#define CHRONOS_DEF
#endif /* CHRONOS_DEF */

#include "crc32c.h"

#define RS_DEF static inline
#define REED_SOLOMON_IMPLEMENTATION
#include "reed_solomon.h"

#define CH_DEF static inline
#define CANONICAL_HUFFMAN_IMPLEMENTATION
#include "canonical_huffman.h"

#include <magick/api.h>
#include <wand/magick_wand.h>

CHRONOS_DEF void chronos_rgb_to_grayscale(const char *in_filename,
    const char *out_filename);

CHRONOS_DEF void chronos_rgb_to_ycocg(char **argv);

#endif /* CHRONOS_H_*/
