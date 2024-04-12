#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);

    SOCKET sockfd;
    struct sockaddr_in server_addr, client_addr;
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
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("[-] Bind error");
        exit(1);
    }

    addr_size = sizeof(client_addr);
    n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &addr_size);
    if (n > 0) {
        buf[n] = '\0';
        printf("[+] Data Received: %s\n", buf);
        
        // Reverse the string
        int len = strlen(buf);
        for(int i = 0; i < len / 2; i++) {
            char temp = buf[i];
            buf[i] = buf[len - i - 1];
            buf[len - i - 1] = temp;
        }
        
        // Send reversed string back to client
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, addr_size);
        printf("[+] Reversed Data Sent: %s\n", buf);
    } else {
        perror("[-] Receive error");
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
