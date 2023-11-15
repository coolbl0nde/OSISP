#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

const int BUF_SIZE = 4096;
std::vector<SOCKET> clientSockets; // Вектор для хранения сокетов клиентов
std::vector<std::thread> threads;   // Вектор для хранения потоков

// Обработчик клиента
void handleClient(SOCKET clientSocket) {
    char buffer[BUF_SIZE];
    while (true) {
        ZeroMemory(buffer, BUF_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUF_SIZE, 0);
        if (bytesReceived <= 0) {
            // Обработка ошибок или закрытие соединения
            break;
        }
        // Пересылка сообщения другим клиентам
        for (const auto& otherClientSocket : clientSockets) {
            if (otherClientSocket != clientSocket) {
                send(otherClientSocket, buffer, bytesReceived, 0);
            }
        }
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    listen(serverSocket, 2);

    // Принимаем два соединения и создаем для них потоки
    for (int i = 0; i < 2; i++) {
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error accepting client: " << WSAGetLastError() << std::endl;
            continue;
        }

        clientSockets.push_back(clientSocket);
        threads.push_back(std::thread(handleClient, clientSocket));
    }

    // Ожидаем завершения работы потоков
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
