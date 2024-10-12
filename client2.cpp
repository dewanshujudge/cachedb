#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <winsock2.h>        // Windows socket library
#include <ws2tcpip.h>        // Additional Winsock utilities for address conversion
#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

static void die(const char *msg) {
    int err = WSAGetLastError();   // Windows version of errno
    fprintf(stderr, "[%d] %s\n", err, msg);
    exit(1);   // Use exit instead of abort to ensure proper cleanup in Windows
}

int t[32];
int main() {
    // Step 1: Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        die("WSAStartup()");
    }
    
   
    // Step 2: Create the socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        die("socket()");
    }

    // Step 3: Set up the server address (127.0.0.1:1234)
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);  // Use htons for port in Winsock
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1

    // Step 4: Connect to the server
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv == SOCKET_ERROR) {
        die("connect()");
    }

    // Step 5: Send a message to the server
    char msg[] = "Around the";
    if (send(fd, msg, strlen(msg), 0) == SOCKET_ERROR) {
        die("send()");
    }

    // Step 6: Receive a response from the server
    char rbuf[64] = {};
    int n = recv(fd, rbuf, sizeof(rbuf) - 1, 0);
    if (n == SOCKET_ERROR) {
        die("recv()");
    }

    // Print the server's response
    printf("server says: %s\n", rbuf);

    // Step 7: Close the socket
    closesocket(fd);  // Use closesocket() instead of close()

    // Step 8: Clean up Winsock
    WSACleanup();

    return 0;
}
