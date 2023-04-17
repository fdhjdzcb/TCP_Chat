#include "client.h"

void configLOGS() { //настройка логирования с помощью библиотеки Google Logging
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    FLAGS_stderrthreshold = 3;
    FLAGS_logbufsecs = 1;
    google::SetLogDestination(google::INFO, "logs/client_log_file");
}

void configPort(sockaddr_in &addr) { //настройка адреса сокета, который будет использоваться для соединения
    auto IP = "127.0.0.1";
    auto port = 1111;
    inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
}

void* receiveMsgFromServer(void *arg){ //получение сообщений от сервера
    int Connection = *((int *) arg);
    int msg_size;

    while(true){
        int recvRes = recv(Connection, (char *) &msg_size, sizeof(int), 0); //получение размера сообщения
        switch (recvRes) {
            case -1: //сообщений нет
                continue;
            case 0: //сервер отключен
                shutdown(Connection, SHUT_RDWR);
                close(Connection);
                LOG(FATAL) << "Сервер отключен";
                break;
            default: //все ОК
                char *msg = new char[msg_size + 1];
                msg[msg_size] = '\0';
                recv(Connection, msg, msg_size, 0); //получение самого сообщения

                LOG(INFO) << "Получено сообщение: " << msg;

                std::cout << msg << std::endl;
                delete[] msg;
        }
    }
}

int createSocket(){ //создание сокета
    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID == -1){
        LOG(ERROR) << "Не удалось создать сокет";
    } else {
        LOG(INFO) << "Сокет создан, дескриптор: " << socketID;
    }
    return socketID;
}

int connectToServerBySocket(int socketID, sockaddr_in &addr){ //подключение к конкретному сокету
    int connRes = connect(socketID, (sockaddr *) &addr, sizeof(addr)); //подключение к сокету
    if (connRes == -1) {
        LOG(ERROR) << "Ошибка подключения к серверу, номер ошибки: " << connRes;
        std::cout << "Error: failed connect to server. Errno: " << connRes << std::endl;
    }

    return connRes;
}

int connectToServer(sockaddr_in &addr){ //подключение к серверу
    int socketID = createSocket();

    int connRes = connectToServerBySocket(socketID, addr);

    if (connRes == 0) {
        std::cout << "Connected!\n";
        LOG(INFO) << "Клиент подключен к серверу на сокете " << socketID; //все ОК
    }
    return socketID;
}

void sendMsg(int socketID, std::string &msg) { //отправляет сообщение msg серверу
    size_t msg_size = msg.size();
    send(socketID, (char *) &msg_size, sizeof(int), 0);
    send(socketID, msg.c_str(), msg_size, 0);
}

std::string getMsg(){ //получает введенное сообщение
    std::string msg;
    while (true){
        std::getline(std::cin, msg);
        if (msg.empty()) {
            std::cout << "\033[1A\033[2K"; //если пустая строка, стирает её из консоли
            continue;
        }
        break;
    }
    return msg;
}