#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unity.h>
#include <sampling.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_gen_sparse_poly(void) {
    uint8_t* poly = generate_sparse_polynomial(128, 2048);
    //count the number of bits set
    uint32_t count = 0;
    for (uint32_t i = 0; i < 2048; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_position = i % 8;
        if ((poly[byte_index] & (1 << bit_position)) != 0) {
            count++;
        }
    }
    TEST_ASSERT_EQUAL(128, count);
}

void test_encap(void) {
    // Future test for BIKE-2 encapsulation
    TEST_ASSERT_NOT_EQUAL(1, 2);
}

void test_decaps(void) {
    // Future test for BIKE-2 decapsulation
    TEST_ASSERT_EQUAL(1, 1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_gen_sparse_poly);
    RUN_TEST(test_encap);
    RUN_TEST(test_decaps);
    return UNITY_END();
}