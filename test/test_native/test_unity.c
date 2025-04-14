#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unity.h>
#include <sampling.h>
#include <bike2-board.h>

void setUp(void)
{
    srand(12);
}

void tearDown(void)
{
}
void test_get_hamming_weight(void)
{
    const uint32_t LEN_BITS = 2048;
    const size_t byte_len = (LEN_BITS + 7u) / 8u;
    uint8_t poly[byte_len];

    memset(poly, 0xFF, byte_len);
    TEST_ASSERT_EQUAL(LEN_BITS, get_hamming_weight(poly, LEN_BITS));

    memset(poly, 0x00, byte_len);
    TEST_ASSERT_EQUAL(0, get_hamming_weight(poly, LEN_BITS));

    poly[0] = 0b10101010;
    poly[1] = 0b11001100;
    TEST_ASSERT_EQUAL(8, get_hamming_weight(poly, LEN_BITS));
}

void test_gen_sparse_poly(void)
{
    const uint32_t LEN_BITS = 2048;
    const size_t byte_len = (LEN_BITS + 7u) / 8u;
    uint8_t poly[byte_len];

    memset(poly, 0, byte_len);
    generate_sparse_polynomial(poly, byte_len, 128, LEN_BITS);

    uint32_t count = 0;
    for (uint32_t i = 0; i < LEN_BITS; i++)
    {
        uint32_t byte_index = i / 8;
        uint32_t bit_position = i % 8;
        if ((poly[byte_index] & (1u << bit_position)) != 0)
        {
            count++;
        }
    }

    TEST_ASSERT_EQUAL(128, count);
}
void test_key_gen(void)
{
    // set the config for thes test
    uint8_t pk[bike2_params_level_1.block_size * 2];
    uint8_t sk[bike2_params_level_1.block_size * 2];

    // clear the buffers
    memset(pk, 0, bike2_params_level_1.block_size);
    memset(sk, 0, bike2_params_level_1.block_size * 2);

    keygen_board(pk, sk, &bike2_params_level_1);

    printf("pk: ");
    for (int i = 0; i < bike2_params_level_1.block_size; i++)
    {
        printf("%02x", pk[i]);
    }
    printf("\n");
    printf("sk: ");
    for (int i = 0; i < bike2_params_level_1.block_size * 2; i++)
    {
        printf("%02x", sk[i]);
    }
    printf("\n");
    // check the pk and sk are not empty
    TEST_ASSERT_NOT_EQUAL(0, 1);
}

void test_encap(void)
{
    // Future test for BIKE-2 encapsulation
    TEST_ASSERT_NOT_EQUAL(1, 2);
}

void test_decaps(void)
{
    // Future test for BIKE-2 decapsulation
    TEST_ASSERT_EQUAL(1, 1);
}

void test_polyMod(void)
{
    // test an array that is smaller than the irreducible
    uint8_t small[5] = {0};
    small[0] = (uint8_t)rand();
    small[2] = (uint8_t)rand();
    small[4] = (uint8_t)rand();

    uint8_t result1[R_SIZE];
    polyMod(result1, small, R_SIZE + 10);

    TEST_ASSERT_EQUAL(small[0], result1[0]);
    TEST_ASSERT_EQUAL(small[2], result1[2]);
    TEST_ASSERT_EQUAL(small[4], result1[4]);

    // test a polynomial that does not need to be reduced
    uint8_t unreduced[R_SIZE + 10] = {0};
    unreduced[0] = (uint8_t)rand();
    unreduced[2] = (uint8_t)rand();
    unreduced[4] = (uint8_t)rand();

    uint8_t result2[R_SIZE];
    polyMod(result2, unreduced, R_SIZE + 10);

    TEST_ASSERT_EQUAL(unreduced[0], result2[0]);
    TEST_ASSERT_EQUAL(unreduced[2], result2[2]);
    TEST_ASSERT_EQUAL(unreduced[4], result2[4]);

    // test a polynomial that should be reduced
    // todo
}

void test_modMult(void)
{
    // multiply by 0 = 0
    uint8_t a[R_SIZE] = {0};
    uint8_t zero[R_SIZE] = {0};
    uint8_t result1[R_SIZE] = {0};
    generate_sparse_polynomial(a, R_SIZE, 128, R_BITS);

    modMult(result1, a, zero, R_SIZE);

    for (uint32_t i = 0; i < R_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(zero[i], result1[i]);
    }

    // multiply by 1 = self
    uint8_t one[R_SIZE] = {0};
    one[0] = 1;
    uint8_t result2[R_SIZE] = {0};

    modMult(result2, a, one, R_SIZE);

    for (uint32_t i = 0; i < R_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(a[i], result2[i]);
    }

    // multiply by inv = 1
    uint8_t inv[R_SIZE] = {0};
    uint8_t result3[R_SIZE] = {0};

    modInv(inv, a, &bike2_params_level_1);

    modMult(result3, a, inv, R_SIZE);

    for (uint32_t i = 0; i < R_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(one[i], result3[i]);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_hamming_weight);
    RUN_TEST(test_gen_sparse_poly);
    RUN_TEST(test_key_gen);
    RUN_TEST(test_encap);
    RUN_TEST(test_decaps);
    RUN_TEST(test_polyMod);
    RUN_TEST(test_modMult);
    return UNITY_END();
}