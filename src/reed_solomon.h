/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * reed_solomon.h - Reed Solomon Codes Implmentation of libcorrect
 */

#if !defined(REED_SOLOMON_H_)
#define REED_SOLOMON_H_

#include <correct.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(RS_DEF)
#define RS_DEF
#endif /* RS_DEF */

#define RS_K      223
#define RS_N      255
#define NROOTS    (RS_N - RS_K)
#define FRAG_SIZE 32
#define MAX_FRAGS ((RS_N + FRAG_SIZE - 1) / FRAG_SIZE)

typedef struct {
    correct_reed_solomon *rs;
} rs_ctx_t;

int rs_init(rs_ctx_t *ctx);
void rs_free(rs_ctx_t *ctx);
void rs_encode(rs_ctx_t *ctx, const uint8_t *input, uint8_t *out);
int rs_decode(rs_ctx_t *ctx, uint8_t *encoded, uint8_t *decoded);
void rs_example(void);

#endif /* REED_SOLOMON_H_ */

#ifdef REED_SOLOMON_IMPLEMENTATION

int
rs_init(rs_ctx_t *ctx)
{
    ctx->rs = correct_reed_solomon_create(
        correct_rs_primitive_polynomial_8_4_3_2_0, 1, 1, NROOTS);
    return ctx->rs ? 0 : -1;
}

void
rs_free(rs_ctx_t *ctx)
{
    if (ctx->rs) correct_reed_solomon_destroy(ctx->rs);
}

void
rs_encode(rs_ctx_t *ctx, const uint8_t *in, uint8_t *out)
{
    memcpy(out, in, RS_K);
    correct_reed_solomon_encode(ctx->rs, in, RS_K, out + RS_K);
}

int
rs_decode(rs_ctx_t *ctx, uint8_t *encoded, uint8_t *decoded)
{
    return correct_reed_solomon_decode(ctx->rs, encoded, RS_N, decoded);
}

void
rs_example(void)
{
    correct_reed_solomon *rs = correct_reed_solomon_create(
        correct_rs_primitive_polynomial_8_4_3_2_0, // primitive_polynomial
        1,                                         // first_consecutive_root
        1,                                         // generator_root_gap
        NROOTS                                     // num_roots
    );

    if (!rs) {
        printf("Failed to create RS codec");
    }

    uint8_t data[RS_K];
    uint8_t encoded[RS_N];
    uint8_t decoded[RS_K];

    memset(data, 0, RS_K);
    memcpy(data, "HELLO LORA", 10);

    memcpy(encoded, data, RS_K);

    correct_reed_solomon_encode(rs, data, RS_K, encoded + RS_K);

    printf("RS encoded\n");

    // Simulate corruption
    encoded[5] ^= 0xFF;
    encoded[100] ^= 0xFF;

    auto rs_result = correct_reed_solomon_decode(rs, encoded, RS_N, decoded);

    if (rs_result < 0) {
        printf("Decode failed");
    } else {
        printf("Decoded %ld bytes\n", rs_result);
        printf("Message: %.*s\n", (int)rs_result, decoded);
    }

    // printf("Corrected errors: %d\n", corrected);

    correct_reed_solomon_destroy(rs);
}

#endif /* REED_SOLOMON_IMPLEMENTATION */
