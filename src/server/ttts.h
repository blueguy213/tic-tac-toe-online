#ifndef _TTTS_H_

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

    #define BUFFER_SIZE 1024

    #define THREAD_POOL_SIZE 10
    #define SERVER_BACKLOG 100
    #define SOCKET_ERROR (-1) 

    void* handle_connection(void* p_client_socket);
    int check(int exp, const char* msg);

#endif