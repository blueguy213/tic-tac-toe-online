#include "ttt.h"
#include "../common/utils.h"

void print_board(char* board) {
    printf(" %c | %c | %c \n---+---+---\n %c | %c | %c \n---+---+---\n %c | %c | %c \n", board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8]);
}

void free_client_game(client_game_t* game) {
    if (game == NULL) return;
    free(game->board);
    pthread_mutex_destroy(&game->lock);
    free(game);
    game = NULL;
}

bool get_draw_response() {
    char draw_buf[10];
    fgets(draw_buf, 10, stdin);
    // Loop until user enters y or n
    while (draw_buf[0] != 'y' && draw_buf[0] != 'n') {
        printf("Invalid draw response. Try again: ");
        fgets(draw_buf, 10, stdin);
    }
    if (draw_buf[0] == 'y') {
        return true;
    } else {
        return false;
    }
}

int get_move(client_game_t* game) {


    regex_t move_regex;
    regcomp(&move_regex, "^[1-3],[1-3]", 0);

    char move_buf[10];
    pthread_mutex_lock(&game->lock);
    State old_state = game->state;
    pthread_mutex_unlock(&game->lock);
    if ((game->state == X && game->role == 'X') || (game->state == O && game->role == 'O')) {
        printf("Enter your move: ");
    } else {
        printf("Waiting for opponent's move...\n");
        printf("Enter 'resign' to resign or 'draw' to offer a draw: ");
    }
    fgets(move_buf, 10, stdin);
    pthread_mutex_lock(&game->lock);
    if (game->state != old_state) {
        pthread_mutex_unlock(&game->lock);
        regfree(&move_regex);
        return -3;
    }
    bool is_turn = (game->state == X && game->role == 'X') || (game->state == O && game->role == 'O');
    while (!(strncmp("resign", move_buf, 6) == 0 || strncmp("draw", move_buf, 4) == 0 || (is_turn && regexec(&move_regex, move_buf, 0, NULL, 0) == 0))) {

        move_buf[strlen(move_buf) - 1] = '\0'; // Remove trailing newline
        printf("%s is invalid input. Try again: ", move_buf, is_turn);
        
        fgets(move_buf, 10, stdin);

        if (((void*) game) == NULL) {
            pthread_mutex_unlock(&game->lock);
            regfree(&move_regex);
            return -3;
        } else if (game->state != old_state) {
        pthread_mutex_unlock(&game->lock);
        regfree(&move_regex);
        return -3;
    }
        is_turn = (game->state == X && game->role == 'X') || (game->state == O && game->role == 'O');
        pthread_mutex_unlock(&game->lock);
    }
    pthread_mutex_unlock(&game->lock);

    regfree(&move_regex);

    if (strncmp("resign", move_buf, 6) == 0) {
        
        return -1;
    } else if (strncmp("draw", move_buf, 4) == 0) {
        return -2;
    }

    // Convert to 2 ints
    int x = move_buf[0] - '0';
    int y = move_buf[2] - '0';

    return (x - 1) * 3 + y - 1;
}

int connect_to_server(const char *ip, int port) {

    int sockfd;
    SA_IN server_addr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        err_and_kill("Error: socket creation failed");
    }

    // Set up server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        err_and_kill("Error: invalid address or address not supported");
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        err_and_kill("Error: connection failed");
    }

    return sockfd;
}

void handle_server_message(char* msg, client_game_t* game) {
    char* code = strtok(msg, "|");
    int len = atoi(strtok(NULL, "|"));
    if (strcmp(code, "WAIT") == 0) {
        printf("Waiting for an opponent...\n");
        return;
    } else if (strcmp(code, "BEGN") == 0) {
        printf("Game started.\n");
        char* role = strtok(NULL, "|");
        char* opponent = strtok(NULL, "|");
        print_board(game->board);
        game->state = X;
        game->role = role[0];
        printf("You are %c and your opponent is %s.\n", game->role, opponent);

    } else if (strcmp(code, "MOVD") == 0) {
        char* role = strtok(NULL, "|");
        char* move = strtok(NULL, "|");
        char* board = strtok(NULL, "|");
        printf("Player %c moved.\n", *role);
        
        strcpy(game->board, board);
        print_board(board);
        if ((game->state == X && game->role == 'X') || (game->state == O && game->role == 'O')) {
            printf("Enter your move: ");
        } else if (game->state > 0) {
            printf("Waiting for opponent's move...\n");
            printf("Enter 'resign' to resign or 'draw' to offer a draw: ");
        }
        fflush(stdout);
        if (game->state == X) {
            game->state = O;
        } else {
            game->state = X;
        }
    } else if (strcmp(code, "INVL") == 0) {
        printf("INVALID move. Try again: ");
    } else if (strcmp(code, "DRAW") == 0) {
        // Prompt user asking them to accept or reject the draw
        char draw_buf[10];
        printf("Draw? [y/n]: ");
        
        if (game->role == 'X') {
            game->state = O_OFFERED_DRAW;
        } else {
            game->state = X_OFFERED_DRAW;
        }

    } else if (strcmp(code, "OVER") == 0) {
        char* outcome = strtok(NULL, "|");
        char* reason = strtok(NULL, "|");
        printf("\nGame over.");
        if (outcome[0] == 'W') {
            printf("\nYou won!");
            if (game->role == 'X') {
                game->state = X_WON;
            } else {
                game->state = O_WON;
            }
        } else if (outcome[0] == 'L') {
            printf("\nYou lost.");
            if (game->role == 'X') {
                game->state = O_WON;
            } else {
                game->state = X_WON;
            }
        } else if (outcome[0] == 'D') {
            printf("\nDraw.");
            game->state = DRAW;
        }
        printf(" Reason: %s\n", reason);
    } else {
        printf("Bad message from server: %s\n", msg);
    }
}

