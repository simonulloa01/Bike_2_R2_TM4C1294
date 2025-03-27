#ifndef DEFS_H
#define DEFS_H

#include <cstdint>

//Note these definitions are for BIKE 2 CPA with security level 1
#define KEY_SIZE            1271
#define CIPHER_SIZE         1271
#define SHARED_SECRET_SIZE  1271

// public key data type
using PublicKey = uint8_t[KEY_SIZE];

// private key data type
using PrivateKey = uint8_t[2*KEY_SIZE];

// Ciphertext data type
using Ciphertext = uint8_t[CIPHER_SIZE];

// shared secret data type
using SharedSecret = uint8_t[SHARED_SECRET_SIZE];

#endif 