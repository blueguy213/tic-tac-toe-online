#include "utils.h"

player_t* new_player(char role, char *name, SA_IN address, int socket, pthread_t tid) {
    player_t* player = malloc(sizeof(player_t));
    player->address = address;
    player->socket = socket;
    player->role = role;
    player->tid = tid;
    player->name = name;
    return player;
}

void free_player(player_t* player) {
    free(player->name);
    free(player);
}

int is_player(player_t* player, char *name, SA_IN address) {
    return strcmp(player->name, name) == 0 && player->address.sin_addr.s_addr == address.sin_addr.s_addr;
}