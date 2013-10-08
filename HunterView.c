#include <stdlib.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
     
typedef struct _hunterView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int score;
    playerID player;
}hunterView;
     
<<<<<<< HEAD
     
     //NEIL A TEST
=======
// TETTST
>>>>>>> 1eaef9af84673d9dad0c1abe9875913f79958225

HunterView newHunterView( char *pastPlays, playerMessage messages[] ) {
    HunterView hunterView = malloc( sizeof( struct hunterView ) );
    hunterView->score = 366;
    hunterView->player = -1;
    return hunterView;
}
     
//Get the current score
// Returns a positive integer [0...366]
int getScore(HunterView currentView){
    int score;
    score = currentView -> score;
    
    return score;
}  

//Get the id of current player - ie whose turn is it?
// Only returns a 'playerID' which is one of:
//   LORD_GODALMING (0): Lord Godalming's turn
//   DR_SEWARD      (1): Dr. Seward's turn
//   VAN_HELSING    (2): Van Helsing's turn
//   MINA_HARKER    (3): Mina Harker's turn
//   DRACULA        (4): Dracula's turn
PlayerID getCurrentPlayer (HunterView currentView){

    PlayerID player;
    player = currentView -> player;
    return player;
}
     
// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView( HunterView toBeDeleted ) {
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}
