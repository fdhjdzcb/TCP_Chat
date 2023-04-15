#ifndef TCP_CHAT_CLIENT_H
#define TCP_CHAT_CLIENT_H

#include "common.h"

void* receiveMsgFromServer(void *arg);

void sendMsg(int socketID, std::string &msg);

void getMsg(std::string &msg);

#endif //TCP_CHAT_CLIENT_H
