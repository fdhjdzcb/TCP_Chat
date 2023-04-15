#include "../inc/server.h"

pthread_t thread_id1;
std::unordered_map<int, std::string> Connections;

int main(int argc, char **argv) {
    google::InitGoogleLogging("Server");
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv);

    LOG(INFO) << "Сервер начал работу";

    sockaddr_in addr{};
    socklen_t sizeOfAddr = sizeof(addr);
    configPort(addr);

    int sListen = createListenSocket(addr, sizeOfAddr);

    int newConnection;
    while (true) {
        newConnection = accept(sListen, (sockaddr * ) & addr, &sizeOfAddr);
        std::cout << "Connection socket: " << newConnection << std::endl;

        if (newConnection == 0) {
            std::cout << "Client connection error\n";
            LOG(ERROR) << "Ошибка подключения клиента";
        } else {
            pthread_create(&thread_id1, nullptr, &clientHandler, &newConnection);
            std::cout << "Client connected! Num of clients: " << Connections.size() << "\n";
            LOG(INFO) << "Клиент подключен к сокету " << newConnection;
            LOG(INFO) << "Общее количество клиентов: " << Connections.size();
        }
    }
}
