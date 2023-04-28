#ifndef _UTILS_H_

    #define _UTILS_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <signal.h>
    #include <arpa/inet.h>
    #include <stdarg.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <sys/wait.h>
    #include <sys/ioctl.h>
    #include <netdb.h>
    #include <pthread.h>
    #include <stdbool.h>
    #include <regex.h>

    #include "game.h"
    #include "player.h"

    #define MAX_LINE_LEN 4096
    #define MAX_NAME_LEN 255
    #define MAX_GAME_COUNT 5
    #define MAX_PLAYER_COUNT 10

    void err_and_kill(const char *fmt, ...);
    char* bin2hex(const unsigned char *input, size_t len);
    void print_state(State state);

    typedef struct sockaddr SA;
    typedef struct sockaddr_in SA_IN;
    
#endif // _UTILS_H_

// Path: src/common/utils.c