#include "ttt.h"
#include "../common/utils.h"

void print_board(char* board) {
    printf(" %c | %c | %c \n---+---+---\n %c | %c | %c \n---+---+---\n %c | %c | %c \n", board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8]);
}

void handle_server_message(char *message, int sockfd, char* board) {
    char *token = strtok(message, "|");
    while (token != NULL) {
        if (strcmp(token, "WAIT") == 0) {
            printf("Waiting for an opponent...\n");
            return;
        } else if (strcmp(token, "BEGN") == 0) {
            char *role = strtok(NULL, "|");
            char *opponent_name = strtok(NULL, "|");
            printf("Game started! You are %s, and your opponent is %s.\n", role, opponent_name);
            if (strcmp(role, "X") == 0) {
                printf("Your turn!\n");
            } else {
                printf("Waiting for your opponent to make a move...\n");
            }
            print_board(board);
            pthread_t game_thread;
            pthread_create(&game_thread, NULL, game_loop, (void *)&sockfd);
            pthread_detach(game_thread);
            return;
        } else if (strcmp(token, "MOVD") == 0) {
            char *role = strtok(NULL, "|");
            char *grid = strtok(NULL, "|");
            printf("Player %s made a move.\n", role);
            printf("Current board:\n");
            print_board(grid);
            printf("Your turn!\n");
            return;
        } else if (strcmp(token, "INVL") == 0) {
            char *reason = strtok(NULL, "|");
            printf("Invalid move or action: %s\n", reason);
            return;
        } else if (strcmp(token, "DRAW") == 0) {
            char *message = strtok(NULL, "|");
            if (strcmp(message, "S") == 0) {
                printf("Your opponent has suggested a draw. Do you accept? (y/n)\n");
            } else if (strcmp(message, "R") == 0) {
                printf("Your opponent has rejected the draw.\n");
            }
            return;
        } else if (strcmp(token, "OVER") == 0) {
            char *outcome = strtok(NULL, "|");
            char *reason = strtok(NULL, "|");
            printf("Game over. You %s. Reason: %s\n", outcome, reason);
            exit(0);
        }
        token = strtok(NULL, "|");
    }
}

int connect_to_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

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

void* listener(int sockfd, char* board) {
    int n;
    char recvline[MAX_LINE_LEN];

    while (1) {
        n = read(sockfd, recvline, MAX_LINE_LEN - 1);

        if (n < 0) {
            err_and_kill("Failed to read from socket");
        }

        recvline[n] = '\0';
        handle_server_message(recvline, sockfd, board);

        sleep(1);
    }

    printf("Server closed connection\n");
    exit(0);
}

void* game_loop(void* args) {

    int sockfd = *((int *) args);

    char message[MAX_LINE_LEN];
    regex_t move_regex;
    regcomp(&move_regex, "^[1-3],[1-3]$", 0);

    while (1) {
        char input[10];
        printf("Enter your move (row,column) or type 'resign' or 'draw': ");
        fgets(input, 10, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "resign") == 0) {
            snprintf(message, MAX_LINE_LEN, "RSGN|0|");
        } else if (strcmp(input, "draw") == 0) {
            snprintf(message, MAX_LINE_LEN, "DRAW|1|S|");
        } else if (regexec(&move_regex, input, 0, NULL, 0) == 0) {
            snprintf(message, MAX_LINE_LEN, "MOVE|5|%s|", input);
        } else {
            printf("Invalid input. Please try again.\n");
        }
        write(sockfd, message, strlen(message));
    }

    printf("Server closed connection\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        err_and_kill("Usage: ttt <IP address> <port number>");
    }

    char name[MAX_NAME_LEN];
    printf("Enter your name: ");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = '\0';

    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

    char message[MAX_LINE_LEN];
    snprintf(message, MAX_LINE_LEN, "PLAY|%zd|%s|", strlen(name), name);
    write(sockfd, message, strlen(message));

    char board[10] = ".........";

    listener(sockfd, board);

    return 0;
}

// int main(int argc, char** argv) {
//     if (argc != 3) {
//         printf("Usage: %s <domain> <port>\n", argv[0]);
//         fflush(stdout);
//         return 1;
//     }

//     int sockfd, send_bytes;
//     SA_IN server_addr;
//     char sendline[MAX_LINE_LEN];

//     // Get the name of the user
//     char name[MAX_NAME_LEN+1];
//     printf("Enter your name: ");
//     fflush(stdout);
//     fgets(name, MAX_NAME_LEN, stdin);
//     name[MAX_NAME_LEN] = '\0';
//     sprintf(sendline, "PLAY|%s|", name);

//     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         err_and_kill("Failed to create socket");
//     }

//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(atoi(argv[2]));

//     if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
//         err_and_kill("Failed to convert IP address");
//     }

//     if (connect(sockfd, (SA*)&server_addr, sizeof(server_addr)) < 0) {
//         err_and_kill("Failed to connect to server");
//     }

//     pthread_t listener_thread;
//     pthread_create(&listener_thread, NULL, listener, (void *)&sockfd);

//     // Send the PLAY command to the server
//     if (write(sockfd, sendline, strlen(sendline)) != strlen(sendline)) {
//         err_and_kill("Failed to write to socket");
//     }

    
//     while (1) {
//         printf("Enter message: ");
//         fflush(stdout);
//         fgets(sendline, MAX_LINE_LEN, stdin);
//         send_bytes = strlen(sendline);

//         if (write(sockfd, sendline, send_bytes) != send_bytes) {
//             err_and_kill("Failed to write to socket");
//         }
//     }

//     return 0;
// }
