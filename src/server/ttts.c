#include "ttts.h"
#define MAX_CLIENTS 100

void *client_handler(void *arg);

int main(int argc, char** argv) {
    
    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    pthread_t tid[MAX_CLIENTS];

    int port, server_socket, client_socket, addr_size;
    int  client_count = 0;

    // Convert port number to integer
    port = check(atoi(argv[1]), "Not a port");
    check(port > 0, "Invalid port number");

    // Create server socket
    server_socket = check(socket(AF_INET, SOCK_STREAM, 0), "Failed to create socket");

    // Set port number to listen on and protocol family to support
    SA_IN server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    // Bind and listen
    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Failed to bind");
    check(listen(server_socket, SERVER_BACKLOG), "Failed to listen");

    printf("Server is listening on port %d...\n", port);

    while (1) { // Loop forever waiting for connections

        printf("Waiting for connections...\n");

        // Accept connection
        size_t addr_size = sizeof(SA_IN);
        check(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size), "Failed to accept connection");

        printf("Client socket: %d\n", client_socket);
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        client_count = client_count+1;

        if (client_count % 2 == 0) {
            int *arg = malloc(sizeof(int) * 2);
            arg[0] = client_socket-1;
            arg[1] = client_socket;
            pthread_create(&tid[client_count/2-1], NULL, client_handler, (void *)arg);
        }

    }
    return 0;
}

// Check for errors and kill program if found
int check(int exp, const char* msg) {
    if (exp == SOCKET_ERROR) {
        err_and_kill(msg);
    }
    return exp;
}

void *client_handler(void *arg) {
    int client_sockets[2];
    client_sockets[0] = ((int *)arg)[0];
    client_sockets[1] = ((int *)arg)[1];
    free(arg);
    
    // handle clients
    printf("Handling two clients\n");
    // call handleTwoClients() function here
    printf("Two clients found, making a tictac toe game\n");


    // close sockets
    close(client_sockets[0]);
    close(client_sockets[1]);
    
    return NULL;
}
