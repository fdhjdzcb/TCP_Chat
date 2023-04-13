#ifndef TCP_CHAT_SERVER_H
#define TCP_CHAT_SERVER_H

#include <unordered_map>
#include <stdexcept>

struct Message{
    char* msg;
    size_t msg_size;
};

Message stringToChar(std::string &msg);

std::string receiveMsgFromClient(int socketID);

void sendMsgToClients(std::string &msg);

void welcome(int socketID);

void *clientHandler(void *arg);

#endif //TCP_CHAT_SERVER_H
