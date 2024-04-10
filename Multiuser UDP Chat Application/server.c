#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8888
#define MAX_MESSAGE_SIZE 1024
#define MAX_CLIENTS 2 // Set the maximum number of clients

typedef struct {
    struct sockaddr_in address;
    SOCKET socket;
    int id;
} Client;

Client clients[MAX_CLIENTS];
HANDLE threads[MAX_CLIENTS];
int clientCount = 0;
CRITICAL_SECTION mutex;

DWORD WINAPI handleClient(LPVOID arg) {
    int id = *((int *)arg);
    char buffer[MAX_MESSAGE_SIZE];
    int bytesRead;

    while (1) {
        // Receive message from client
        bytesRead = recv(clients[id].socket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            printf("Receive failed for client %d: %d\n", id + 1, WSAGetLastError());
            break; // Exit the thread on receive failure
        }

        buffer[bytesRead] = '\0';
        printf("Client %d: %s", id + 1, buffer);

        // Broadcast the received message to all clients
        EnterCriticalSection(&mutex);
        for (int i = 0; i < clientCount; ++i) {
            if (i != id) {
                sendto(clients[i].socket, buffer, bytesRead, 0, (struct sockaddr*)&clients[i].address, sizeof(clients[i].address));
            }
        }
        LeaveCriticalSection(&mutex);

        // Send a message to the client (for demonstration purposes)
        sprintf(buffer, "Server: Message from server to Client %d\n", id + 1);
        sendto(clients[id].socket, buffer, strlen(buffer), 0, (struct sockaddr*)&clients[id].address, sizeof(clients[id].address));
    }

    // Cleanup
    closesocket(clients[id].socket);
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_MESSAGE_SIZE];
    int clientLen = sizeof(struct sockaddr_in);
    int bytesRead;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Initialize mutex
    InitializeCriticalSection(&mutex);

    printf("Server listening on port %d...\n", PORT);

    // Accept clients and handle them
    while (clientCount < MAX_CLIENTS) {
        // Receive message from client
        bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clients[clientCount].address, &clientLen);
        if (bytesRead == SOCKET_ERROR) {
            printf("Receive failed: %d\n", WSAGetLastError());
            break; // Exit the loop on receive failure
        }

        buffer[bytesRead] = '\0';
        printf("Client %d connected: %s", clientCount + 1, buffer);

        // Add client to the list
        clients[clientCount].socket = serverSocket;
        clients[clientCount].id = clientCount;
        threads[clientCount] = CreateThread(NULL, 0, handleClient, (LPVOID)&clients[clientCount].id, 0, NULL);
        if (threads[clientCount] == NULL) {
            printf("Thread creation failed: %d\n", GetLastError());
            break; // Exit the loop on thread creation failure
        }
        clientCount++;

        // Send a welcome message to the client (for demonstration purposes)
        sprintf(buffer, "Server: Welcome, you are Client %d\n", clientCount);
        sendto(clients[clientCount - 1].socket, buffer, strlen(buffer), 0, (struct sockaddr*)&clients[clientCount - 1].address, sizeof(clients[clientCount - 1].address));
    }

    // Cleanup
    for (int i = 0; i < clientCount; i++) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
        closesocket(clients[i].socket);
    }
    DeleteCriticalSection(&mutex);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
