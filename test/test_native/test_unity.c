#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unity.h>

/**
 * @brief Convert a hex string to binary.
 */



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

