#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include "command_handler.h"
#include "logger.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

#define PORT 1337
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
            perror("Accepting connection failed");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        size_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                printf("Connection closed by client\n");
            }
            else {
                perror("Error in recv");
            }
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesRead] = '\0';

        char command[16];
        char data[BUFFER_SIZE];
        sscanf(buffer, "%s %[^\n]", command, data);

        char response[BUFFER_SIZE];
        handle_command(command, data, response);

        send(clientSocket, response, strlen(response), 0);

        logAIUsage(buffer, response); // Log AI usage

        close(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
