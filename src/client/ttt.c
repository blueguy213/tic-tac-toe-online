#include <pthread.h>
#include "../common/utils.h"

void* listener(void *arg) {
    
    int sockfd = *((int *)arg);
    int n;
    char recvline[MAX_LINE_LEN];


    while (1) {

        n = read(sockfd, recvline, MAX_LINE_LEN - 1);

        if (n < 0) {
            err_and_kill("Failed to read from socket");
        }

        sleep(1);
    }

    printf("Server closed connection\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <domain> <port>\n", argv[0]);
        fflush(stdout);
        return 1;
    }

    // Get the name of the user
    char name[MAX_NAME_LEN+1];
    printf("Enter your name: ");
    fflush(stdout);
    fgets(name, MAX_NAME_LEN, stdin);
    name[MAX_NAME_LEN] = '\0';

    int sockfd, send_bytes;
    SA_IN server_addr;
    char sendline[MAX_LINE_LEN];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_and_kill("Failed to create socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        err_and_kill("Failed to convert IP address");
    }

    if (connect(sockfd, (SA*)&server_addr, sizeof(server_addr)) < 0) {
        err_and_kill("Failed to connect to server");
    }

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, listener, (void *)&sockfd);
    
    while (1) {
        printf("Enter message: ");
        fflush(stdout);
        fgets(sendline, MAX_LINE_LEN, stdin);
        send_bytes = strlen(sendline);

        if (write(sockfd, sendline, send_bytes) != send_bytes) {
            err_and_kill("Failed to write to socket");
        }
    }

    return 0;
}
