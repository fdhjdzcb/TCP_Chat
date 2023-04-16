#include "client.h"

void* receiveMsgFromServer(void *arg){
    int Connection = *((int *) arg);
    int msg_size;
    while(true){
        if(recv(Connection, (char *) &msg_size, sizeof(int), 0) == -1)
            continue;
        char *msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        if(recv(Connection, msg, msg_size, 0) == -1)
            continue;
        LOG(INFO) << "Получено сообщение: " << msg;
        std::cout << msg << std::endl;
        delete[] msg;
    }
}

int connectToServer(sockaddr_in &addr, const socklen_t &sizeOfAddr){ //подключение к серверу
    int socketID = socket(AF_INET, SOCK_STREAM, 0); //создание сокета
    if (socketID == -1){
        LOG(ERROR) << "Не удалось создать сокет";
        return -1;
    }

    int connErrNum = connect(socketID, (sockaddr *) &addr, sizeOfAddr); //подключение к сокету
    if (connErrNum == -1) {
        LOG(ERROR) << "Ошибка подключения к серверу, номер ошибки: " << connErrNum;
        std::cout << "Error: failed connect to server. Errno: " << connErrNum << std::endl;
        return -1;
    }

    std::cout << "Connected!\n";
    LOG(INFO) << "Клиент подключен к серверу на сокете " << socketID; //все ОК
    return socketID;
}

void configLOGS() { //настройка логирования с помощью библиотеки Google Logging
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    FLAGS_stderrthreshold = 3;
    FLAGS_logbufsecs = 1;
    google::SetLogDestination(google::INFO, "logs/server_log_file");
}

void configPort(sockaddr_in &addr) { //настройка адреса сокета, который будет использоваться для соединения
    auto IP = "127.0.0.1";
    auto port = 1111;
    inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
}


void sendMsg(int socketID, std::string &msg) { //отправляет сообщение msg серверу
    size_t msg_size = msg.size();
    send(socketID, (char *) &msg_size, sizeof(int), 0);
    send(socketID, msg.c_str(), msg_size, 0);
}

void getMsg(std::string &msg){ //получает введенное сообщение
    while (true){
        std::getline(std::cin, msg);
        if (msg.empty()) {
            std::cout << "\033[1A\033[2K"; //если пустая строка, стирает её из консоли
            continue;
        }
        break;
    }
}