#ifndef BIKE2_BOARD_H
#define BIKE2_BOARD_H
#include <string.h>
#include "defs.h"
#include "modArith.h"
#include <sampling.h>

void keygen_board(uint8_t *pk, uint8_t *sk, const bike2_params_t *params);

void ecap_board(const uint8_t *pk, uint8_t *ct, uint8_t *ss, const bike2_params_t *params);

void decap_board(const uint8_t *sk, const uint8_t *ct, uint8_t *ss, const bike2_params_t *params);

void hash(uint8_t *output, const uint8_t *input, const uint32_t length);

void computeSyndrome(uint8_t *syndrome, uint8_t ct0, uint8_t ct1, uint8_t h0, uint8_t h1, const bike2_params_t *params);

void transpose(uint8_t *col, uint8_t *row, const uint8_t size);

void decode(uint8_t *ss, const uint8_t *syndrome, const bike2_params_t *params);

#endif
