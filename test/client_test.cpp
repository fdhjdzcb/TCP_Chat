#include "../inc/client.h"

TEST(ClientTests, createSocket){
    ASSERT_TRUE(createSocket() > 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}