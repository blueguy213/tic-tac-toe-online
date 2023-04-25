#ifndef _PLAYER_H_
    
    #define _PLAYER_H_

    #include "utils.h"
    #include "game.h"
    
    typedef struct {
        char role;
        char name[255];
        SA address;
        int socket;
        Game game;
    } Player;

    Player* new_player(char role, char *name, SA address, int socket);
    void free_player(Player* player);
    int is_player(Player* player, char *name, SA address);

#endif // _PLAYER_H_