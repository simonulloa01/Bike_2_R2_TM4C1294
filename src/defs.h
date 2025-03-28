#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

// Note these definitions are for BIKE 2 CPA with security level 1
#define PUBLIC_KEY_SIZE 1271
#define PUBLIC_KEY_BITS 10163
#define PUBLIC_KEY_WEIGHT 71
#define PRIVATE_KEY_SIZE (1271 * 2)
#define CIPHER_SIZE 1271
#define SHARED_SECRET_SIZE 1271
#define AES256_KEY_SIZE 32
#define AES256_KEY_BITS (AES256_KEY_SIZE * 8)
#define AES256_BLOCK_SIZE 16

#endif