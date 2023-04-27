#ifndef _GAME_H_

    #define _GAME_H_

    #include "utils.h"
    #include "player.h"

    typedef enum {
        X = 1,
        O = 2,
        DRAW = 0,
        X_WON = -1,
        O_WON = -2,
        X_OFFERED_DRAW = -3,
        O_OFFERED_DRAW = -4,
        UNSTARTED = -5
    } State;

    typedef struct {
        char* board; // The game board as a 9 character string (3 rows of 3 characters)
        pthread_t tid;
        State state;
        pthread_mutex_t lock; // Mutex lock for the game state since it is shared between both players' threads
        player_t* playerX; // Pointer to the player with the X role
        player_t* playerO; // Pointer to the player with the O role
    } game_t;

    game_t* new_game(player_t* x, player_t* o); // Initialize a new game and return a pointer to it
    void free_game(game_t* game); // Free the memory allocated for a game
    void draw(game_t* game, player_t* player); // Update the game state to a draw by the given player
    void move(game_t* game, player_t* player, int x, int y); // Update the game state with the given move by the given player
    void resign(game_t* game, player_t* player); // Update the game state to a resignation by the given player
    void print_board_as_display(game_t* game); // Return the game board as a display
    int check_game(game_t* game); // Check if the game is over
    char** gamemaster(game_t* game, char* input, player_t* game_over); // Handle the game logic

#endif // _GAME_H_