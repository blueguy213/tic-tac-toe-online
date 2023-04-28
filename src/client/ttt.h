#ifndef _TTT_H_

    #define _TTT_H_

    #include "../common/utils.h"

    typedef struct {
        char* board; // The game board as a 9 character string (3 rows of 3 characters)
        int sockfd;
        State state;
        pthread_mutex_t lock; // Mutex lock for the game state since it is shared between the listening and responding threads        State state;
        char role;
    } client_game_t;

    void print_board(char* board);
    void handle_server_message(char *msg, client_game_t* game);
    int connect_to_server(const char *ip, int port);
    void* listener(void* game);
    void *receive_messages(void *arg);
    void* game_loop(void* args);
    void free_client_game(client_game_t* game);
    
#endif // _TTT_H_