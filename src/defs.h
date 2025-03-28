#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <stdbool.h>

// Note these definitions are for BIKE 2 CPA with security level 1
#define R_BITS 10163
#define R_SIZE (R_BITS % 8 == 0 ? R_BITS / 8 : R_BITS / 8 + 1)
#define N_BITS (10163 * 2)
#define N_SIZE (N_BITS % 8 == 0 ? N_BITS / 8 : N_BITS / 8 + 1)
#define W 142
#define T 134

#define PUBLIC_KEY_WEIGHT 71
#define CIPHER_SIZE 1271
#define SHARED_SECRET_SIZE 1271
#define AES256_KEY_SIZE 32
#define AES256_KEY_BITS (AES256_KEY_SIZE * 8)
#define AES256_BLOCK_SIZE 16

#endif