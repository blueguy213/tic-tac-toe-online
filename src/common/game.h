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
        O_OFFERED_DRAW = -4
    } State;

    typedef struct {
        char* board; // The game board as a 9 character string (3 rows of 3 characters)

        //  1 if it is X's turn
        //  2 if it is O's turn
        //  0 if the game is over and a draw
        // -1 if the game is won by X
        // -2 if the game is won by O
        // -3 if the game is a draw offered by X
        // -4 if the game is a draw offered by O
        State state;

        pthread_mutex_t lock; // Mutex lock for the game state since it is shared between both players' threads
        player_t* playerX; // Pointer to the player with the X role
        player_t* playerO; // Pointer to the player with the O role
    } game_t;

    Game* new_game(); // Initialize a new game and return a pointer to it
    void free_game(Game* game); // Free the memory allocated for a game
    void draw(Game* game, Player* player); // Update the game state to a draw by the given player
    void move(Game* game, Player* player, int x, int y); // Update the game state with the given move by the given player
    void resign(Game* game, Player* player); // Update the game state to a resignation by the given player
    void print_board_as_display(Game* game); // Return the game board as a display
    int check_game(Game* game); // Check if the game is over
    char** gamemaster(game_t* game, char* input, player_t* game_over); // Handle the game logic

#endif // _GAME_H_