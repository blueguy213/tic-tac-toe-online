#include "ttts.h"

int main(int argc, char** argv) {
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port, server_socket, client_socket, addr_size;

    port = atoi(argv[1]);
    server_socket = check(socket(AF_INET, SOCK_STREAM, 0), "Failed to create socket");

    SA_IN server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Failed to bind");
    check(listen(server_socket, SERVER_BACKLOG), "Failed to listen");

    printf("Server is listening on port %d...\n", port);

    while (1) {

        printf("Waiting for connections...\n");

        size_t addr_size = sizeof(SA_IN);
        check(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size), "Failed to accept connection");
        
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    return 0;
}

int check(int exp, const char* msg) {
    if (exp == SOCKET_ERROR) {
        err_and_kill(msg);
    }
    return exp;
}