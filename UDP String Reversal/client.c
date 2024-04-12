#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        exit(0);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    SOCKET sockfd;
    struct sockaddr_in server_addr;
    char buf[1024];
    int addr_size;
    int n;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == INVALID_SOCKET) {
        perror("[-] Socket error");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    printf("Enter a string: ");
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf) - 1] = '\0'; // Remove newline character from input

    sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[+] Data Sent: %s\n", buf);

    // Receive reversed string from server
    n = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    if (n > 0) {
        buf[n] = '\0';
        printf("[+] Reversed Data Received: %s\n", buf);
    } else {
        perror("[-] Receive error");
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
