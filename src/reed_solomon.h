/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(REED_SOLOMON_H_)
#define REED_SOLOMON_H_

#include <correct.h>

#if !defined(RS_DEF)
#define RS_DEF
#endif /* RS_DEF */

/* Define maxval for data shards */
#if !defined(DATA_SHARD_MAX)
#define DATA_SHARD_MAX (255)
#endif /* DATA_SHARD_MAX */

typedef struct Reed_Solomon_Type {
    int data_shards;
    int parity_shards;
    int shards;
    unsigned char *m;
    unsigned char *parity;
} Reed_Solomon_Type;

void rs_init(void);
Reed_Solomon_Type *reed_solomon_new(int data_shards, int parity_shards);
void reed_solomon_release(Reed_Solomon_Type *rs);

#endif /* REED_SOLOMON_H_ */

#ifdef REED_SOLOMON_IMPLEMENTATION

#endif /* REED_SOLOMON_IMPLEMENTATION */
