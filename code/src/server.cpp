#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <unordered_map>
#include <stdexcept>
#include <glog/logging.h>
#include <sys/stat.h>

struct Message{
    char* msg;
    size_t msg_size;
};

pthread_t thread_id1;
std::unordered_map<int, std::string> Connections;

//class Disconnect : public std::exception { };

Message stringToChar(std::string &msg){
    size_t msg_size = msg.size();
    char *cmsg = new char[msg_size + 1];
    strcpy(cmsg, msg.c_str());
    cmsg[msg_size] = '\0';
    return {cmsg, msg_size + 1};
}

std::string receiveMsgFromClient(int socketID){
    int msg_size;
    int recvRes = recv(socketID, (char *) &msg_size, sizeof(int), 0);

    if (recvRes == 0) {
        throw std::runtime_error(std::to_string(socketID) + " disconnected");
    }
    std::cout << "recvRes^^^^^^^^ " << recvRes << std::endl;

    char *msg = new char[msg_size + 1];
    recv(socketID, msg, msg_size, 0);
    msg[msg_size] = '\0';
    return msg;
};

void sendMsgToClients(std::string &msg){
    auto newMsg = stringToChar(msg);
    for (auto [socketNum, username] : Connections) {
        send(socketNum, (char *) &newMsg.msg_size, sizeof(int), 0);
        send(socketNum, newMsg.msg, newMsg.msg_size, 0);
        //std::cout << newMsg.msg << " for " << username << std::endl;
    }
};

void welcome(int index){
    std::string welcomeMsg = "Привет! Сейчас в чате:";
    for (const auto& conn : Connections){
        /*if (i == index){
            continue;
        }*/
        welcomeMsg.append(" ");
        welcomeMsg.append(conn.second);
        welcomeMsg.append(",");
    }
    welcomeMsg[welcomeMsg.length() - 1] = '!';
    std::cout << welcomeMsg << std::endl;
    auto newMsg = stringToChar(welcomeMsg);
    send(index, (char *) &newMsg.msg_size, sizeof(int), 0);
    send(index, newMsg.msg, newMsg.msg_size, 0);
}

void *clientHandler(void *arg){
    int socketID = *((int *) arg);

    try{
        std::string username = receiveMsgFromClient(socketID);
        Connections[socketID] = username;
        welcome(socketID);

        while (true){
            std::string msg = Connections[socketID];
            msg.append(": ");
            msg.append(receiveMsgFromClient(socketID));
            sendMsgToClients(msg);
        }
    }
    catch (const std::runtime_error& e){
        shutdown(socketID, SHUT_RDWR);
        close(socketID);
        std::cerr << e.what() << std::endl;
        //std::cerr << Connections[socketID] << " disconnected\n";
        Connections.erase(socketID);
        for (const auto& conn : Connections){
            std::cout << "start " << conn.first << " " << conn.second << " end\n";
        }
        return nullptr;
    }
}

int main() {
    //mkdir("/logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    google::InitGoogleLogging("Server");
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    google::SetLogDestination(google::INFO, "log_file");

    LOG(WARNING) << "Это предупреждение.";
    LOG(ERROR) << "Это сообщение об ошибке.";
    LOG(INFO) << "Это информационное сообщение.";

    sockaddr_in addr{};
    socklen_t sizeOfAddr = sizeof(addr);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sListen,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    bind(sListen, (sockaddr *) &addr, sizeOfAddr);
    std::cout << "Waiting for client...\n";
    listen(sListen, SOMAXCONN);
    std::cout << "ListenSocketID: " << sListen << std::endl;

    int newConnection;
    while(true) {
        newConnection = accept(sListen, (sockaddr *) &addr, &sizeOfAddr);
        std::cout << "Connection socket: " << newConnection << std::endl;

        if (newConnection == 0) {
            std::cout << "Client connection error\n";
        } else {
            Connections[newConnection] = "name";
            for (auto conn : Connections){
                std::cout << "map: " << conn.first << " " << conn.second << std::endl;
            }
            pthread_create(&thread_id1, nullptr, &clientHandler, &newConnection);
            std::cout << "Client connected! Num of clients: " << Connections.size() << "\n";
        }
    }
}
