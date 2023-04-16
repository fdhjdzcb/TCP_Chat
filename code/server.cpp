#include "../inc/server.h"

pthread_t thread_id;
std::unordered_map<int, std::string> Connections;

int main(int argc, char **argv) {
    google::InitGoogleLogging("Server"); //инициализация библиотеки для логирования
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv); //инициализация библиотеки для unit-тестирования

    LOG(INFO) << "Сервер начал работу";

    sockaddr_in addr{}; //настройка адреса сокета, который будет использоваться для соединения
    socklen_t sizeOfAddr = sizeof(addr);
    configPort(addr);

    int sListen = createListenSocket(addr);

    int newConnection; //переменная для первичной записи нового клиента
    while (true) {
        newConnection = accept(sListen, (sockaddr * ) &addr, &sizeOfAddr);
        std::cout << "Connection socket: " << newConnection << std::endl;

        if (newConnection == 0) {
            std::cout << "Client connection error\n";
            LOG(ERROR) << "Ошибка подключения клиента";
        } else {
            pthread_create(&thread_id, nullptr, &clientHandler, &newConnection);
            LOG(INFO) << "Клиент подключен к сокету " << newConnection;
        }
    }
}
