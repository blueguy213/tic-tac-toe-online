#ifndef _PLAYER_H_
    
    #define _PLAYER_H_

    #include "utils.h"
    #include "game.h"

    typedef struct sockaddr SA;
    typedef struct sockaddr_in SA_IN;
    
    typedef struct {
        char role; // 'X' or 'O'
        char* name; // The player's name (up to 255 characters)
        SA_IN address; // The player's client address (IP and port)
        int socket; // The player's socket file descriptor on the server
        pthread_t tid; // The process ID of the server thread handling the player
        struct Game *game; // The game the player is in (mutable state)
    } Player;

    Player* new_player(char role, char *name, SA_IN address, int socket, pthread_t tid);
    void free_player(Player* player);
    int is_player(Player* player, char *name, SA_IN address);

#endif // _PLAYER_H_