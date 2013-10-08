#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include <string.h>

#define PLAYLEN 7
     
struct hunterView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int score;
    PlayerID player;
};
     
//SPLIT STRING PLAY INTO 2D array with 7 chars. 
HunterView newHunterView( char *pastPlays, playerMessage messages[] ) {
    HunterView hunterView = malloc( sizeof( *hunterView ) );
    hunterView->score = GAME_START_SCORE;
    hunterView->player = PLAYER_LORD_GODALMING;
    
    int i;
    int totalPlay;
    int counter;
    
    counter = 0;
    totalPlay = strlen(pastPlays)/PLAYLEN;
    char finalPlay[totalPlay][PLAYLEN+1];
    
    for(i=0; i<totalPlay; i++){
        int j;
        
        for(j=0; j<PLAYLEN; j++){
            
            finalPlay[i][j] = pastPlays[counter];
            finalPlay[i][7] = '\0';
            counter++;
           
        }
        counter++;
    }
    
    for (i=0; i<totalPlay; i++) {
        printf ("[%d]", i);
        printf ("%s\n", finalPlay[i]);
    }
    //hunterView->score = calculateScore(finalPlay);
    return hunterView;
}
     
//static int calculateScore (finalPlay);

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

