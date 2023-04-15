#include "../inc/client.h"

int main() {
    google::InitGoogleLogging("Client");
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    FLAGS_log_dir = "logs";
    FLAGS_logtostderr = false;
    FLAGS_stderrthreshold = 3;
    FLAGS_logbufsecs = 1;
    google::SetLogDestination(google::INFO, "logs/client_log_file");

    int Connection;
    pthread_t receiver_th;

    sockaddr_in addr{};
    socklen_t sizeOfAddr = sizeof(addr);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    Connection = socket(AF_INET, SOCK_STREAM, 0);
    int connErrNum = connect(Connection, (sockaddr *) &addr, sizeOfAddr);
    if (connErrNum < 0) {
        LOG(ERROR) << "Ошибка подключения к серверу, номер ошибки: " << connErrNum;
        std::cout << "Error: failed connect to server. Errno: " << connErrNum << std::endl;
        return 1;
    }
    std::cout << "Connected!\n";
    LOG(INFO) << "Клиент подключен к серверу на сокете " << Connection;

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