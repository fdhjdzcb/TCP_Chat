#include "server.h"

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

int createListenSocket(sockaddr_in &addr) { //создание слушающего сокета для прослушивания входящих соединений
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sListen, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    int bindRes = bind(sListen, (sockaddr * ) & addr, sizeof(addr));
    if (bindRes == -1){
        LOG(FATAL) << "Не удалось связать адрес и порт сокета с указанным идентификатором sListen";
    }

    listen(sListen, SOMAXCONN);
    std::cout << "ListenSocketID: " << sListen << std::endl;
    LOG(INFO) << "Сервер слушает сокет " << sListen;
    return sListen;
}

void addName(std::string &msg, int socketID) { //добавляет имя пользователя к сообщению перед отправкой
    std::string msgWithName = Connections[socketID];
    msgWithName.append(": ");
    msg = msgWithName + msg; //msg == "Name: текст сообщения"
}

std::string receiveMsgFromClient(int socketID) { //получает сообщение от клиента по указанному идентификатору сокета socketID
    int msg_size;
    int recvSizeRes = recv(socketID, (char *) &msg_size, sizeof(int), 0);

    if (recvSizeRes == 0 or msg_size <= 0) {
        throw std::runtime_error("Client from socket " + std::to_string(socketID) + " disconnected");
    }

    char *msg = new char[msg_size + 1];
    recv(socketID, msg, msg_size, 0);
    msg[msg_size] = '\0';

    LOG(INFO) << "Получено сообщение с сокета " << socketID
              << "\nРезультат recv: " << recvSizeRes
              << "\nСообщение: " << msg;

    return msg;
};

void sendMsg(int socketID, std::string &msg) { //отправляет сообщение msg клиенту по указанному идентификатору сокета socketID
    size_t msg_size = msg.size();
    send(socketID, (char *) &msg_size, sizeof(int), 0);
    send(socketID, msg.c_str(), msg_size, 0);
}

void sendMsgToClients(std::string &msg) { //отправляет итоговое сообщение msg всем клиентам
    for (auto [socketID, username]: Connections) {
        sendMsg(socketID, msg);
    }
    LOG(INFO) << "Всем клиентам отправлено сообщение: " << msg;
};

void addClientToList(std::string &msg, const auto &name) { //добавляет имя очередного клиента к сообщению
    msg.append(" ");
    msg.append(name);
    msg.append(",");
}

std::string createWelcomeMsg() { //создает привественное сообщение в формате "Привет! Сейчас в чате: name1, name2, ..., nameN"
    std::string welcomeMsg;
    if (Connections.empty()){
        welcomeMsg = "Привет! Сейчас в чате никого нет!";
    } else {
        welcomeMsg = "Привет! Сейчас в чате:";
        for (const auto &conn: Connections) {
            addClientToList(welcomeMsg, conn.second);
        }
        welcomeMsg[welcomeMsg.length() - 1] = '!';
    }
    return welcomeMsg;
}

void welcome(int socketID) { //отправляет приветственное сообщение клиенту
    std::string welcomeMsg = createWelcomeMsg();
    sendMsg(socketID, welcomeMsg);
    LOG(INFO) << "Клиенту на сокете " << socketID << " отправлено приветственное сообщение:\n" << welcomeMsg;
}

void deleteClient(int socketID) { //закрывает сокет и удаляет клиента с указанным идентификатором socketID
    shutdown(socketID, SHUT_RDWR);
    close(socketID);
    LOG(WARNING) << "Клиент " << Connections[socketID] << " с сокета " << socketID << " отключен";

    Connections.erase(socketID);
    LOG(INFO) << "Сокет " << socketID << " закрыт";
}

void receiveNameFromClient(int socketID) { //принимает имя пользователя от клиента, который подключился к указанному идентификатору сокета socketID
    std::string username;
    username = receiveMsgFromClient(socketID);
    if (!username.empty()) {
        welcome(socketID);
        Connections[socketID] = username;

        LOG(INFO) << "Клиенту на сокете " << socketID << " присвоено имя " << username;
        LOG(INFO) << "Общее количество клиентов: " << Connections.size();
    }
}

void *clientHandler(void *arg) { //принимает и отправляет сообщения
    int socketID = *((int *) arg);

    std::string getName = "Введите ваше имя: ";
    sendMsg(socketID, getName);

    try {
        receiveNameFromClient(socketID); //получает имя клиента
        while (true) {
            std::string msgFromClient = receiveMsgFromClient(socketID); //получает сообщение

            LOG(INFO) << "От клиента " << Connections[socketID]
                      << " на сокете " << socketID
                      << " получено сообщение:\n" << msgFromClient;

            addName(msgFromClient, socketID); //добавляет к сообщению имя отправителя
            sendMsgToClients(msgFromClient); //отправляет всем клиентам
        }
    }
    catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl; // "Client from socket N disconnected"
        deleteClient(socketID); //закрывает сокет и удаляет клиента
        return nullptr;
    }
}