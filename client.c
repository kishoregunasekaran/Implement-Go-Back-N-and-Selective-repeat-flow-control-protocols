#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 9090
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int frame_number = 0;
    int total_frames;
    int window_size;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection to server failed. Error code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Client connected to server.\n");

    // Get the total number of frames and window size from the user
    printf("Enter the total number of frames to send: ");
    scanf("%d", &total_frames);

    printf("Enter the window size: ");
    scanf("%d", &window_size);

    // Send frames based on window size
    while (frame_number < total_frames) {
        int send_count = 0;

        while (send_count < window_size && frame_number < total_frames) {
            snprintf(buffer, sizeof(buffer), "Frame %d", frame_number);
            printf("Client sending: %s\n", buffer);
            send(client_socket, buffer, strlen(buffer) + 1, 0);
            send_count++;
            frame_number++;
        }

        // Receive ACKs for the sent frames
        for (int i = 0; i < send_count; i++) {
            recv(client_socket, buffer, sizeof(buffer), 0);
            printf("Client received: %s\n", buffer);
        }
    }

    // Cleanup
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
