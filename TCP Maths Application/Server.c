#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 8080

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET sockfd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    int num1, num2, choice, ans;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error("WSAStartup failed");
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Binding failed");
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == SOCKET_ERROR) {
        error("Listen failed");
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connection from client
    if ((new_socket = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) == INVALID_SOCKET) {
        error("Accept failed");
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive and process client requests
    while (1) {
        recv(new_socket, (char *)&num1, sizeof(int), 0);
        recv(new_socket, (char *)&num2, sizeof(int), 0);
        recv(new_socket, (char *)&choice, sizeof(int), 0);

        if (choice == 5) {
            break;
        }

        switch (choice) {
            case 1:
                ans = num1 + num2;
                break;
            case 2:
                ans = num1 - num2;
                break;
            case 3:
                ans = num1 * num2;
                break;
            case 4:
                if (num2 == 0) {
                    ans = 0; // Division by zero
                } else {
                    ans = num1 / num2;
                }
                break;
            default:
                ans = 0; // Invalid choice
        }

        send(new_socket, (char *)&ans, sizeof(int), 0);
    }

    printf("Client disconnected\n");

    // Close sockets
    closesocket(new_socket);
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
