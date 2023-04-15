#include "gtest/gtest.h"
#include "../../../inc/server.h"

TEST(ServerTests, num1) {
    std::string testStr = "hello, world";
    Message testMSG = {(char *) "hello, world", 12};
    ASSERT_EQ(testMSG.msg, stringToMessageStruct(testStr).msg);
    ASSERT_EQ(testMSG.msg_size, stringToMessageStruct(testStr).msg_size);
}