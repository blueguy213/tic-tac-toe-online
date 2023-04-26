#include "lobby.h"

// Create a new lobby
lobby_t* new_lobby(char *name, char *password, int max_players) {
    lobby_t* lobby = malloc(sizeof(lobby_t));
    lobby->name = name;
    lobby->max_players = max_players;
    lobby->num_players = 0;
    lobby->max_games = max_players / 2;
    lobby->num_games = 0;
    lobby->games = NULL;
    lobby->players = malloc(sizeof(player_t*) * max_players);
    lobby->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    return lobby;
}

// Free a lobby
void free_lobby(lobby_t* lobby) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->mutex);
    free(lobby->name);
    for (int i = 0; i < lobby->player_count; i++) {
        free_player(lobby->players[i]);
    }
    free(lobby->players);
    if (lobby->game != NULL) {
        free_game(lobby->game);
    }
    // Unlock the lobby mutex
    pthread_mutex_unlock(&lobby->mutex);
    free(lobby);
}

// Check if a lobby is full
bool is_lobby_full(lobby_t* lobby) {
    return lobby->player_count == lobby->max_players;
}

/**
 * Add a player to the lobby and return the index of the player (or some error code)
 * -1 = lobby is full
 * -2 = player already in lobby
 * */ 
int add_player(lobby_t* lobby, player_t* player) {
    // Lock the lobby mutex
    pthread_mutex_lock(&lobby->mutex);
    // Check if the lobby is full
    if (is_lobby_full(lobby)) {
        pthread_mutex_unlock(&lobby->mutex);
        return -1;
    }
    // Check if the player is already in the lobby
    for (int i = 0; i < lobby->player_count; i++) {
        if (is_player(lobby->players[i], player->name, player->address)) {
            pthread_mutex_unlock(&lobby->mutex);
            return -2;
        }
    }

    // Add the player to the lobby
    int player_index = lobby->player_count;
    lobby->players[lobby->player_count] = player;
    lobby->player_count++;
    pthread_mutex_unlock(&lobby->mutex);
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
    pthread_mutex_lock(&lobby->mutex);
    // Check if the lobby is empty
    if (lobby->player_count == 0) {
        pthread_mutex_unlock(&lobby->mutex);
        return -1;
    }
    // Check if the player is in the lobby
    for (int i = 0; i < lobby->player_count; i++) {
        if (is_player(lobby->players[i], name, address)) {
            pthread_mutex_unlock(&lobby->mutex);
            return -2;
        }
    }
    // Check if the player is in a game that is in the lobby
    for (int i = 0; i < lobby->game_count; i++) {
        if (is_player(lobby->games[i]->x, name, address) || is_player(lobby->games[i]->o, name, address)) {
            pthread_mutex_unlock(&lobby->mutex);
            return -3;
        }
    }

    // Remove the player from the lobby and shift the array
    for (int i = 0; i < lobby->player_count; i++) {
        if (is_player(lobby->players[i], name, address)) {
            free_player(lobby->players[i]);
            for (int j = i; j < lobby->player_count - 1; j++) {
                lobby->players[j] = lobby->players[j + 1];
            }
            lobby->player_count--;
            pthread_mutex_unlock(&lobby->mutex);
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
    pthread_mutex_lock(&lobby->mutex);
    // Check if the lobby is full
    if (lobby->game_count == lobby->max_games) {
        pthread_mutex_unlock(&lobby->mutex);
        return -1;
    }
    // Check if the game is already in the lobby
    for (int i = 0; i < lobby->game_count; i++) {
        if (lobby->games[i] == game) {
            pthread_mutex_unlock(&lobby->mutex);
            return -2;
        }
    }
    // Check if the players are in the lobby
    if (!is_player_in_lobby(lobby, x) || !is_player_in_lobby(lobby, o)) {
        pthread_mutex_unlock(&lobby->mutex);
        return -3;
    }
    // Add the game to the lobby
    lobby->games[lobby->game_count] = game;
    int game_index = game_count;
    lobby->game_count++;
    pthread_mutex_unlock(&lobby->mutex);
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
    pthread_mutex_lock(&lobby->mutex);
    // Check if the lobby is empty
    if (lobby->game_count == 0) {
        pthread_mutex_unlock(&lobby->mutex);
        return -1;
    }
    // Check if the game is in the lobby
    for (int i = 0; i < lobby->game_count; i++) {
        if (lobby->games[i] == game) {
            // Check if the game is in progress
            if (game->state < O_WON || game->state > DRAW) {
                pthread_mutex_unlock(&lobby->mutex);
                return -3;
            }
            pthread_mutex_unlock(&lobby->mutex);
            return -2;
        }
    }
}

// Print the lobby to stdout
int print_lobby(lobby_t* lobby) {

}