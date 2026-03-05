/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHRONOS_H_
#define CHRONOS_H_

#define CRC32C_DEF static inline
#define CRC32C_IMPLEMENTATION
#include "crc32c.h"

#define RS_DEF static inline
#define REED_SOLOMON_IMPLEMENTATION
#include "reed_solomon.h"

#define CANONICAL_HUFFMAN_IMPLEMENTATION
#include "canonical_huffman.h"

#endif /* CHRONOS_H_*/
