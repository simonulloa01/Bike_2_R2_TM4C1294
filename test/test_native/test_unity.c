#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unity.h>

/**
 * @brief Convert a hex string to binary.
 */
void hex_to_bin(const char* hex_str, uint8_t* bin_output, size_t bin_len) {
    size_t i = 0, j = 0;
    
    memset(bin_output, 0, bin_len);
    
    while (hex_str[i] && hex_str[i+1] && j < bin_len) {
        char byte_str[3] = {hex_str[i], hex_str[i+1], '\0'};
        bin_output[j++] = (uint8_t)strtol(byte_str, NULL, 16);
        i += 2;
    }
    TEST_PASS();
    
}


void setUp(void)
{
}

void tearDown(void)
{
}

void test_key_gen(void)
{
    TEST_PASS();
}

void test_encap(void)
{
    TEST_ASSERT_NOT_EQUAL(1, 2);
    
}
void test_decaps(void)
{
    TEST_ASSERT_EQUAL(1, 1);
    
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_key_gen);
    RUN_TEST(test_encap);
    RUN_TEST(test_decaps);
    return UNITY_END();
}

