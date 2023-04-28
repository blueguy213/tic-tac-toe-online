#include "lobby.h"

// Create a new lobby
lobby_t* new_lobby(int max_players) {
    lobby_t* lobby = malloc(sizeof(lobby_t));
    lobby->max_players = max_players;
    lobby->num_players = 0;
    lobby->max_games = max_players / 2;
    lobby->num_games = 0;
    lobby->waiting_player = NULL;
    lobby->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    return lobby;
}

// Free a lobby
void free_lobby(lobby_t* lobby) {
    // Lock the lobby mutex
    
    for (int i = 0; i < lobby->num_players; i++) {
        free_player(lobby->players[i]);
    }
    for (int i = 0; i < lobby->num_games; i++) {
        free_game(lobby->games[i]);
    }
    // Unlock the lobby mutex
    
    free(lobby);
}

// Check if a lobby is full
int is_lobby_full(lobby_t* lobby) {
    return lobby->num_players == lobby->max_players;
}

// Check if a player is in the lobby (by name and address)
int is_player_in_lobby(lobby_t* lobby, char* name, SA_IN* address) {
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(lobby->players[i], name, address)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Add a player to the lobby and return the index of the player (or some error code)
 * -1 = lobby is full
 * -2 = A player with the same name and address is already in the lobby
 * -3 = There is no waiting player
 * 0 <= i <= max_games = There is a waiting player and the index of the game that the players are in
 * */ 
int add_player(lobby_t* lobby, player_t* player) {
    // Lock the lobby mutex
    
    // Check if the lobby is full
    if (is_lobby_full(lobby)) {
        
        return -1;
    }
    // Check if the player is already in the lobby
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(lobby->players[i], player->name, player->address)) {
            return -2;
        }
    }
    printf("Adding player %s to lobby\n", player->name);
    lobby->players[lobby->num_players] = player;
    lobby->num_players++;
    // Check if there is a waiting player
    if (lobby->waiting_player == NULL) {
        lobby->waiting_player = player;
        return -3;
    }
    // Create a new game
    game_t* game = new_game(lobby->waiting_player, player);
    // Add the game to the lobby
    lobby->games[lobby->num_games] = game;
    lobby->num_games++;
    // Remove the waiting player
    lobby->waiting_player = NULL;
    // Unlock the lobby mutex
    printf("Added player %s to lobby\n", player->name);
    return lobby->num_games - 1;
}

/**
 * Remove a player from the lobby and return the index of the player (or some error code)
 * -1 = lobby is empty
 * -2 = player not in lobby
 * -3 = player in game
 * 0 <= i <= max_players = index of the player that was removed from the lobby
 * */ 
int remove_player(lobby_t* lobby, char* name, SA_IN* address) {    
    // Check if the lobby is empty
    if (lobby->num_players == 0) {
        return -1;
    }

    // Remove the player from the lobby and shift the array
    for (int i = 0; i < lobby->num_players; i++) {
        if (is_player(lobby->players[i], name, address)) {
            printf("Removing player %s from lobby\n", lobby->players[i]->name);
            free_player(lobby->players[i]);
            for (int j = i; j < lobby->num_players - 1; j++) {
                lobby->players[j] = lobby->players[j + 1];
            }
            lobby->num_players--;
            return i;
        }
    }
    return -2;
}

/**
 * Add a game to the lobby and return the index of the game (or some error code)
 * -1 = lobby is full
 * -2 = game already in lobby
 * -3 = player not in lobby
 *  0 <= i <= max_games = index of the game that was added to the lobby
 * */ 
int add_game(lobby_t* lobby, game_t* game, player_t* x, player_t* o) {
    // Lock the lobby mutex
    
    // Check if the lobby is full
    if (lobby->num_games == lobby->max_games) {
        return -1;
    }
    // Check if the game is already in the lobby
    for (int i = 0; i < lobby->num_games; i++) {
        if (lobby->games[i] == game) {
            return -2;
        }
    }
    // Check if the players are in the lobby
    if (!is_player_in_lobby(lobby, x->name, x->address) || !is_player_in_lobby(lobby, o->name, o->address)) {
        return -3;
    }
    // Add the game to the lobby
    lobby->games[lobby->num_games] = game;
    int game_index = lobby->num_games;
    lobby->num_games++;
    
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
    
    // Check if the lobby is empty
    if (lobby->num_games == 0) {
        return -1;
    }
    // Check if the game is in the lobby
    for (int i = 0; i < lobby->num_games; i++) {
        if (lobby->games[i]->tid == game->tid) {
            printf("Removing game %d running on thread %ld\n", i, game->tid);
            // Remove the game from the lobby and shift the array back to fill the gap
            free_game(lobby->games[i]);
            for (int j = i; j < lobby->num_games - 1; j++) {
                lobby->games[j] = lobby->games[j + 1];
            }
            lobby->num_games--;
            return i;
        }
    }
    return -2;
}

// Print the lobby to stdout
void print_lobby(lobby_t* lobby) {
    
    printf("Lobby:\n");
    // Print the players with the players formatted as "\tname:name\nSA_IN:address\n"
    printf("\tPlayers: %d\n", lobby->num_players);
    for (int i = 0; i < lobby->num_players; i++) {
        printf("\t\t%s\n", lobby->players[i]->name);
    }

    // Print the games formatted as "board\nstate:state\nx:name:address\no:name\n"
    printf("\tGames: %d\n", lobby->num_games);
    for (int i = 0; i < lobby->num_games; i++) {
        printf("\t\tGame %d:\n", i);
        printf("\t\t\tboard: %s", lobby->games[i]->board);
        printf("\t\t\tstate: ");
        print_state(lobby->games[i]->state);
        printf("\n\t\t\tx: %s", lobby->games[i]->playerX->name);
        printf("\t\t\to: %s\n", lobby->games[i]->playerO->name);
    }
    printf("\n");
    fflush(stdout);
    
}