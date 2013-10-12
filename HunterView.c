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
    char lastPlayer = currentView->seperatedPP[currentView->totalTurns-1][0];
    printf("lastPlayer = *%c*\n", lastPlayer);
    if (lastPlayer == 'G') {
        return PLAYER_DR_SEWARD;
    } else if (lastPlayer == 'S') {
        return PLAYER_VAN_HELSING;
    } else if (lastPlayer == 'H') {
        return PLAYER_MINA_HARKER;
    } else if (lastPlayer == 'M') {
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

// #        ####    ####     ##     #####     #     ####   #    #
// #       #    #  #    #   #  #      #       #    #    #  ##   #
// #       #    #  #       #    #     #       #    #    #  # #  #
// #       #    #  #       ######     #       #    #    #  #  # #
// #       #    #  #    #  #    #     #       #    #    #  #   ##
// ######   ####    ####   #    #     #       #     ####   #    #

// Get the current location id of a given player
// May be UNKNOWN_LOCATION if the player has not had a turn yet
// (ie at the beginning of the game when the round is 0)
// otherwise for a hunter it should be an integer in the interval [0..70] 
// The given roundNumber is >= 0 and <= the current round number
// 'whichHunter' specifies which hunter's location to return
//    and must be a value in the interval [0...3] (see 'player' type)
// Or for dracula it should 
// gets the location of Dracula at the start of a particular round
// Returns an integer:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea
//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have 
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula
//   LOCATION_UNKNOWN if the round number is 0
LocationID getLocation(HunterView currentView, PlayerID player) {

    char location[3];
    location[2] = '\0';

    Round roundsPlayed = getRound(currentView);
    if (currentView->totalTurns % 5 > player)
        roundsPlayed++;

    location[0] = currentView->seperatedPP[((roundsPlayed-1)*5)+player][1];
    location[1] = currentView->seperatedPP[((roundsPlayed-1)*5)+player][2];

    // check that a move has been made otherwise return -1
    if (roundsPlayed > 0) { 
        return translateLocationID(location);
    } else {
        return UNKNOWN_LOCATION;
    }
}


//  ####   ######   ##### #     #     #     ####    #####   ####   #####    #   #
// #    #  #          #   #     #     #    #          #    #    #  #    #    # #
// #       #####      #   #######     #     ####      #    #    #  #    #     #
// #  ###  #          #   #     #     #         #     #    #    #  #####      #
// #    #  #          #   #     #     #    #    #     #    #    #  #   #      #
//  ####   ######     #   #     #     #     ####      #     ####   #    #     #

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

    // array to store the 2 letter id pulled from pastplays
    char location[3];
    location[2] = '\0';

    // Fill array with -1
    for (i=0; i<TRAIL_SIZE; i++) {
        trail[i] = -1;
    }

    // 
    Round roundsPlayed = getRound(currentView);
    if (currentView->totalTurns%5 > player) {
        roundsPlayed++;
    }
    printf("totalround = %d\n", roundsPlayed);

    if (roundsPlayed > 0 && roundsPlayed <= 5) {
        for(i=0; i < roundsPlayed; i++) {
            printf("round = %d, playerid = %d weee = %d\n", i, player, (((roundsPlayed-1)*5)-(i*5))+player);
            location[0] = currentView->seperatedPP[(((roundsPlayed-1)*5)-(i*5))+player][1];
            location[1] = currentView->seperatedPP[(((roundsPlayed-1)*5)-(i*5))+player][2];
            trail[i] = translateLocationID(location);
            printf("location = **%s**\n", location);
        }
    } else if (roundsPlayed >= 6) {
        int start = roundsPlayed*5;
        for(i=TRAIL_SIZE; i > 0; i--) {
            printf("round = %d, playerid = %d weee = %d\n", i, player, (start-(i*5)));
            location[0] = currentView->seperatedPP[start-(i*5)][1];
            location[1] = currentView->seperatedPP[start-(i*5)][2];
            trail[i-1] = translateLocationID(location);
            printf("location = **%s**\n", location);
        }
    }

    for (i=0; i<TRAIL_SIZE; i++) {
        printf("history [%d] = %d\n", i, trail[i]);
    }

}

