#include "utils.h"

Player* new_player(char role, char *name, SA_IN address, int socket, pid_t pid) {
    Player* player = malloc(sizeof(Player));
    player->address = address;
    player->socket = socket;
    player->role = role;
    player->pid = pid;
    player->name = name;
    return player;
}

void free_player(Player* player) {
    free(player->name);
    free(player);
}

int is_player(Player* player, char *name, SA_IN address) {
    return strcmp(player->name, name) == 0 && player->address.sin_addr.s_addr == address.sin_addr.s_addr;
}