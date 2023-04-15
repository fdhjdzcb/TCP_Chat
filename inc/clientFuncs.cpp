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
};

void sendMsg(int socketID, std::string &msg) {
    size_t msg_size = msg.size();
    send(socketID, (char *) &msg_size, sizeof(int), 0);
    send(socketID, msg.c_str(), msg_size, 0);
}

void getMsg(std::string &msg){
    while (true){
        std::getline(std::cin, msg);
        if (msg.empty()) {
            std::cout << "\033[1A\033[2K";
            continue;
        }
        break;
    }
}