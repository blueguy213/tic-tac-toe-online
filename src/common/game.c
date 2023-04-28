#include "utils.h"

game_t* new_game(player_t* x, player_t* o) {
    x->role = 'X';
    o->role = 'O';
    game_t* game = malloc(sizeof(game_t));
    game->board = malloc(10 * sizeof(char));
    strcpy(game->board, ".........");
    game->playerX = x;
    game->playerO = o;
    game->state = UNSTARTED;
    return game;
}

void free_game(game_t* game) {
    if (game == NULL) return;
    free(game->board);
    free(game);
}

void draw(game_t* game, player_t* player) {
    if (game->state > 0) {
        if (player->role == 'X') {
            game->state = X_OFFERED_DRAW;
        } else {
            game->state = O_OFFERED_DRAW;
        }
    } else {
        if (player->role == 'X') {
            game->state = DRAW;
        } else {
            game->state = DRAW;
        }
    }
}

void move(game_t* game, player_t* player, int x, int y) {
    if (game->state > 0) {
        if (player->role == 'X') {
            game->board[x + y * 3] = 'X';
        } else {
            game->board[x + y * 3] = 'O';
        }
    } else {
        if (player->role == 'X') {
            game->board[x + y * 3] = 'X';
        } else {
            game->board[x + y * 3] = 'O';
        }
    }
}

void resign(game_t* game, player_t* player) {
    if (player->role == 'X') {
        game->state = O_WON;
    } else {
        game->state = X_WON;
    }
}

/**
 * Checks if the game is over and updates the game state accordingly.
 * Returns: 0 if game is still in progress, -1 if X wins, and -2 if O wins, -3 if it's a draw.
*/
int check_game(game_t* game) {
    // Check for horizontal wins
    for (int i = 0; i < 3; i++) {
        if (game->board[i * 3] == game->board[i * 3 + 1] && game->board[i * 3 + 1] == game->board[i * 3 + 2]) {
            if (game->board[i * 3] == 'X') {
                return -1;
            } else if (game->board[i * 3] == 'O') {
                return -2;
            }
        }
    }
    // Check for vertical wins
    for (int i = 0; i < 3; i++) {
        if (game->board[i] == game->board[i + 3] && game->board[i + 3] == game->board[i + 6]) {
            if (game->board[i] == 'X') {
                return -1;
            } else if (game->board[i] == 'O') {
                return -2;
            }
        }
    }
    // Check for diagonal wins
    if ((game->board[0] == game->board[4] && game->board[4] == game->board[8]) || (game->board[2] == game->board[4] && game->board[4] == game->board[6])) {
        if (game->board[4] == 'X') {
            return -1;
        } else if (game->board[4] == 'O') {
            return -2;
        }
    }

    // Check for draw by checking if there are any empty spaces left
    for (int i = 0; i < 9; i++) {
        if (game->board[i] == '.') {
            return 0;
        }
    }
    return -3;
}

/**
 *  O | X | O 
 * ---+---+---
 *  . | X | O 
 * ---+---+---
 *  X | O | X 
 * 
 * Example board display
*/
void print_board_as_display(game_t* game) {
    printf(" %c | %c | %c \n---+---+---\n %c | %c | %c \n---+---+---\n %c | %c | %c \n", game->board[0], game->board[1], game->board[2], game->board[3], game->board[4], game->board[5], game->board[6], game->board[7], game->board[8]);
}

