#ifndef _TTTS_H_

    #include "../common/utils.h"

    #define THREAD_POOL_SIZE 10
    #define SERVER_BACKLOG 100
    #define SOCKET_ERROR (-1) 

    void* handle_connection(void* p_client_socket);
    int check(int exp, const char* msg);


#endif