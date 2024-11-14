#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 9090               // Change the port if needed
#define BUFFER_SIZE 1024
#define TIMEOUT 3000            // Timeout for ACKs in milliseconds

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int addr_len = sizeof(client_addr);
    int expected_frame = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Bind to localhost

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        int error_code = WSAGetLastError();
        printf("Bind failed. Error code: %d\n", error_code);

        if (error_code == WSAEADDRINUSE) {
            printf("Port %d is already in use. Try changing the port number.\n", PORT);
        }

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        printf("Listen failed. Error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Server: Waiting for client connections...\n");

    // Accept a connection from a client
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket < 0) {
        printf("Accept failed. Error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server: Client connected.\n");

    // Loop to receive frames and send ACKs
    while (1) {
        int recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (recv_size > 0) {
            printf("Server received: %s\n", buffer);

            // Send ACK for the received frame
            snprintf(buffer, sizeof(buffer), "ACK for Frame %d", expected_frame);
            send(client_socket, buffer, strlen(buffer) + 1, 0);
            printf("Server sent: %s\n", buffer);
            expected_frame++;
        } else if (recv_size == 0) {
            printf("Connection closed by client.\n");
            break;
        } else {
            printf("Error receiving data. Error code: %d\n", WSAGetLastError());
            break;
        }
    }

    // Cleanup
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
