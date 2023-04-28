#ifndef _LOBBY_H_
    
    #define _LOBBY_H_

    #include "../common/utils.h"

    // Definition of lobby_t struct
    typedef struct {
        int num_players;
        int max_players;
        int num_games;
        int max_games;
        game_t* games[5];
        player_t* players[10];
        player_t* waiting_player;
        pthread_mutex_t lock;
    } lobby_t;

    // Function prototypes
    lobby_t* new_lobby(int max_players);
    void free_lobby(lobby_t* lobby); // Free the memory allocated for a lobby and all of its games and players
    int is_lobby_full(lobby_t* lobby); // Check if the lobby is full (returns 1 if full, 0 if not)
    int add_player(lobby_t* lobby, player_t* player); // Add a player to the lobby and return the index of the player (or some error code)
    int remove_player(lobby_t* lobby, char* name, SA_IN* address); // Remove a player from the lobby and return the index of the player (or some error code)
    int add_game(lobby_t* lobby, game_t* game, player_t* x, player_t* o); // Add a game to the lobby and return the index of the game (or some error code)
    int remove_game(lobby_t* lobby, game_t* game); // Remove a game from the lobby and return the index of the game (or some error code)
    void print_lobby(lobby_t* lobby); // Print the lobby to stdout

#endif // _LOBBY_H_