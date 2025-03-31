
#include "rng.h"

void perform_aes(uint8_t *ciphertext, aes_ctr_prf_state *rngState)
{
    AES_encrypt(rngState->ctr.bytes, ciphertext, &rngState->ctr.bytes);
    rngState->ctr.qwords[0]++;
}

void aes_ctr_prf(uint8_t *random, aes_ctr_prf_state *rngState, const uint32_t len)
{
    //When Len i smaller then whats left in the buffer 
    //No need in additional AES.
    if ((len + rngState->pos) <= AES256_BLOCK_SIZE)
    {
        memcpy(random, &rngState->buffer.bytes[rngState->pos], len);
        rngState->pos += len;
    }

    //if s.pos != AES256_BLOCK_SIZE then copy whats left in the buffer.
    //else copy zero bytes.
    uint32_t idx = AES256_BLOCK_SIZE - rngState->pos;
    memcpy(random, &rngState->buffer.bytes[rngState->pos], idx);

    //Init s.pos;
    rngState->pos = 0;

    //Copy full AES blocks.
    while((len - idx) >= AES256_BLOCK_SIZE)
    {
        perform_aes(&random[idx], rngState);
        idx += AES256_BLOCK_SIZE;
    }

    perform_aes(rngState->buffer.bytes, rngState);

    //Copy the tail.
    rngState->pos = len - idx;
    memcpy(&random[idx], rngState->buffer.bytes, rngState->pos);
}

uint8_t bit_scan_reverse(uint64_t val)
{
    //index is always smaller than 64.
    uint8_t index = 0;

    while(val != 0)
    {
        val >>= 1;
        index++;
    }

    return index;
}

uint32_t randModLen(const uint32_t len, aes_ctr_prf_state *rngState)
{
    uint64_t mask = (1 << bit_scan_reverse(len)) - 1;
    uint32_t retVal;

    //todo why are we looping?
    do
    {
        // genrate random number
        aes_ctr_prf((uint8_t*)&retVal, rngState, sizeof(retVal));

        // mask to stay in mod
        retVal &= mask;
    } while (retVal >= len);

    return retVal;
}
/**
 * @brief Randomly sample a bit array of given length with a given weight
 */
void randSample(uint8_t *result, const uint32_t weight, const uint32_t len, aes_ctr_prf_state *rngState)
{
    uint32_t randomPosition;
    uint64_t ctr = 0;

    // make sure the result is all zeros
    memset(result, 0, len);

    // set random bits in result until the weight matches
    do
    {
        // get random position
        randomPosition = randModLen(len, rngState);

        // check that this bit has not been set yet
        uint32_t arrayIndex = randomPosition / 8;
        uint8_t bitIndex = randomPosition % 8;
        if ((result[arrayIndex] >> bitIndex) & 0x01)
        {
            // set bit
            result[arrayIndex] |= 0x01 << (bitIndex);
            ctr++;
        }
    } while (ctr != weight);
}

void init_aes_ctr_prf_state(aes_ctr_prf_state* rngState, const uint8_t* seed)
{
    //set aes key schedule
    AES_set_encrypt_key(seed, AES256_KEY_BITS, &rngState->key);

    //init encrypt
    AES_encrypt(rngState->ctr.bytes, rngState->buffer.bytes, &rngState->key);
    rngState->ctr.qwords[0]++;
    rngState->pos = 0;
}
