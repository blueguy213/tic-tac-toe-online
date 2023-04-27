#ifndef _TTT_H_

    #define _TTT_H_

    void print_board(char* board);
    void handle_server_message(char *message, int sockfd);
    int connect_to_server(const char *ip, int port);
    void* listener(int sockfd);
    void *receive_messages(void *arg);
    void* game_loop(int sockfd);
    
#endif // _TTT_H_