/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * crc32c.h - A CRC32-Castagnoli library using Hardware-accelerated &
 *            Parallel Computation CRC32C intrinsics from ARMv8 and ARMv9
 */

#if !defined(CRC32C_H_)
#define CRC32C_H_

#if !defined(CRC32C_DEF)
#define CRC32C_DEF
#endif /* CRC32C_DEF */

#include <arm_acle.h> /* __crc32c ARM intrinsics */
#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uint32_t */
#include <stdio.h>    /* printf() */

/**
 * @struct Crc32c_Span
 * @brief Holds a span of memory for the CRC32C computation
 */
typedef struct Crc32c_Span {
    const void *data; /**< The data in opaque type */
    size_t length;    /**< The length of the data */
} Crc32c_Span;

// TODO: Applying this + considering moving it to `chronos.h`
typedef struct Crc32c_Frame {
    Crc32c_Span span;
    uint32_t crc;
} Crc32c_Frame;

CRC32C_DEF uint32_t crc32c_encode(Crc32c_Span span);

#endif /* CRC32C_H_ */

#if defined(CRC32C_IMPLEMENTATION)

CRC32C_DEF uint32_t
crc32c_encode(Crc32c_Span span)
{
    uint32_t crc = 0xFFFFFFFFu;
    // printf("[CRC-INFO] pre-crc = 0x%08X\n", crc);
    for (size_t i = 0; i < span.length; i++) {
#if defined(CRC32C_USE_UINT8) /* CRC32C_USE_WORDSIZE */
        crc = __crc32cb(crc, ((uint8_t *)span.data)[i]);
#elif defined(CRC32C_USE_UINT16)
        crc = __crc32ch(crc, ((uint16_t *)span.data)[i]);
#elif defined(CRC32C_USE_UINT32)
        crc = __crc32cw(crc, ((uint32_t *)span.data)[i]);
#elif defined(CRC32C_USE_UINT64)
        crc = __crc32cd(crc, ((uint64_t *)span.data)[i]);
#else
        // printf("[CRC-WARN] word-size for CRC32C is not defined, no CRC
        // then.");
#endif /* CRC32C_USE_WORDSIZE */
    }
    // printf("[CRC-INFO] post-crc = 0x%08X\n", crc);
    return crc ^ 0xFFFFFFFF;
}

#endif /* CRC32_IMPLEMENTATION */
