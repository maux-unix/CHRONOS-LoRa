/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "chronos.h"

int
main(int argc, char **argv)
{
    Crc32c_Span span = { .data = "Maulana", .length = sizeof(span.data) - 1 };
    uint32_t crc = crc32c_bitwise_encode(span, .previous_crc32c = 0);
    printf("0x%08X", crc);
    return 0;
}
