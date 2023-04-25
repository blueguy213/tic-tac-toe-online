#ifndef _GAME_H_

    #define _GAME_H_

    #include "utils.h"
    #include "player.h"

    typedef struct {
        char board[3][3];
        int turn;
        pid_t pid;
        pthread_mutex_t lock;
    } Game;

    Game* new_game();
    int free_game(Game* game);
    int draw(Game* game, Player* player);
    int move(Game* game, Player* player, int x, int y);
    int resign(Game* game, Player* player);
    char* board_as_message(Game* game);
    char* board_as_display(Game* game);

#endif // _GAME_H_