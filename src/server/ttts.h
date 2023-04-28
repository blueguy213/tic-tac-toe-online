#ifndef _TTTS_H_

    #define _TTTS_H_

    #include "../common/utils.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <errno.h>
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <stdbool.h>

    #include "lobby.h"

    #define BUFFER_SIZE 1024

    #define THREAD_POOL_SIZE 10
    #define SERVER_BACKLOG 100
    #define SOCKET_ERROR (-1)

    typedef struct game_thread_args {
        lobby_t *lobby;
        int game_index;
    } game_thread_args_t;

    int check(int exp, const char* msg);
    int send_to_socket(int socket, const char *str);
    void handleGame(game_t* game);
    void *client_handler(void* args);

#endif // _TTTS_H_
