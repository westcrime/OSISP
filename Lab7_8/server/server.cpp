#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 10

SOCKET clientSockets[MAX_CLIENTS];
int numClients = 0;

void SendToAllClients(const char* message, int senderIndex) {
    for (int i = 0; i < numClients; ++i) {
        if (i == senderIndex) {
            std::string new_message("User ");
            new_message += std::to_string(senderIndex) + " (You): ";
            new_message += message;
            send(clientSockets[i], new_message.c_str(), strlen(new_message.c_str()), 0);
        }
        else
        {
            std::string new_message("User ");
            new_message += std::to_string(senderIndex) + ": ";
            new_message += message;
            send(clientSockets[i], new_message.c_str(), strlen(new_message.c_str()), 0);
        }
    }
}

DWORD WINAPI ClientThread(LPVOID param) {
    int clientIndex = (int)param;
    char recvbuf[1024];

    while (true) {
        int recvResult = recv(clientSockets[clientIndex], recvbuf, sizeof(recvbuf), 0);
        if (recvResult > 0) {
            recvbuf[recvResult] = '\0';
            std::cout << "Received: " << recvbuf << std::endl;
            SendToAllClients(recvbuf, clientIndex);
        }
        else if (recvResult == 0) {
            std::cout << "Client " << clientIndex << " disconnected." << std::endl;
            closesocket(clientSockets[clientIndex]);
            break;
        }
        else {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSockets[clientIndex]);
            break;
        }
    }

    return 0;
}

int main() {
    WSADATA wsaData;
    int iResult;

    SOCKET listenSocket = INVALID_SOCKET;
    addrinfo* result = NULL;
    addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started, waiting for incoming connections..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        if (numClients < MAX_CLIENTS) {
            clientSockets[numClients++] = clientSocket;
            CreateThread(NULL, 0, ClientThread, (LPVOID)(numClients - 1), 0, NULL);
        }
        else {
            std::cout << "Too many clients. Connection rejected." << std::endl;
            closesocket(clientSocket);
        }
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
