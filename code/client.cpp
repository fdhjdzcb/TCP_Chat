#include "../inc/client.h"

int main(int argc, char **argv) {
    google::InitGoogleLogging("Client");
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv);

    LOG(INFO) << "Клиент начал работу";

    sockaddr_in addr{};
    socklen_t sizeOfAddr = sizeof(addr);
    configPort(addr);

    int Connection = connectToServer(addr, sizeOfAddr);
    if (Connection == -1){
        return 1;
    }

    pthread_t receiver_th;
    pthread_create(&receiver_th, nullptr, &receiveMsgFromServer, &Connection);

    std::string name;
    std::string msg;
    getMsg(name);
    sendMsg(Connection, name); //send name

    while (true) {
        getMsg(msg); //send ordinary message
        std::cout << "\033[1A\033[2K"; //стереть введенное сообщение
        sendMsg(Connection, msg);
        LOG(INFO) << "Сообщение отправлено. Текст: " << msg;
    }
    pthread_cancel(receiver_th);
    //pthread_cancel(sender_th);
    close(Connection);
    return 0;
}