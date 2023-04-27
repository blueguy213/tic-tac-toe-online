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

    int port, server_socket, client_socket, addr_size, game_index;
    int  client_count = 0;

    lobby_t* lobby = new_lobby(10);

    port = atoi(argv[1]);
    server_socket = check(socket(AF_INET, SOCK_STREAM, 0), "Failed to create socket");

    SA_IN server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    // player_t* previous_player;
    
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
        newplayer = new_player('X', player_name, client_addr, client_socket); // 'X' will be replaced with the proper role later
        print_player(*newplayer);
        // Send the WAIT response to the client
        char wait_response[] = "WAIT";
        write(client_socket, wait_response, strlen(wait_response));

        client_count = client_count + 1;

        if ((game_index = add_player(lobby, newplayer)) >= 0) {
            game_thread_args_t *args = malloc(sizeof(game_thread_args_t));
            args->lobby = lobby;
            args->game_index = game_index;
            pthread_create(&tid, NULL, client_handler, (void *)args);
            // pthread_create(&tid[client_count / 2 - 1], NULL, client_handler, lobby->games[game_index]);
        }

        print_lobby(lobby);
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

void handleGame(game_t* game);

void *client_handler(void* args) {

    game_thread_args_t *game_args = (game_thread_args_t *) args;

    lobby_t *lobby = game_args->lobby;
    int game_index = game_args->game_index;

    game_t *game = lobby->games[game_index];
    
    // handle clients
    printf("Handling two clients\n");
    // call handleTwoClients() function here
    printf("Two clients found, making a tictac toe game\n");

    handleGame(game);

    // close sockets
    close(game->playerX->socket);
    close(game->playerO->socket);

    pthread_mutex_lock(&lobby->lock);
    
    free_game(lobby->games[game_index]);
    // Remove the game from the lobby
    remove_game(lobby, game);

    // Remove the players from the lobby
    remove_player(lobby, game->playerX->name, game->playerX->address);
    remove_player(lobby, game->playerO->name, game->playerO->address);
    
    pthread_mutex_unlock(&lobby->lock);

    // free memory
    free_game(game);

    pthread_exit(NULL);
}

void handleGame(game_t* game) {

    fd_set readfds;
    char buffer[BUFFER_SIZE];
    char reply_buffer[BUFFER_SIZE];

    bool running = true;
    int socket1 = game->playerX->socket;
    int socket2 = game->playerO->socket;

    // Send the BEGN response to both players
    sprintf(reply_buffer, "BEGN|%c|%s|", game->playerX->role, game->playerO->name);
    send_to_socket(socket1, reply_buffer);
    sprintf(reply_buffer, "BEGN|%c|%s|", game->playerO->role, game->playerX->name);
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
            active_player = game->playerX;
            active_socket = socket1;
            opponent_socket = socket2;
        } else if (FD_ISSET(socket2, &readfds)) {
            active_player = game->playerO;
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
            //free(game_output[0]);
        }
        if (game_output[1] != NULL) {
            send_to_socket(opponent_socket, game_output[1]);
            //free(game_output[1]);
        }

        free(game_output);
    }
}

int send_to_socket(int socket, const char *str) {
    ssize_t send_bytes = send(socket, str, strlen(str), 0);
    if (send_bytes < 0) {
        perror("Failed to send to socket");
        return -1;
    }
    return 0;
}