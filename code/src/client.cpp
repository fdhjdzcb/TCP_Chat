#include <../../../inc/client.h>
#include <../../../inc/common.h>

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

/*void* sendMsgToServer(void *arg){
    std::string msg1;
    while(true){
        std::getline(std::cin, msg1);
        int msg_size = msg1.size();
        send(Connection, (char *) &msg_size, sizeof(int), 0);
        send(Connection, msg1.c_str(), msg_size, 0);
        usleep(100);
    }
};*/

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

    std::string msg;
    std::cout << "Введите ваше имя: ";

    pthread_create(&receiver_th, nullptr, &receiveMsgFromServer, &Connection);
    //pthread_create(&sender_th, nullptr, &sendMsgToServer, nullptr);
    while (true) {
        std::getline(std::cin, msg);
        size_t msg_size = msg.size();
        send(Connection, (char *) &msg_size, sizeof(int), 0);
        send(Connection, msg.c_str(), msg_size, 0);
        LOG(INFO) << "Сообщение отправлено. Текст: " << msg;
        usleep(10);
    }
    pthread_cancel(receiver_th);
    //pthread_cancel(sender_th);
    close(Connection);
    return 0;
}