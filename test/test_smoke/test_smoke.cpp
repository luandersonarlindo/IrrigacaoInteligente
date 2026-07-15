#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_harness_runs(void) {
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_harness_runs);
    return UNITY_END();
}
