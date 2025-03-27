#ifndef BIKE_2_H
#define BIKE_2_H

#include <cstring>
#include "defs.h"
#include "modArith.h"
#include "rng.h"

void keygen(Key *pk, Key *sk);

void encrypt(Key *pk, Ciphertext *ct, SharedSecret *ss);

void decrypt(Key *sk, Ciphertext *ct, SharedSecret *ss);

#endif