static LocationID translateLocationID (char* locationCode) {
    // ==== "Ci"ties ====
    if (strcmp(locationCode, "AL") == 0) {
        return ALICANTE;
    } else if (strcmp(locationCode, "AM") == 0) {
        return AMSTERDAM;
    } else if (strcmp(locationCode, "AT") == 0) {
        return ATHENS;
    } else if (strcmp(locationCode, "BA") == 0) {
        return BARCELONA;
    } else if (strcmp(locationCode, "BI") == 0) {
        return BARI;
    } else if (strcmp(locationCode, "BE") == 0) {
        return BELGRADE;
    } else if (strcmp(locationCode, "BR") == 0) {
        return BERLIN;
    } else if (strcmp(locationCode, "BO") == 0) {
        return BORDEAUX;
    } else if (strcmp(locationCode, "BU") == 0) {
        return BRUSSELS;
    } else if (strcmp(locationCode, "BC") == 0) {
        return BUCHAREST;
    } else if (strcmp(locationCode, "BD") == 0) {
        return BUDAPEST;
    } else if (strcmp(locationCode, "CA") == 0) {
        return CADIZ;
    } else if (strcmp(locationCode, "CG") == 0) {
        return CAGLIARI;
    } else if (strcmp(locationCode, "CD") == 0) {
        return CASTLE_DRACULA;
    } else if (strcmp(locationCode, "CF") == 0) {
        return CLERMONT_FERRAND;
    } else if (strcmp(locationCode, "CO") == 0) {
        return COLOGNE;
    } else if (strcmp(locationCode, "CN") == 0) {
        return CONSTANTA;
    } else if (strcmp(locationCode, "DU") == 0) {
        return DUBLIN;
    } else if (strcmp(locationCode, "ED") == 0) {
        return EDINBURGH;
    } else if (strcmp(locationCode, "FL") == 0) {
        return FLORENCE;
    } else if (strcmp(locationCode, "FR") == 0) {
        return FRANKFURT;
    } else if (strcmp(locationCode, "GA") == 0) {
        return GALATZ;
    } else if (strcmp(locationCode, "GW") == 0) {
        return GALWAY;
    } else if (strcmp(locationCode, "GE") == 0) {
        return GENEVA;
    } else if (strcmp(locationCode, "GO") == 0) {
        return GENOA;
    } else if (strcmp(locationCode, "GR") == 0) {
        return GRANADA;
    } else if (strcmp(locationCode, "HA") == 0) {
        return HAMBURG;
    } else if (strcmp(locationCode, "JM") == 0) {
        return ST_JOSEPH_AND_ST_MARYS;
    } else if (strcmp(locationCode, "KL") == 0) {
        return KLAUSENBURG;
    } else if (strcmp(locationCode, "LE") == 0) {
        return LE_HAVRE;
    } else if (strcmp(locationCode, "LI") == 0) {
        return LEIPZIG;
    } else if (strcmp(locationCode, "LS") == 0) {
        return LISBON;
    } else if (strcmp(locationCode, "LV") == 0) {
        return LIVERPOOL;
    } else if (strcmp(locationCode, "LO") == 0) {
        return LONDON;
    } else if (strcmp(locationCode, "MA") == 0) {
        return MADRID;
    } else if (strcmp(locationCode, "MN") == 0) {
        return MANCHESTER;
    } else if (strcmp(locationCode, "MR") == 0) {
        return MARSEILLES;
    } else if (strcmp(locationCode, "MI") == 0) {
        return MILAN;
    } else if (strcmp(locationCode, "MU") == 0) {
        return MUNICH;
    } else if (strcmp(locationCode, "NA") == 0) {
        return NANTES;
    } else if (strcmp(locationCode, "NP") == 0) {
        return NAPLES;
    } else if (strcmp(locationCode, "NU") == 0) {
        return NUREMBURG;
    } else if (strcmp(locationCode, "PA") == 0) {
        return PARIS;
    } else if (strcmp(locationCode, "PL") == 0) {
        return PLYMOUTH;
    } else if (strcmp(locationCode, "PR") == 0) {
        return PRAGUE;
    } else if (strcmp(locationCode, "RO") == 0) {
        return ROME;
    } else if (strcmp(locationCode, "SA") == 0) {
        return SALONICA;
    } else if (strcmp(locationCode, "SN") == 0) {
        return SANTANDER;
    } else if (strcmp(locationCode, "SR") == 0) {
        return SARAGOSSA;
    } else if (strcmp(locationCode, "SJ") == 0) {
        return SARAJEVO;
    } else if (strcmp(locationCode, "SO") == 0) {
        return SOFIA;
    } else if (strcmp(locationCode, "ST") == 0) {
        return STRASBOURG;
    } else if (strcmp(locationCode, "SW") == 0) {
        return SWANSEA;
    } else if (strcmp(locationCode, "SZ") == 0) {
        return SZEGED;
    } else if (strcmp(locationCode, "TO") == 0) {
        return TOULOUSE;
    } else if (strcmp(locationCode, "VA") == 0) {
        return VALONA;
    } else if (strcmp(locationCode, "VR") == 0) {
        return VARNA;
    } else if (strcmp(locationCode, "VE") == 0) {
        return VENICE;
    } else if (strcmp(locationCode, "VI") == 0) {
        return VIENNA;
    } else if (strcmp(locationCode, "ZA") == 0) {
        return ZAGREB;
    } else if (strcmp(locationCode, "ZU") == 0) {
        return ZURICH;
     

    //==== SEAS ====
    } else if (strcmp(locationCode, "NS") == 0) {
        return NORTH_SEA;
    } else if (strcmp(locationCode, "EC") == 0) {
        return ENGLISH_CHANNEL;
    } else if (strcmp(locationCode, "IS") == 0) {
        return IRISH_SEA;
    } else if (strcmp(locationCode, "AO") == 0) {
        return ATLANTIC_OCEAN;
    } else if (strcmp(locationCode, "BB") == 0) {
        return BAY_OF_BISCAY;
    } else if (strcmp(locationCode, "MS") == 0) {
        return MEDITERRANEAN_SEA;
    } else if (strcmp(locationCode, "TS") == 0) {
        return TYRRHENIAN_SEA;
    } else if (strcmp(locationCode, "IO") == 0) {
        return IONIAN_SEA;
    } else if (strcmp(locationCode, "AS") == 0) {
        return ADRIATIC_SEA;
    } else if (strcmp(locationCode, "BS") == 0) {
        return BLACK_SEA;

    //===== OTHER ====
    } else if (strcmp(locationCode, "C?") == 0) {
     return CITY_UNKNOWN;
    } else if (strcmp(locationCode, "S?") == 0) {
         return SEA_UNKNOWN;
    } else if (strcmp(locationCode, "HI") == 0) {
         return HIDE;
    } else if (strcmp(locationCode, "D1") == 0) {
         return DOUBLE_BACK_1;
    } else if (strcmp(locationCode, "D2") == 0) {
         return DOUBLE_BACK_2;
    } else if (strcmp(locationCode, "D3") == 0) {
         return DOUBLE_BACK_3;
    } else if (strcmp(locationCode, "D4") == 0) {
         return DOUBLE_BACK_4;
    } else if (strcmp(locationCode, "D5") == 0) {
         return DOUBLE_BACK_5;
    } else if (strcmp(locationCode, "TP") == 0) {
         return TELEPORT;

    } else {
        return UNKNOWN_LOCATION;
    }
}