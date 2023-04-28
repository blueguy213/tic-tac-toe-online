#include "utils.h"

player_t* new_player(char role, char *name, SA_IN address, int socket) {
    player_t* player = malloc(sizeof(player_t));
    player->address = malloc(sizeof(SA_IN));
    memcpy(player->address, &address, sizeof(SA_IN));
    player->socket = socket;
    player->role = role;
    player->name = malloc(strlen(name) + 1);
    strcpy(player->name, name);
    return player;
}

void free_player(player_t* player) {
    if (player == NULL) return;
    free(player->name);
    free(player->address);
    free(player);
}

int is_player(player_t* player, char *name, SA_IN* address) {
    return strcmp(player->name, name) == 0 && memcmp(&player->address->sin_addr, &address->sin_addr, sizeof(struct in_addr)) == 0;
}

void print_player(player_t* player) {
    printf("Player Info:\n");
    printf("\tRole: %c\n", player->role);
    printf("\tName: %s\n", player->name);
    printf("\tAddress: %s:%d\n", inet_ntoa(player->address->sin_addr), ntohs(player->address->sin_port));
    printf("\tSocket: %d\n", player->socket);
}