#include "utils.h"

game_t* new_game(player_t* x, player_t* o) {
    game_t* game = malloc(sizeof(game_t));
    game->board = malloc(9 * sizeof(char));
    for (int i = 0; i < 9; i++) {
        game->board[i] = '.';
    }
    game->playerX = x;
    game->playerO = o;
    return game;
}

void free_game(game_t* game) {
    free_player(game->playerO);
    free_player(game->playerX);
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
            game->state = O;
        } else {
            game->board[x + y * 3] = 'O';
            game->state = X;
        }
    } else {
        if (player->role == 'X') {
            game->board[x + y * 3] = 'X';
            game->state = O;
        } else {
            game->board[x + y * 3] = 'O';
            game->state = X;
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

char** gamemaster(Game* game, char* input, Player* sender) {
    char** output = malloc(2 * sizeof(char*));

    // Parse input
    char code[5];
    sscanf(input, "%4[^|]", code);

    if (strcmp(code, "PLAY") == 0) {
        char name[256];
        sscanf(input, "PLAY|%*d|%[^|]", name);
        if (game->playerX == NULL) {

            output[0] = "WAIT|0|";
            output[1] = NULL;
        } else if (game->playerO == NULL) {

            output[0] = "WAIT|0|";
            char buffer[256 + 8]; // Make sure the buffer is large enough to hold the name and the additional characters.
            // sprintf(buffer, "BEGN|0|X|%s", game->playerX->name);
            output[1] = strdup(buffer);
        }
    } else if (strcmp(code, "MOVE") == 0) {
        if (sender == NULL) {
            output[0] = "INVL|23|!Unrecognized sender.|";
            output[1] = NULL;
            return output;
        }

        int x, y;
        sscanf(input, "MOVE|%*d|%*c|%d,%d", &x, &y);

        if (game->board[(x - 1) + (y - 1) * 3] == '.') {
            move(game, sender, x - 1, y - 1);
            int game_result = check_game(game);
            if (game_result < 0) {
                if (game_result == -1) {
                    output[0] = "OVER|5|W|X won!";
                    output[1] = "OVER|5|L|X won!";
                } else if (game_result == -2) {
                    output[0] = "OVER|5|L|O won!";
                    output[1] = "OVER|5|W|O won!";
                } else {
                    output[0] = "OVER|5|D|Draw!";
                    output[1] = "OVER|5|D|Draw!";
                }
            } else {
                char buffer[32];
                sprintf(buffer, "MOVD|16|%c|%d,%d|%.9s|", sender->role, x, y, game->board);
                output[0] = strdup(buffer);
                output[1] = strdup(buffer);
            }
        } else {
            output[0] = "INVL|24|That space is occupied.|";
            output[1] = NULL;
        }
    } else if (strcmp(code, "RSGN") == 0) {
        if (sender == NULL) {
            output[0] = "INVL|23|!Unrecognized sender.|";
            output[1] = NULL;
            return output;
        }

        resign(game, sender);
        if (sender->role == 'X') {
            output[0] = "OVER|5|L|X resigned!";
            output[1] = "OVER|5|W|X resigned!";
        } else {
            output[0] = "OVER|5|W|O resigned!";
            output[1] = "OVER|5|L|O resigned!";
        }
   } else if (strcmp(code, "DRAW") == 0) {
    if (sender == NULL) {
        output[0] = "INVL|23|!Unrecognized sender.|";
        output[1] = NULL;
        return output;
    }

    char message;
    sscanf(input, "DRAW|%*d|%c", &message);
    if (message == 'S') {
        draw(game, sender);
        output[0] = "DRAW|2|S|";
        output[1] = "DRAW|2|S|";
    } else if (message == 'A') {
        output[0] = "OVER|5|D|Draw accepted!";
        output[1] = "OVER|5|D|Draw accepted!";
    } else if (message == 'R') {
        output[0] = "DRAW|2|R|";
        output[1] = "DRAW|2|R|";
    }
} else {
    output[0] = "INVL|23|!Unrecognized command.|";
    output[1] = NULL;
}
return output;
}
