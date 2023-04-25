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
    #include <sys/ioctl.h>
    #include <netdb.h>

    #define MAXLINE 4096
    #define SA struct sockaddr

    void err_and_kill(const char *fmt, ...);
    char* bin2hex(const unsigned char *input, size_t len);

#endif // _UTILS_H_

// Path: src/common/utils.c