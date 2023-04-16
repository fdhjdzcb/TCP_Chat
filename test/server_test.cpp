#include "../inc/server.h"

TEST(ServerTests, addNameToList){
    std::string msg = "previous,";
    std::string newName = "newName";
    addClientToList(msg, newName);
    ASSERT_EQ(msg, "previous, newName,");
}

TEST(ServerTests, createEmptyWelcomeMsg){
    ASSERT_EQ("Привет! Сейчас в чате никого нет!", createWelcomeMsg());
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

