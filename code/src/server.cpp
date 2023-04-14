#include "../../inc/server.h"
#include "../../inc/common.h"

pthread_t thread_id1;
std::unordered_map<int, std::string> Connections;


void configLOGS(){
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    FLAGS_stderrthreshold = 3;
    FLAGS_logbufsecs = 1;
    google::SetLogDestination(google::INFO, "logs/server_log_file");
}

void configPort(auto &addr){
    auto IP = "127.0.0.1";
    auto port = 1111;
    inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
}

int createListenSocket(auto &addr, auto &sizeOfAddr){
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sListen, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bind(sListen, (sockaddr *) &addr, sizeOfAddr);
    std::cout << "Waiting for client...\n";
    listen(sListen, SOMAXCONN);
    std::cout << "ListenSocketID: " << sListen << std::endl;
    LOG(INFO) << "Сервер слушает сокет " << sListen;
    return sListen;
}

Message stringToChar(std::string &msg) {
    size_t msg_size = msg.size();
    char *cmsg = new char[msg_size + 1];
    strcpy(cmsg, msg.c_str());
    cmsg[msg_size] = '\0';

    LOG(INFO) << "Сообщение преобразовано из std::string в структуру Message:"
              << "\nСообщение: " << cmsg
              << "\nРазмер: " << msg_size + 1
              << "\n";

    return {cmsg, msg_size + 1};
}


void addName(std::string &msg, int socketID){
    std::string msgWithName = Connections[socketID];
    msgWithName.append(": ");
    msg = msgWithName + msg;
}

std::string receiveMsgFromClient(int socketID) {
    int msg_size;
    int recvRes = recv(socketID, (char *) &msg_size, sizeof(int), 0);

    if (recvRes == 0) {
        throw std::runtime_error(std::to_string(socketID) + " disconnected");
    }

    char *msg = new char[msg_size + 1];
    recv(socketID, msg, msg_size, 0);
    msg[msg_size] = '\0';

    LOG(INFO) << "Получено сообщение с сокета " << socketID
              << "\nОт клиента: " << Connections[socketID]
              << "\nРезультат recv: " << recvRes
              << "\nСообщение: " << msg;

    return msg;
};

void sendMsgToClients(std::string &msg) {
    auto newMsg = stringToChar(msg);
    for (auto [socketNum, username]: Connections) {
        send(socketNum, (char *) &newMsg.msg_size, sizeof(int), 0);
        send(socketNum, newMsg.msg, newMsg.msg_size, 0);
    }
    LOG(INFO) << "Всем клиентам отправлено сообщение: " << newMsg.msg;
};

void addClientToList(std::string &msg, const auto &name){
    msg.append(" ");
    msg.append(name);
    msg.append(",");
}

std::string createWelcomeMsg(int socketID){
    std::string welcomeMsg = "Привет! Сейчас в чате:";
    for (const auto &conn: Connections) {
        addClientToList(welcomeMsg, conn.second);
    }
    welcomeMsg[welcomeMsg.length() - 1] = '!';
    return welcomeMsg;
}

void welcome(int socketID) {
    std::string welcomeMsg = createWelcomeMsg(socketID);

    LOG(INFO) << "Клиенту на сокете " << socketID << " отправлено приветственное сообщение:\n" << welcomeMsg;
    auto newMsg = stringToChar(welcomeMsg);
    send(socketID, (char *) &newMsg.msg_size, sizeof(int), 0);
    send(socketID, newMsg.msg, newMsg.msg_size, 0);
}

void deleteClient(int socketID){
    shutdown(socketID, SHUT_RDWR);
    close(socketID);
    LOG(WARNING) << "Клиент " << Connections[socketID] << " с сокета " << socketID << " отключен";
    Connections.erase(socketID);
    LOG(INFO) << "Сокет " << socketID << " закрыт";
}

void receiveNameFromClient(int socketID){
    std::string username;
    username = receiveMsgFromClient(socketID);
    if (!username.empty()) {
        Connections[socketID] = username;
        LOG(INFO) << "Клиенту на сокете " << socketID << " присвоено имя " << username;
        welcome(socketID);
    }
}

void *clientHandler(void *arg) {
    int socketID = *((int *) arg);

    try {
        receiveNameFromClient(socketID);
        while (true) {
            std::string msgFromClient = receiveMsgFromClient(socketID);

            LOG(INFO) << "От клиента " << Connections[socketID] << " на сокете " << socketID << " получено сообщение:\n"
                      << msgFromClient;

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

int main() {
    google::InitGoogleLogging("Server");
    configLOGS();

    LOG(INFO) << "Сервер начал работу";

    sockaddr_in addr{};
    socklen_t sizeOfAddr = sizeof(addr);
    configPort(addr);

    int sListen = createListenSocket(addr, sizeOfAddr);

    int newConnection;
    while (true) {
        newConnection = accept(sListen, (sockaddr *) &addr, &sizeOfAddr);
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
