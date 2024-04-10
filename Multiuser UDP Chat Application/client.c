#include <stdio.h>
#include <winsock2.h>


#define PORT 8888
#define MAX_MESSAGE_SIZE 1024


typedef struct {
    struct sockaddr_in serverAddr;
    SOCKET socket;
} Client;


Client client;


DWORD WINAPI receiveMessages(LPVOID arg) {
    char buffer[MAX_MESSAGE_SIZE];
    int bytesRead;


    while (1) {
        bytesRead = recv(client.socket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            printf("Receive failed: %d\n", WSAGetLastError());
            break;
        }


        if (bytesRead == 0) {
            printf("Connection closed by server\n");
            break;
        }


        buffer[bytesRead] = '\0';
        printf("Server: %s", buffer);
    }


    return 0;
}


int main() {
    HANDLE receiveThread;
    char message[MAX_MESSAGE_SIZE];


    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }


    // Create socket
    client.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client.socket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


    // Configure server address
    memset(&client.serverAddr, 0, sizeof(client.serverAddr));
    client.serverAddr.sin_family = AF_INET;
    client.serverAddr.sin_port = htons(PORT);


    // Convert IP address string to binary form
    if (inet_addr("127.0.0.1") == INADDR_NONE) {
        printf("Invalid IP address\n");
        closesocket(client.socket);
        WSACleanup();
        return 1;
    }
    client.serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    // Create a thread to receive messages
    receiveThread = CreateThread(NULL, 0, receiveMessages, NULL, 0, NULL);
    if (receiveThread == NULL) {
        printf("Thread creation failed: %d\n", GetLastError());
        closesocket(client.socket);
        WSACleanup();
        return 1;
    }


    // Send and receive messages
    while (1) {
        // Get user input
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);


        // Send message to server
        int bytesSent = sendto(client.socket, message, strlen(message), 0,
            (struct sockaddr*)&client.serverAddr,
            sizeof(client.serverAddr));
        if (bytesSent == SOCKET_ERROR) {
            printf("Send failed: %d\n", WSAGetLastError());
            break;
        }
    }


    closesocket(client.socket);
    WSACleanup();


    return 0;
}
