#ifndef TCP_CHAT_SERVER_H
#define TCP_CHAT_SERVER_H

#include <unordered_map>
#include <stdexcept>

struct Message{
    char* msg;
    size_t msg_size;
};

void configLOGS();
void configPort(auto &addr);
int createListenSocket(auto &addr, auto &sizeOfAddr);

Message stringToChar(std::string &msg);

void addName(std::string &msg, int socketID);
std::string receiveMsgFromClient(int socketID);
void sendMsgToClients(std::string &msg);

void receiveNameFromClient(int socketID);
void addClientToList(std::string &msg, const auto &name);
std::string createWelcomeMsg(int socketID);
void welcome(int socketID);

void deleteClient(int socketID);

void *clientHandler(void *arg);

#endif //TCP_CHAT_SERVER_H
