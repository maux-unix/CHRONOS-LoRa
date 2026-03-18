/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "chronos.h"

#include <stdint.h>
#include <stdio.h>

int
main(void)
{
    uint32_t *table = { };
    for (size_t i = 0; i < 256; ++i) {
        printf("0x%08X,\n", table[i]);
    }
    return 0;
}
