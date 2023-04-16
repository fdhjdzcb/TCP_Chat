#ifndef TCP_CHAT_SERVER_H
#define TCP_CHAT_SERVER_H

#include <unordered_map>
#include <stdexcept>
#include "common.h"

extern pthread_t thread_id;
extern std::unordered_map<int, std::string> Connections;

void configLOGS();
void configPort(sockaddr_in &addr);
int createListenSocket(sockaddr_in &addr);

void addName(std::string &msg, int socketID);
std::string receiveMsgFromClient(int socketID);
void sendMsg(int socketID, std::string &msg);
void sendMsgToClients(std::string &msg);

void receiveNameFromClient(int socketID);
void addClientToList(std::string &msg, const auto &name);
std::string createWelcomeMsg();
void welcome(int socketID);

void deleteClient(int socketID);

void *clientHandler(void *arg);

#endif //TCP_CHAT_SERVER_H