void* listener(void* game_void) {

    client_game_t* game = (client_game_t*)game_void;

    int n;
    char recvline[MAX_LINE_LEN];

    while (1) {

        n = read(game->sockfd, recvline, MAX_LINE_LEN - 1);
        if (n < 0) {
            free_client_game(game);
            err_and_kill("Failed to read from socket");
        } else if (n == 0) {
            break;
        } else {
            recvline[n] = '\0';
            pthread_mutex_lock(&game->lock);
            handle_server_message(recvline, game);
            pthread_mutex_unlock(&game->lock);
        }
        if (game->state == X_WON || game->state == O_WON || game->state == DRAW) {
            break;
        }
        sleep(1);
    }
    
    printf("Server closed connection\n");
    pthread_exit(NULL);
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char name[MAX_NAME_LEN];
    printf("Enter your name: ");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "|\n")] = '\0';

    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

    client_game_t *game = malloc(sizeof(client_game_t));
    game->sockfd = sockfd;
    game->board = malloc(sizeof(char) * 10);
    strcpy(game->board, ".........");
    game->state = UNSTARTED;

    bool running = true;

    char message[MAX_LINE_LEN];
    snprintf(message, MAX_LINE_LEN, "PLAY|%zd|%s|", strlen(name) + 1, name);
    write(sockfd, message, strlen(message));

    pthread_mutex_init(&game->lock, NULL);

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, listener, (void *)game);

    while (running) {
        pthread_mutex_lock(&game->lock);
        if ((game->state == X && game->role == 'X') || (game->state == O && game->role == 'O')) {
            pthread_mutex_unlock(&game->lock);
            
            printf("Your turn!\n");
            fflush(stdout);
            
            int move_index = get_move(game);
            printf("Move index: %d\n", move_index);
            if (move_index == -1) {
                pthread_mutex_lock(&game->lock);
                if (game->role == 'X') {
                    game->state = O_WON;
                } else {
                    game->state = X_WON;
                }
                pthread_mutex_unlock(&game->lock);
                snprintf(message, MAX_LINE_LEN, "RSGN|0|");
                write(sockfd, message, strlen(message));
            } else if (move_index == -2) {
                pthread_mutex_lock(&game->lock);
                if (game->role == 'X') {
                    game->state = X_OFFERED_DRAW;
                } else {
                    game->state = O_OFFERED_DRAW;
                }
                snprintf(message, MAX_LINE_LEN, "DRAW|2|S|");
                write(sockfd, message, strlen(message));
                pthread_mutex_unlock(&game->lock);
            } else if (move_index == -3) {

            } else {

                printf("Sending move...\n");

                int x = (move_index / 3) + 1;
                int y = (move_index % 3) + 1;

                snprintf(message, MAX_LINE_LEN, "MOVE|%d|%c|%d,%d|", 6, game->role, x, y);
                write(sockfd, message, strlen(message));
            }
        } else if ((game->state == X && game->role == 'O') || (game->state == O && game->role == 'X')) {
            pthread_mutex_unlock(&game->lock);

            int move_index = get_move(game);

            printf("Waiting for opponent to move...\n");
            fflush(stdout);
        } else if (game->state == O_OFFERED_DRAW || game->state == X_OFFERED_DRAW) {
            pthread_mutex_unlock(&game->lock);
            
            bool draw_accepted = get_draw_response();
            if (draw_accepted) {
                snprintf(message, MAX_LINE_LEN, "DRAW|2|A|");
                
            } else {
                snprintf(message, MAX_LINE_LEN, "DRAW|2|R|");
                write(sockfd, message, strlen(message));
            }
        } else if (game->state == X_WON || game->state == O_WON || game->state == DRAW) {
            pthread_mutex_unlock(&game->lock);
            running = false;
        } else {
            pthread_mutex_unlock(&game->lock);
        }
    }
    printf("Game over.\n");

    if (pthread_join(listener_thread, NULL) < 0) {
        if (game != NULL) {
            free_client_game(game);
        }
        err_and_kill("Failed to join listener thread");
    }
    exit(0);
}