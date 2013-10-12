#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include <string.h>

#define PLAYLEN 7

static LocationID translateLocationID(char*);
     
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

    int i;
    int counter;
    
    counter = 0;
    hunterView->totalTurns = (strlen(pastPlays)+1)/(PLAYLEN+1);
    
    // Initialise the 2D array of strings
    // http://stackoverflow.com/a/14583642
    // Initialise an array of pointers for the  amount of total turns
    hunterView->seperatedPP = malloc (hunterView->totalTurns * sizeof(char*));
    assert(hunterView->seperatedPP != NULL);

    // Intialise a string for every turn
    for(i = 0; i < hunterView->totalTurns; i++) {
        hunterView->seperatedPP[i] = malloc(sizeof(char));
        assert(hunterView->seperatedPP[i] != NULL);
    }

    for(i=0; i<hunterView->totalTurns; i++){

        int j;
        for(j=0; j<PLAYLEN; j++){
            
            hunterView->seperatedPP[i][j] = pastPlays[counter];
            counter++;
        }
        //hunterView->seperatedPP[i][PLAYLEN] = '\0';
        counter++;
    }
    
    for (i=0; i<hunterView->totalTurns; i++) {
        printf ("[%d]*%s*\n", i, hunterView->seperatedPP[i]);
    }

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
    char currentPlayer = currentView->seperatedPP[currentView->totalTurns-1][0];
    printf("currentPlayer = *%c*\n", currentPlayer);
    if (currentPlayer == 'G') {
        return PLAYER_DR_SEWARD;
    } else if (currentPlayer == 'S') {
        return PLAYER_VAN_HELSING;
    } else if (currentPlayer == 'H') {
        return PLAYER_MINA_HARKER;
    } else if (currentPlayer == 'M') {
        return PLAYER_DRACULA;
    } else {
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

//Get the current round
Round getRound (HunterView currentView) {
    return currentView->totalTurns/5;
}

// Fills the trail array with the location ids of the last 6 turns for the given player
// For dracula this may include integers:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea

//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula


// For any player if the move does not exist yet (i.e, the start of the game),
// the value should be UNKNOWN_LOCATION (-1)
// For example after 2 turns the array may have the contents
// {29, 182, -1, -1, -1, -1} 
// This would mean in the first move the player started on location 182 
// then moved to the current location of 29
void getHistory (HunterView currentView, PlayerID player,LocationID trail[TRAIL_SIZE]) {
    int i;
    char location[3];
    location[2] = '\0';

    // Fill array with -1
    for (i=0; i<TRAIL_SIZE; i++) {
        trail[i] = -1;
    }

    Round currentRound = getRound(currentView);
    printf("totalround = %d\n", currentRound);

    if (currentRound > 0) {
        for(i=0; i < getRound(currentView); i++) {
            printf("round = %d, playerid = %d\n", i, player);
            location[0] = currentView->seperatedPP[(i*5)+player][1];
            location[1] = currentView->seperatedPP[(i*5)+player][2];
            trail[i] = translateLocationID(location);
            printf("location = **%s**\n", location);
        }
        if (currentView->totalTurns%5 > player) {
            printf("round = %d, playerid = %d\n", i, player);
            location[0] = currentView->seperatedPP[(i*5)+player][1];
            location[1] = currentView->seperatedPP[(i*5)+player][2];
            trail[i] = translateLocationID(location);
            printf("location = **%s**\n", location);
        }
    }

    for (i=0; i<TRAIL_SIZE; i++) {
        printf("history [%d] = %d\n", i, trail[i]);
    }

}
