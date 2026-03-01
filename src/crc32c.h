/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CRC32C_H_
#define CRC32C_H_

#ifndef CRC32C_DEF
#define CRC32C_DEF
#endif /* CRC32C_DEF */

#include <stddef.h> /* size_t */
#include <stdint.h> /* uint32_t */
#include <stdio.h>  /* printf() */

constexpr uint32_t CRC32C_NORMAL_POLYNOMIAL = 0x1EDC6F41u;
constexpr uint32_t CRC32C_REVERSED_POLYNOMIAL = 0x82F63B78u;

typedef struct {
    const void *data;
    size_t length;
} Crc32c_Span;

/* This is for default/optional parameter for `crc32c_bitwise_encode' */
typedef struct {
    uint32_t previous_crc32c;
} Crc32c_Opt;

/* Expand the parameters for `crc32c_bitwise_encode' */
#define crc32c_bitwise_encode(span, ...) \
    crc32c_bitwise_encode_opt((span), (Crc32c_Opt) { __VA_ARGS__ })

CRC32C_DEF uint32_t crc32c_bitwise_encode_opt(Crc32c_Span span, Crc32c_Opt opt);
CRC32C_DEF void crc32c_fill_table(uint32_t *table);

#endif /* CRC32C_H_ */

#ifdef CRC32C_IMPLEMENTATION

CRC32C_DEF uint32_t
crc32c_bitwise_encode_opt(Crc32c_Span span, Crc32c_Opt opt)
{
    uint32_t crc = ~opt.previous_crc32c; // same as previousCrc32 ^ 0xFFFFFFFF
    unsigned char* current = (unsigned char*) span.data;

    printf("[INFO] opt.previous_crc32 = %d\n", opt.previous_crc32c);
    printf("[INFO] span.data = %s\n", current);
    printf("[INFO] span.length = %zu\n", span.length);

    while (span.length--) {
        crc ^= *current++;
        for (unsigned int j = 0; j < 8; j++)
            if (crc & 1) crc = (crc >> 1) ^ CRC32C_REVERSED_POLYNOMIAL;
            else crc = crc >> 1;
    }
    return ~crc; // same as crc ^ 0xFFFFFFFF
}

CRC32C_DEF void
crc32c_fill_table(uint32_t *table)
{
    uint8_t index = 0;
    uint8_t z;

    do {
        table[index] = index;
        for (z = 8; z; z--)
            table[index] = (table[index] & 1) ?
                (table[index] >> 1) ^ CRC32C_REVERSED_POLYNOMIAL :
                table[index] >> 1;
    } while (++index);
}

#endif /* CRC32_IMPLEMENTATION */
