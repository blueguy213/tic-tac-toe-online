#include "ttts.h"
#define MAX_CLIENTS 100

void *client_handler(void *arg);
int send_to_socket(int socket, const char *str);

int main(int argc, char** argv) {
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    pthread_t tid[MAX_CLIENTS];

    int port, server_socket, client_socket, addr_size;
    int  client_count = 0;

    lobby_t* lobby = new_lobby(10);

    port = atoi(argv[1]);
    server_socket = check(socket(AF_INET, SOCK_STREAM, 0), "Failed to create socket");

    SA_IN server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    player_t* previous_player;
    
    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Failed to bind");
    check(listen(server_socket, SERVER_BACKLOG), "Failed to listen");

    printf("Server is listening on port %d...\n", port);

    while (1) {

        printf("Waiting for connections...\n");

        size_t addr_size = sizeof(SA_IN);
        check(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size), "Failed to accept connection");

        printf("Client socket: %d\n", client_socket);
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Read the PLAY command from the client
        char play_command[256];
        read(client_socket, play_command, sizeof(play_command));

        printf("%s\n", play_command);

        // Parse the player's name
        char player_name[256];
        int play_number;

        sscanf(play_command, "PLAY|%d|%[^|]|", &play_number, player_name);

        // Create a new player
        player_t *newplayer = malloc(sizeof(player_t));
        newplayer = new_player('X', player_name, client_addr, client_socket, tid); // 'X' will be replaced with the proper role later

        print_player(*newplayer);

        // Send the WAIT response to the client
        char wait_response[] = "WAIT";
        write(client_socket, wait_response, strlen(wait_response));

        client_count = client_count + 1;

        if (client_count % 2 == 0) {
            player_t **arg = malloc(sizeof(player_t*) * 2);

            arg[0] = newplayer; // Pass the new player created for client_socket
            arg[1] = previous_player; // Pass the new player created for client_socket-1

            pthread_create(&tid[client_count / 2 - 1], NULL, client_handler, (void *)arg);
        } else {
            newplayer->role = 'O';
            previous_player = malloc(sizeof(player_t)); // Allocate memory for the previous player
            *previous_player = *newplayer;
        }
    }
    // Free everything
    free_lobby(lobby);
    close(server_socket);
    return 0;
}

int check(int exp, const char* msg) {
    if (exp == SOCKET_ERROR) {
        err_and_kill(msg);
    }
    return exp;
}

void handleTwoClients(player_t player1, player_t player2);

void *client_handler(void *arg) {
    player_t *players = malloc(sizeof(player_t) * 2); // Allocate memory for the players array

    players[0] = *((player_t**)arg)[0];
    players[1] = *((player_t**)arg)[1];

    free(arg);
    
    // handle clients
    printf("Handling two clients\n");
    // call handleTwoClients() function here
    printf("Two clients found, making a tictac toe game\n");

    handleTwoClients(players[0], players[1]);

    // close sockets
    close(players[0].socket);
    close(players[1].socket);

    free(players);
    
    return NULL;
}

void handleTwoClients(player_t player1, player_t player2) {
    fd_set readfds; 
    char buffer[BUFFER_SIZE];
    char reply_buffer[BUFFER_SIZE];

    // Initialize the game and assign roles to the players
    player1.role = 'X';
    player2.role = 'O';
    game_t* game = new_game(&player1, &player2);

    bool running = true;
    int socket1 = player1.socket;
    int socket2 = player2.socket;

    // Send the BEGN response to both players
    sprintf(reply_buffer, "BEGN|%c|%s|", player1.role, player2.name);
    send_to_socket(socket1, reply_buffer);
    sprintf(reply_buffer, "BEGN|%c|%s|", player2.role, player1.name);
    send_to_socket(socket2, reply_buffer);

    while (running) {
        FD_ZERO(&readfds); 
        FD_SET(socket1, &readfds); 
        FD_SET(socket2, &readfds); 

        int max_fd = socket1 > socket2 ? socket1 : socket2;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            err_and_kill("select failed");
            break;
        }

        player_t* active_player;
        int active_socket;
        int opponent_socket;

        if (FD_ISSET(socket1, &readfds)) {
            active_player = &player1;
            active_socket = socket1;
            opponent_socket = socket2;
        } else if (FD_ISSET(socket2, &readfds)) {
            active_player = &player2;
            active_socket = socket2;
            opponent_socket = socket1;
        } else {
            continue;
        }

        ssize_t bytes_received = recv(active_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            running = false;
            continue;
        }
        
        buffer[bytes_received] = '\0';
        printf("Message from %s: %s\n", active_player->name, buffer);

        char** game_output = gamemaster(game, buffer, active_player);

        if (game_output[0] != NULL) {
            send_to_socket(active_socket, game_output[0]);
            free(game_output[0]);
        }
        if (game_output[1] != NULL) {
            send_to_socket(opponent_socket, game_output[1]);
            free(game_output[1]);
        }

        free(game_output);

        // print_lobby(lobby);
    }

    // Clean up the game
    free_game(game);
}

int send_to_socket(int socket, const char *str) {
    ssize_t send_bytes = send(socket, str, strlen(str), 0);
    if (send_bytes < 0) {
        perror("Failed to send to socket");
        return -1;
    }
    return 0;
}


// void handleTwoClients(player_t player1, player_t player2) {
//     fd_set readfds; // File descriptor set for select()
//     char buffer[BUFFER_SIZE];
//     char reply_buffer[BUFFER_SIZE]; // Add this line to create a buffer for the reply string
//     bool running = true;
//     int socket1 = player1.socket;
//     int socket2 = player2.socket;

//     game_t* game;

//     // char begin_message[250];
//     // snprintf(begin_message, sizeof(begin_message), "BEGN|%c|%s|", player1.role, player1.name);
//     // send_to_socket(player1.socket, begin_message);

//     while (running) {
//         FD_ZERO(&readfds); // Clear the file descriptor set
//         FD_SET(socket1, &readfds); // Add socket1 to the set
//         FD_SET(socket2, &readfds); // Add socket2 to the set

//         int max_fd = socket1 > socket2 ? socket1 : socket2;

//         printf("socket1: %d, socket2: %d\n", socket1, socket2);

//         // Wait for either socket to become ready for reading
//         int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

//         if (activity < 0) {
//             err_and_kill("select failed");
//             break;
//         }

//         // Check if socket1 is ready for reading
//         if (FD_ISSET(socket1, &readfds)) {
//             ssize_t bytes_received = recv(socket1, buffer, BUFFER_SIZE - 1, 0);
//             if (bytes_received > 0) {
//                 buffer[bytes_received] = '\0';
//                 printf("Message from socket1: [%s]\n", buffer);

//                 // char** output = gamemaster(game, buffer, NULL);

//                 sprintf(reply_buffer, "%s", buffer); // Use the reply_buffer here
//                 send_to_socket(socket2, reply_buffer); // Pass the reply_buffer instead of sprintf's return value
//         } else {
//                 running = false;
//             }
//         }

//         // Check if socket2 is ready for reading
//         if (FD_ISSET(socket2, &readfds)) {
//             ssize_t bytes_received = recv(socket2, buffer, BUFFER_SIZE - 1, 0);
//             if (bytes_received > 0) {
//                 buffer[bytes_received] = '\0';
//                 printf("Message from socket2: %s\n", buffer);
//                 sprintf(reply_buffer, "%s", buffer); // Use the reply_buffer here
//                 send_to_socket(socket1, reply_buffer); // Pass the reply_buffer instead of sprintf's return value
//         } else {
//                 running = false;
//             }
//         }
//     }
// }