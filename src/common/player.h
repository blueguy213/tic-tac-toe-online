#ifndef _PLAYER_H_
    
    #define _PLAYER_H_

    #include "utils.h"
    #include "game.h"

    typedef struct sockaddr SA;
    typedef struct sockaddr_in SA_IN;
    
    typedef struct {
        char role; // 'X' or 'O'
        char* name; // The player's name (up to 255 characters)
        SA_IN* address; // The player's client address (IP and port)
        int socket; // The player's socket file descriptor on the server
        struct game_t *game; // The game the player is in (mutable state)
    } player_t;

    player_t* new_player(char role, char *name, SA_IN address, int socket);
    void free_player(player_t* player);
    int is_player(player_t* player, char *name, SA_IN* address);
    void print_player(player_t* player);

#endif // _PLAYER_H_