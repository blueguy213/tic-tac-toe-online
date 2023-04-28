#include "utils.h"

// Print error message and exit
void err_and_kill(const char *fmt, ...) {

    // Save errno before calling any other function
    int errnum = errno;
    va_list args;

    // Print error message
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);

    // Print errno message if any
    if (errnum != 0) {
        fprintf(stderr, "(errno = %d): %s\n\n", errnum, strerror(errnum));
        fflush(stderr);
    }

    va_end(args);
    exit(EXIT_FAILURE);
}

// Convert binary to hex
char* bin2hex(const unsigned char *bin, size_t len) {

    char *hexits = "0123456789ABCDEF";
    char *hex;

    if (bin == NULL || len <= 0) {
        return NULL;
    }

    int hex_len = (len * 3) + 1;

    hex = malloc(hex_len);
    memset(hex, 0, hex_len);

    for (int i = 0; i < len; i++) {
        hex[i * 3] = hexits[bin[i] >> 4];
        hex[(i * 3) + 1] = hexits[bin[i] & 0x0F];
        hex[(i * 3) + 2] = ' ';
    }

    return hex;
}

void print_state(State state) {
    if (state == X) {
        printf("X\n");
    } else if (state == O) {
        printf("O\n");
    } else if (state == DRAW) {
        printf("DRAW\n");
    } else if (state == X_OFFERED_DRAW) {
        printf("X_OFFERED_DRAW\n");
    } else if (state == O_OFFERED_DRAW) {
        printf("O_OFFERED_DRAW\n");
    } else if (state == X_WON) {
        printf("X_WON\n");
    } else if (state == O_WON) {
        printf("O_WON\n");
    } else if (state == UNSTARTED) {
        printf("UNSTARTED\n");
    }
}