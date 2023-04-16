#include "../inc/client.h"

int main(int argc, char **argv) {
    google::InitGoogleLogging("Client"); //инициализация библиотеки для логирования
    configLOGS();

    ::testing::InitGoogleTest(&argc, argv); //инициализация библиотеки для unit-тестирования

    LOG(INFO) << "Клиент начал работу";

    sockaddr_in addr{}; //настройка адреса сокета, который будет использоваться для соединения
    socklen_t sizeOfAddr = sizeof(addr);
    configPort(addr);

    int Connection = connectToServer(addr, sizeOfAddr); //подключение к серверу
    if (Connection == -1){
        LOG(ERROR) << "Не удалось подключиться к серверу";
        return 1;
    }

    pthread_t receiver_th;
    pthread_create(&receiver_th, nullptr, &receiveMsgFromServer, &Connection); //создание потока для получения сообщений

    std::string name;
    std::string msg;
    getMsg(name); // ввод имени
    sendMsg(Connection, name); //отправка имени серверу

    while (true) {
        getMsg(msg); //отправка обычного сообщения серверу
        std::cout << "\033[1A\033[2K"; //стирает введенное сообщение из консоли
        sendMsg(Connection, msg); //отправляет сообщение серверу
        LOG(INFO) << "Сообщение отправлено. Текст: " << msg;
    }
    pthread_cancel(receiver_th);
    //pthread_cancel(sender_th);
    close(Connection);
    return 0;
}