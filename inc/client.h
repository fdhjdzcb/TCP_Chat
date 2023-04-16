#ifndef TCP_CHAT_CLIENT_H
#define TCP_CHAT_CLIENT_H

#include "common.h"

void* receiveMsgFromServer(void *arg);

int createSocket();

int connectToServerBySocket(int socketID, sockaddr_in &addr);

int connectToServer(sockaddr_in &addr);

void configLOGS();

void configPort(sockaddr_in &addr);

void sendMsg(int socketID, std::string &msg);

std::string getMsg();

#endif //TCP_CHAT_CLIENT_H