void gamemaster(game_t* game, char* input, player_t* sender, char* x_out, char* o_out) {
    // char** output = malloc(2 * sizeof(char*));

    print_player(sender);

    // Parse input
    char code[5];
    sscanf(input, "%4[^|]", code);

    if (strcmp(code, "PLAY") == 0) {
        char name[256];
        sscanf(input, "PLAY|%*d|%[^|]", name);
        if (game->playerX == NULL) {

            strcpy(x_out, "WAIT|0|");
            strcpy(o_out, "");
        }
    } else if (strcmp(code, "MOVE") == 0) {

        int x, y;
        char role;
        sscanf(input, "MOVE|%*d|%c|%d,%d|", &role, &x, &y);
        // Check if it's the sender's turn
        if (role == 'X' && game->state != X) {
            strcpy(x_out, "INVL|23|Not your turn|");
            strcpy(o_out, "");
        } else if (role == 'O' && game->state != O) {
            strcpy(x_out, "");
            strcpy(o_out, "INVL|23|Not your turn|");
        } else if (game->board[(x - 1) + (y - 1) * 3] == '.') {
            move(game, sender, x - 1, y - 1);
            int game_result = check_game(game);
            printf("Game result: %d\n", game_result);
            if (game_result < 0) {
                if (game_result == -1) {
                    game->state = X_WON;
                    strcpy(x_out, "OVER|8|W|X won|");
                    strcpy(o_out, "OVER|8|L|X won|");
                } else if (game_result == -2) {
                    game->state = O_WON;
                    strcpy(x_out, "OVER|8|L|O won|");
                    strcpy(o_out, "OVER|8|W|O won|");
                } else {
                    game->state = DRAW;
                    strcpy(x_out, "OVER|8|D|Draw|");
                    strcpy(o_out, "OVER|8|D|Draw|");
                }
            } else {
                if (game->state == X || game->state == UNSTARTED) {
                    game->state = O;
                } else {
                    game->state = X;
                }
                char buffer[32];
                sprintf(buffer, "MOVD|16|%c|%d,%d|%s|", sender->role, x, y, game->board);
                strcpy(x_out, buffer);
                strcpy(o_out, buffer);
            }
        } else {
            strcpy(x_out, "INVL|24|That space is occupied|");
            strcpy(o_out, "");
        }
    } else if (strcmp(code, "RSGN") == 0) {
        resign(game, sender);
        if (sender->role == 'X') {
            strcpy(x_out, "OVER|5|L|X resigned|");
            strcpy(o_out, "OVER|5|W|X resigned|");
        } else {
            strcpy(x_out, "OVER|5|W|O resigned|");
            strcpy(o_out, "OVER|5|L|O resigned|");
        }
    } else if (strcmp(code, "DRAW") == 0) {
        char message;
        sscanf(input, "DRAW|%*d|%c|", &message);
        if (message == 'S') {
            draw(game, sender);
            strcpy(x_out, "DRAW|2|S|");
            strcpy(o_out, "DRAW|2|S|");
        } else if (message == 'A') {
            if ((game->state != X_OFFERED_DRAW && sender->role == 'X') || (game->state != O_OFFERED_DRAW && sender->role == 'O')) {
                strcpy(x_out, "INVL|23|Not your turn|");
                strcpy(o_out, "");
            } else {
                draw(game, sender);
                strcpy(x_out, "DRAW|2|A|");
                strcpy(o_out, "DRAW|2|A|");
            }
            draw(game, sender);
        } else if (message == 'R') {
            if ((game->state != X_OFFERED_DRAW && sender->role == 'X') || (game->state != O_OFFERED_DRAW && sender->role == 'O')) {
                strcpy(x_out, "INVL|23|Not your turn|");
                strcpy(o_out, "");
            } else {
                if (sender->role == 'X') {
                    game->state = X;
                } else {
                    game->state = O;
                }
                strcpy(x_out, "DRAW|2|R|");
                strcpy(o_out, "DRAW|2|R|");
            }
        }
    } else {
        strcpy(x_out, "INVL|23|!Unrecognized command.|");
        strcpy(o_out, "");
    }
}

void print_game_info(game_t* game) {
    printf("Game info:\n");
    printf("\tPlayer X: %s", game->playerX == NULL ? "NULL" : game->playerX->name);
    printf("\tPlayer O: %s", game->playerO == NULL ? "NULL" : game->playerO->name);
    printf("\tState: %d\n", game->state);
    printf("\tBoard: %s\n", game->board);
}