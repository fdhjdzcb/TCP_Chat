#include "../inc/client.h"

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

TEST(CreateSocketTest, BasicTest){
    ASSERT_TRUE(createSocket() > 0);
}

TEST(GetMsgTest, EmptyInput) {
    std::stringstream input("");
    std::stringstream output;
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());

    std::string msg = getMsg();
    EXPECT_EQ(msg, "");
}

TEST(GetMsgTest, NonEmptyInput) {
    std::stringstream input("Test message\n");
    std::stringstream output;
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());

    std::string msg = getMsg();
    EXPECT_EQ(msg, "Test message");
}

int main(int argc, char **argv) {
    google::InitGoogleLogging("ServerTests");
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}