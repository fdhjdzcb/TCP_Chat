#include "server.h"

void configLOGS() {
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    FLAGS_stderrthreshold = 3;
    FLAGS_logbufsecs = 1;
    google::SetLogDestination(google::INFO, "logs/server_log_file");
}

void configPort(sockaddr_in &addr) {
    auto IP = "127.0.0.1";
    auto port = 1111;
    inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
}

int createListenSocket(sockaddr_in &addr, const socklen_t &sizeOfAddr) {
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sListen, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bind(sListen, (sockaddr * ) & addr, sizeOfAddr);
    listen(sListen, SOMAXCONN);
    std::cout << "ListenSocketID: " << sListen << std::endl;
    LOG(INFO) << "Сервер слушает сокет " << sListen;
    return sListen;
}

void addName(std::string &msg, int socketID) {
    std::string msgWithName = Connections[socketID];
    msgWithName.append(": ");
    msg = msgWithName + msg;
}

std::string receiveMsgFromClient(int socketID) {
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

void sendMsg(int socketID, std::string &msg) {
    //auto newMsg = stringToMessageStruct(msg);
    size_t msg_size = msg.size();
    send(socketID, (char *) &msg_size, sizeof(int), 0);
    send(socketID, msg.c_str(), msg_size, 0);
}

void sendMsgToClients(std::string &msg) {
    for (auto [socketID, username]: Connections) {
        sendMsg(socketID, msg);
    }
    LOG(INFO) << "Всем клиентам отправлено сообщение: " << msg;
};

void addClientToList(std::string &msg, const auto &name) {
    msg.append(" ");
    msg.append(name);
    msg.append(",");
}

std::string createWelcomeMsg() {
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

void welcome(int socketID) {
    std::string welcomeMsg = createWelcomeMsg();
    sendMsg(socketID, welcomeMsg);
    LOG(INFO) << "Клиенту на сокете " << socketID << " отправлено приветственное сообщение:\n" << welcomeMsg;
}

void deleteClient(int socketID) {
    shutdown(socketID, SHUT_RDWR);
    close(socketID);
    LOG(WARNING) << "Клиент " << Connections[socketID] << " с сокета " << socketID << " отключен";
    Connections.erase(socketID);
    LOG(INFO) << "Сокет " << socketID << " закрыт";
}

void receiveNameFromClient(int socketID) {
    std::string username;
    username = receiveMsgFromClient(socketID);
    if (!username.empty()) {
        welcome(socketID);
        Connections[socketID] = username;
        LOG(INFO) << "Клиенту на сокете " << socketID << " присвоено имя " << username;
    }
}

void *clientHandler(void *arg) {
    int socketID = *((int *) arg);

    std::string getName = "Введите ваше имя: ";
    sendMsg(socketID, getName);
    try {
        receiveNameFromClient(socketID);
        while (true) {
            std::string msgFromClient = receiveMsgFromClient(socketID);

            LOG(INFO) << "От клиента " << Connections[socketID]
                      << " на сокете " << socketID
                      << " получено сообщение:\n" << msgFromClient;

            addName(msgFromClient, socketID);
            sendMsgToClients(msgFromClient);
        }
    }
    catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        deleteClient(socketID);
        return nullptr;
    }
}