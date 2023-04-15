#include "../inc/server.h"
#include "../inc/common.h"

TEST(ServerTests, addNameToList){
    std::string msg = "previous,";
    std::string newName = "newName";
    addClientToList(msg, newName);
    ASSERT_EQ(msg, "previous, newName,");
}

TEST(ServerTests, createEmptyWelcomeMsg){
    ASSERT_EQ("Привет! Сейчас в чате!", createWelcomeMsg());
}

TEST(ServerTests, emptyToMessageStruct){
    std::string testStr;
    Message testMSG = {(char*) "", 1};
    Message resMSG = stringToMessageStruct(testStr);
    ASSERT_EQ(*testMSG.msg, *resMSG.msg);
    ASSERT_EQ(testMSG.msg_size, resMSG.msg_size);
}

TEST(ServerTests, helloworldToMessageStruct) {
    std::string testStr = "hello, world";
    Message testMSG = {(char*) "hello, world", 13};
    Message resMSG = stringToMessageStruct(testStr);
    ASSERT_EQ(*testMSG.msg, *resMSG.msg);
    ASSERT_EQ(testMSG.msg_size, resMSG.msg_size);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

