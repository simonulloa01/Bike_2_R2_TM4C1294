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
    uint8_t pk[bike2_params_level_1.block_size];
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

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_hamming_weight);
    RUN_TEST(test_gen_sparse_poly);
    //RUN_TEST(test_key_gen);
    RUN_TEST(test_encap);
    RUN_TEST(test_decaps);
    return UNITY_END();
}