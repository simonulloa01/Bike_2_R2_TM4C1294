#ifndef BIKE2_BOARD_H
#define BIKE2_BOARD_H
#include <string.h>
#include "defs.h"
#include "modArith.h"
#include <sampling.h>

void keygen_board(uint8_t *pk, uint8_t *sk, const bike2_params_t *params);

void encrypt_board(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const bike2_params_t *params);

void decrypt_board(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const bike2_params_t *params);

#endif
