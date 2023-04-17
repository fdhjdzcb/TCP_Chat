#include "../inc/server.h"

std::unordered_map<int, std::string> Connections;

TEST(LoggingTest, ConfigLogs) {
    configLOGS();
    ASSERT_EQ(access("logs", F_OK), 0);
    ASSERT_EQ(FLAGS_log_dir, "logs");
    ASSERT_FALSE(FLAGS_logtostderr);
    ASSERT_EQ(FLAGS_stderrthreshold, 3);
    ASSERT_EQ(FLAGS_logbufsecs, 1);
}

TEST(SocketTest, ConfigPort) {
    sockaddr_in addr{};
    configPort(addr);
    ASSERT_EQ(std::string(inet_ntoa(addr.sin_addr)), "127.0.0.1");
    ASSERT_EQ(ntohs(addr.sin_port), 1111);
    ASSERT_EQ(addr.sin_family, AF_INET);
}

TEST(SocketTest, CreateListenSocket) {
    sockaddr_in addr{};
    configPort(addr);
    int sListen = createListenSocket(addr);
    ASSERT_GT(sListen, 0);
    char buf[32];
    ASSERT_EQ(recv(sListen, buf, sizeof(buf), MSG_PEEK), -1);
    close(sListen);
}

TEST(ServerTests, addClientNameToList){
    std::string msg = "previous,";
    std::string newName = "newName";
    addClientToList(msg, newName);
    ASSERT_EQ(msg, "previous, newName,");
}

TEST(ServerTests, addNameUsernameFor1){
    Connections[1] = "Username";
    std::string msg = "message";
    addName(msg, 1);
    ASSERT_EQ("Username: message", msg);
}

TEST(CreateWelcomeMsgTest, EmptyConnectionsTest) {
    Connections.clear();
    std::string welcomeMsg = createWelcomeMsg();
    ASSERT_EQ(welcomeMsg, "Привет! Сейчас в чате никого нет!");
}

TEST(CreateWelcomeMsgTest, NonEmptyConnectionsTest) {
    Connections.clear();
    Connections[0] = "Боб";
    Connections[1] = "Алиса";
    std::string welcomeMsg = createWelcomeMsg();
    ASSERT_EQ(welcomeMsg, "Привет! Сейчас в чате: Алиса, Боб!");
}


int main(int argc, char **argv) {
    google::InitGoogleLogging("ServerTests");
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}