#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

const int BUF_SIZE = 4096;

// Функция для обработки входящих сообщений
void receiveMessages(SOCKET clientSocket) {
    char buffer[BUF_SIZE];
    while (true) {
        ZeroMemory(buffer, BUF_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUF_SIZE, 0);
        if (bytesReceived > 0) {
            std::cout << "user1> " << std::string(buffer, 0, bytesReceived) << std::endl;
        }
        // В реальном приложении здесь должна быть проверка на ошибки и закрытие соединения при необходимости
    }
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    // Инициализация Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Создание клиентского сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Задание адреса и порта сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000); // Порт сервера
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // IP сервера

    // Подключение к серверу
    connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Запуск потока для приема сообщений
    std::thread receiveThread(receiveMessages, clientSocket);

    // Основной цикл для отправки сообщений
    std::string userInput;
    while (true) {
        std::getline(std::cin, userInput);
        if (userInput == "exit") {
            break; // Выход из цикла, если пользователь ввел "exit"
        }
        send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);
        // В реальном приложении здесь должна быть проверка на ошибки и закрытие соединения при необходимости
    }

    // Закрытие потока и сокета
    receiveThread.detach();
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
