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
    // stores the total amount of turns that have occured within the game
    int totalTurns; 
    // all the pastPlays(PP) seperated, so that they can be individually accessed through the array
    char** seperatedPP;
    
};

// #    #  ######  #    # #     #  #    #  #    #   #####  ######  #####
// ##   #  #       #    # #     #  #    #  ##   #     #    #       #    #
// # #  #  #####   #    # #######  #    #  # #  #     #    #####   #    #
// #  # #  #       # ## # #     #  #    #  #  # #     #    #       #####
// #   ##  #       ##  ## #     #  #    #  #   ##     #    #       #   #
// #    #  ######  #    # #     #   ####   #    #     #    ######  #    #
//SPLIT STRING PLAY INTO 2D array with 7 chars. 
HunterView newHunterView( char *pastPlays, playerMessage messages[] ) {
    HunterView hunterView = malloc( sizeof( *hunterView ) );
    hunterView->score = GAME_START_SCORE;
    hunterView->player = PLAYER_LORD_GODALMING;

    int i,k;
    int counter;
    
    counter = 0;
    hunterView->totalTurns = strlen(pastPlays)/PLAYLEN;
    
    // Initialise the 2D array of strings
    // http://stackoverflow.com/a/14583642
    // Initialise an array of pointers for the  amount of total turns
    hunterView->seperatedPP = malloc (hunterView->totalTurns * sizeof(char*));
    assert(hunterView->seperatedPP != NULL);

    // Intialise a string for every turn
    for(k = 0; k < hunterView->totalTurns; k++) {
        hunterView->seperatedPP[k] = malloc((PLAYLEN+1)*sizeof(char));
        assert(hunterView->seperatedPP[k] != NULL);
    }

    for(i=0; i<hunterView->totalTurns; i++){

        int j;
        for(j=0; j<PLAYLEN; j++){
            
            hunterView->seperatedPP[i][j] = pastPlays[counter];
            counter++;
        }
        hunterView->seperatedPP[i][PLAYLEN] = '\0';
        counter++;
    }
    
    for (i=0; i<hunterView->totalTurns; i++) {
        printf ("[%d] %s\n", i, hunterView->seperatedPP[i]);
    }

    printf("total turns = %d\n", hunterView->totalTurns);
    //hunterView->score = calculateScore(finalPlay);
    return hunterView;
}

//static int calculateScore (finalPlay);


//                         #####
//  ####   ######   ##### #     #   ####    ####   #####   ######
// #    #  #          #   #        #    #  #    #  #    #  #
// #       #####      #    #####   #       #    #  #    #  #####
// #  ###  #          #         #  #       #    #  #####   #
// #    #  #          #   #     #  #    #  #    #  #   #   #
//  ####   ######     #    #####    ####    ####   #    #  ######
//Get the current score
// Returns a positive integer [0...366]
int getScore(HunterView currentView){
    int score;
    score = currentView -> score;
    return score;
}  

// #####                          ######
//#     #  #    #  #####   #####  #     #  #         ##     #   #  ######  #####
//#        #    #  #    #  #    # #     #  #        #  #     # #   #       #    #
//#        #    #  #    #  #    # ######   #       #    #     #    #####   #    #
//#        #    #  #####   #####  #        #       ######     #    #       #####
//#     #  #    #  #   #   #   #  #        #       #    #     #    #       #   #
// #####    ####   #    #  #    # #        ######  #    #     #    ######  #    #

//Get the id of current player - ie whose turn is it?
// Only returns a 'playerID' which is one of:
//   LORD_GODALMING (0): Lord Godalming's turn
//   DR_SEWARD      (1): Dr. Seward's turn
//   VAN_HELSING    (2): Van Helsing's turn
//   MINA_HARKER    (3): Mina Harker's turn
//   DRACULA        (4): Dracula's turn
PlayerID getCurrentPlayer (HunterView currentView){

    PlayerID currentPlayer = PLAYER_LORD_GODALMING;
    char currentPlayer = currentView->seperatedPP[0][currentView->totalTurns-1];
    //printf("currentPlayer = *%c*\n", currentPlayer);
    if (currentPlayer == 'G') {
        return PLAYER_SEWARD;
    } else if (currentPlayer == 'S') {
        return PLAYER_VAN_HELSING;
    } else if (currentPlayer == 'H') {
        return PLAYER_MINA_HAKER;
    } else if (currentPlayer == 'M') {
        return PLAYER_DRACULA;
    } else if (currentPlayer == 'D') {
        return PLAYER_LORD_GODALMING;
    }
}

// #####      #     ####   #####    ####    ####   ######
// #    #     #    #       #    #  #    #  #       #
// #    #     #     ####   #    #  #    #   ####   #####
// #    #     #         #  #####   #    #       #  #
// #    #     #    #    #  #       #    #  #    #  #
// #####      #     ####   #        ####    ####   ######
     
// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView( HunterView toBeDeleted ) {
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}

