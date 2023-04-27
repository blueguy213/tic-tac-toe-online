#include "utils.h"

player_t* new_player(char role, char *name, SA_IN address, int socket, pthread_t tid) {
    player_t* player = malloc(sizeof(player_t));
    player->address = address;
    player->socket = socket;
    player->role = role;
    player->tid = tid;
    player->name = malloc(strlen(name) + 1);
    strcpy(player->name, name);
    return player;
}

void free_player(player_t* player) {
    if (player == NULL) return;
    free(player->name);
    free(player);
}

int is_player(player_t* player, char *name, SA_IN address) {
    return strcmp(player->name, name) == 0 && player->address.sin_addr.s_addr == address.sin_addr.s_addr;
}

void print_player(player_t player) {
    printf("Player Info:\n");
    printf("Role: %c\n", player.role);
    printf("Name: %s\n", player.name);
    printf("Address: %s:%d\n", inet_ntoa(player.address.sin_addr), ntohs(player.address.sin_port));
    printf("Socket: %d\n", player.socket);
    printf("Thread ID: %lu\n", (unsigned long)player.tid);
    if (player.game != NULL) {
        printf("Game: (pointer to the game_t struct)\n");
    } else {
        printf("Game: (not currently in a game)\n");
    }
}