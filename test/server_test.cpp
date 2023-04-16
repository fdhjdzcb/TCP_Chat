#include "../inc/server.h"

TEST(ServerTests, addClientNameToList){
    std::string msg = "previous,";
    std::string newName = "newName";
    addClientToList(msg, newName);
    ASSERT_EQ(msg, "previous, newName,");
}

TEST(ServerTests, createEmptyWelcomeMsg){
    ASSERT_EQ("Привет! Сейчас в чате никого нет!", createWelcomeMsg());
}

TEST(ServerTests, createNameWelcomeMsg){
    Connections[0] = "name";
    ASSERT_EQ("Привет! Сейчас в чате: name!", createWelcomeMsg());
}

TEST(ServerTests, addNameUsernameFor1){
    Connections[1] = "Username";
    std::string msg = "message";
    addName(msg, 1);
    ASSERT_EQ("Username: message", msg);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}