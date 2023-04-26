#include <pthread.h>
#include "../common/utils.h"

void *receive_messages(void *arg) {
    int sockfd = *((int *)arg);
    int n;
    char recvline[MAX_LINE_LEN];

    while ((n = read(sockfd, recvline, MAX_LINE_LEN - 1)) > 0) {
        recvline[n] = '\0';
        printf("\nServer reply: %s\nEnter message: ", recvline);
    }

    if (n < 0) {
        err_and_kill("Failed to read from socket");
    }

    printf("Server closed connection\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <domain> <port>\n", argv[0]);
        return 1;
    }

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

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, &sockfd) != 0) {
        err_and_kill("Failed to create receive thread");
    }

    while (1) {
        printf("Enter message: ");
        fgets(sendline, MAX_LINE_LEN, stdin);
        send_bytes = strlen(sendline);

        if (write(sockfd, sendline, send_bytes) != send_bytes) {
            err_and_kill("Failed to write to socket");
        }
    }

    return 0;
}
