#include "networking.h"

int tcp_server_socket;
struct sockaddr_in tcp_server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);

int tcp_start_server() {
    // int tcp_server_socket;
    // struct sockaddr_in tcp_server_addr, client_addr;
    // socklen_t addr_len = sizeof(client_addr);

    // Create a server socket
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_port = htons(PORT);
    tcp_server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket to a specific address and port
    if (bind(tcp_server_socket, (struct sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(tcp_server_socket, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);
    // Accept a connection from a client
    tcp_server_socket = accept(tcp_server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (tcp_server_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void tcp_handle_request()
{
    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    receive_message(tcp_server_socket);
}
