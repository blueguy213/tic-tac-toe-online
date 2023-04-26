#include "lobby.h"

// Create a new lobby
lobby_t* new_lobby(char *name, char *password, int max_players) {
    lobby_t* lobby = malloc(sizeof(lobby_t));
    lobby->max_players = max_players;
    lobby->num_players = 0;
    lobby->max_games = max_players / 2;
    lobby->num_games = 0;
    lobby->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    return lobby;
}

// Free a lobby
void free_lobby(lobby_t* lobby) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->lock);
    for (int i = 0; i < lobby->num_players; i++) {
        free_player(&(lobby->players[i]));
    }
    for (int i = 0; i < lobby->num_games; i++) {
        free_game(&(lobby->games[i]));
    }
    // Unlock the lobby mutex
    pthread_mutex_unlock(&lobby->lock);
    free(lobby);
}

// Check if a lobby is full
int is_lobby_full(lobby_t* lobby) {
    return lobby->num_players == lobby->max_players;
}

// Check if a player is in the lobby (by name and address)
int is_player_in_lobby(lobby_t* lobby, char* name, SA_IN address) {
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(&lobby->players[i], name, address)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Add a player to the lobby and return the index of the player (or some error code)
 * -1 = lobby is full
 * -2 = player already in lobby
 * */ 
int add_player(lobby_t* lobby, player_t* player) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->lock);
    // Check if the lobby is full
    if (is_lobby_full(lobby)) {
        pthread_mutex_unlock(&lobby->lock);
        return -1;
    }
    // Check if the player is already in the lobby
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(&lobby->players[i], player->name, player->address)) {
            pthread_mutex_unlock(&lobby->lock);
            return -2;
        }
    }

    // Add the player to the lobby
    int player_index = lobby->num_players;
    lobby->players[lobby->num_players] = player;
    lobby->num_players++;
    pthread_mutex_unlock(&lobby->lock);
    return player_index;
}

/**
 * Remove a player from the lobby and return the index of the player (or some error code)
 * -1 = lobby is empty
 * -2 = player not in lobby
 * -3 = player in game
 * */ 
int remove_player(lobby_t* lobby, char* name, SA_IN address) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->lock);
    // Check if the lobby is empty
    if (lobby->num_players == 0) {
        pthread_mutex_unlock(&lobby->lock);
        return -1;
    }
    // Check if the player is in the lobby
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(&lobby->players[i], name, address)) {
            pthread_mutex_unlock(&lobby->lock);
            return -2;
        }
    }
    // Check if the player is in a game that is in the lobby
    for (int i = 0; i < lobby->num_games; i++) {
        if (is_player(&lobby->games[i]->playerX, name, address) || is_player(lobby->games[i]->playerO, name, address)) {
            pthread_mutex_unlock(&lobby->lock);
            return -3;
        }
    }

    // Remove the player from the lobby and shift the array
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(&lobby->players[i], name, address)) {
            free_player(&lobby->players[i]);
            for (int j = i; j < lobby->num_players - 1; j++) {
                lobby->players[j] = lobby->players[j + 1];
            }
            lobby->num_players--;
            pthread_mutex_unlock(&lobby->lock);
            return i;
        }
    }
}

/**
 * Add a game to the lobby and return the index of the game (or some error code)
 * -1 = lobby is full
 * -2 = game already in lobby
 * -3 = player not in lobby
 * */ 
int add_game(lobby_t* lobby, game_t* game, player_t* x, player_t* o) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->lock);
    // Check if the lobby is full
    if (lobby->num_games == lobby->max_games) {
        pthread_mutex_unlock(&lobby->lock);
        return -1;
    }
    // Check if the game is already in the lobby
    for (int i = 0; i < lobby->num_games; i++) {
        if (lobby->games[i] == game) {
            pthread_mutex_unlock(&lobby->lock);
            return -2;
        }
    }
    // Check if the players are in the lobby
    if (!is_player_in_lobby(lobby, x->name, x->address) || !is_player_in_lobby(lobby, o->name, o->address)) {
        pthread_mutex_unlock(&lobby->lock);
        return -3;
    }
    // Add the game to the lobby
    lobby->games[lobby->num_games] = game;
    int game_index = lobby->num_games;
    lobby->num_games++;
    pthread_mutex_unlock(&lobby->lock);
    return game_index;
}

/**
 * Remove a game from the lobby and return the index of the game (or some error code)
 * -1 = lobby is empty
 * -2 = game not in lobby
 * -3 = game in progress
 * */ 
int remove_game(lobby_t* lobby, game_t* game) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->lock);
    // Check if the lobby is empty
    if (lobby->num_games == 0) {
        pthread_mutex_unlock(&lobby->lock);
        return -1;
    }
    // Check if the game is in the lobby
    for (int i = 0; i < lobby->num_games; i++) {
        if (lobby->games[i] == game) {
            // Check if the game is in progress
            if (game->state < O_WON || game->state > DRAW) {
                pthread_mutex_unlock(&lobby->lock);
                return -3;
            }
            pthread_mutex_unlock(&lobby->lock);
            return -2;
        }
    }
}

// Print the lobby to stdout
int print_lobby(lobby_t* lobby) {
    // Print the players with the players formatted as "name:address\n" 
    for (int i = 0; i < lobby->num_players; i++) {
        printf("%s:%s\n", lobby->players[i]->name, lobby->players[i]->address);
    }

    // Print the games formatted as "board\nstate:state\nx:name:address\no:name:address\n"
    for (int i = 0; i < lobby->num_games; i++) {
        printf("%s\nstate:%d\nx:%s:%s\no:%s:%s\n", lobby->games[i]->board, lobby->games[i]->state, lobby->games[i]->playerX->name, lobby->games[i]->playerX->address, lobby->games[i]->playerO->name, lobby->games[i]->playerO->address);
    }
}