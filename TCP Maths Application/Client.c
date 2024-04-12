#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
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
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Connection failed");
    }

    printf("Connected to server\n");

    // Send and receive data
    while (1) {
        printf("Enter Number 1: ");
        scanf("%d", &num1);
        send(sockfd, (char *)&num1, sizeof(int), 0);

        printf("Enter Number 2: ");
        scanf("%d", &num2);
        send(sockfd, (char *)&num2, sizeof(int), 0);

        printf("Enter Your Choice\n1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Exit\n");
        scanf("%d", &choice);
        send(sockfd, (char *)&choice, sizeof(int), 0);

        if (choice == 5) {
            break;
        }

        recv(sockfd, (char *)&ans, sizeof(int), 0);
        printf("Server: The answer is: %d\n", ans);
    }

    // Close socket
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
