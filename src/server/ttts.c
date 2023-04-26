#include "ttts.h"

void *client_handler(void *arg);

int main(int argc, char** argv) {
    
    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Declare local variables
    int port, server_socket, client_socket, addr_size, player_count = 0;
    Player* players = malloc(sizeof(Player) * MAX_PLAYER_COUNT);
    Player* waiting = NULL;
    // int  client_count = 0;

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

        player_count = player_count + 1;

        // Read the client's message
        char recvline[MAX_LINE_LEN];
        memset(recvline, 0, MAX_LINE_LEN);
        check(read(client_socket, recvline, MAX_LINE_LEN - 1), "Failed to read from socket");
        printf("Client message: %s\n", recvline);

        // if (player_count % 2 == 0) {
        //     int *arg = malloc(sizeof(int) * 2);
        //     arg[0] = client_socket-1;
        //     arg[1] = client_socket;
        //     pthread_create(&tid[player_count/2-1], NULL, client_handler, (void *)arg);
        // }

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

void handleTwoClients(int socket1, int socket2);

void *client_handler(void *arg) {
    int client_sockets[2];
    client_sockets[0] = ((int *)arg)[0];
    client_sockets[1] = ((int *)arg)[1];
    free(arg);
    
    // handle clients
    printf("Handling two clients\n");
    // call handleTwoClients() function here
    printf("Two clients found, making a tictac toe game\n");

    handleTwoClients(client_sockets[0], client_sockets[1]);

    // close sockets
    close(client_sockets[0]);
    close(client_sockets[1]);
    
    return NULL;
}

void handleTwoClients(int socket1, int socket2) {
    fd_set readfds; // File descriptor set for select()
    char buffer[BUFFER_SIZE];
    bool running = true;

    while (running) {
        FD_ZERO(&readfds); // Clear the file descriptor set
        FD_SET(socket1, &readfds); // Add socket1 to the set
        FD_SET(socket2, &readfds); // Add socket2 to the set

        int max_fd = socket1 > socket2 ? socket1 : socket2;

        // Wait for either socket to become ready for reading
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select failed");
            break;
        }

        // Check if socket1 is ready for reading
        if (FD_ISSET(socket1, &readfds)) {
            ssize_t bytes_received = recv(socket1, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Message from socket1: %s\n", buffer);
            } else {
                running = false;
            }
        }

        // Check if socket2 is ready for reading
        if (FD_ISSET(socket2, &readfds)) {
            ssize_t bytes_received = recv(socket2, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Message from socket2: %s\n", buffer);
            } else {
                running = false;
            }
        }
    }
}
