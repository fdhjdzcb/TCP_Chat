#ifndef TCP_CHAT_CLIENT_H
#define TCP_CHAT_CLIENT_H

#include "common.h"

void* receiveMsgFromServer(void *arg);

int connectToServer(sockaddr_in &addr, const socklen_t &sizeOfAddr);

void configLOGS();

void configPort(sockaddr_in &addr);

void sendMsg(int socketID, std::string &msg);

void getMsg(std::string &msg);

#endif //TCP_CHAT_CLIENT_H
