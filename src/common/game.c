#include "utils.h"

Game* new_game(Player* x, Player* o) {
    Game* game = malloc(sizeof(Game));
    game->board = malloc(9 * sizeof(char));
    for (int i = 0; i < 9; i++) {
        game->board[i] = '.';
    }
    game->playerX = x;
    game->playerO = o;
    return game;
}

void free_game(Game* game) {
    free_player(game->playerO);
    free_player(game->playerX);
    free(game->board);
    free(game);
}

void draw(Game* game, Player* player) {
    if (game->turn > 0) {
        if (player->role == 'X') {
            game->turn = -3;
        } else {
            game->turn = -4;
        }
    } else {
        if (player->role == 'X') {
            game->turn = 0;
        } else {
            game->turn = 0;
        }
    }
}

void move(Game* game, Player* player, int x, int y) {
    if (game->turn > 0) {
        if (player->role == 'X') {
            game->board[x + y * 3] = 'X';
            game->turn = 2;
        } else {
            game->board[x + y * 3] = 'O';
            game->turn = 1;
        }
    } else {
        if (player->role == 'X') {
            game->board[x + y * 3] = 'X';
            game->turn = 2;
        } else {
            game->board[x + y * 3] = 'O';
            game->turn = 1;
        }
    }
}

void resign(Game* game, Player* player) {
    if (player->role == 'X') {
        game->turn = -2;
    } else {
        game->turn = -1;
    }
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
void print_board_as_display(Game* game) {
    printf(" %c | %c | %c \n---+---+---\n %c | %c | %c \n---+---+---\n %c | %c | %c \n", game->board[0], game->board[1], game->board[2], game->board[3], game->board[4], game->board[5], game->board[6], game->board[7], game->board[8]);
}

