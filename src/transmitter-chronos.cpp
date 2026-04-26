/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

extern "C" {
    #define REED_SOLOMON_IMPLEMENTATION
    #define RS_DEF static inline
    #include "reed_solomon.h"
}

int
main(void)
{
    rs_example();
    return 0;
}
