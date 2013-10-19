#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static char* encodeLocationID (LocationID locationCode);

void decideMove (HunterView gameState) {


    // array to store current location of each player
    LocationID currLoc[4];
    srand (time(NULL));
    
    /* Initial moves
        Hunter 0 moves to city Galway
        Hunter 1 moves to city Galatz
        Hunter 2 moves to city Athens
        Hunter 3 moves to city Madrid*/
    if ( getRound(gameState) == 0) {
        if (getCurrentPlayer(gameState) == 0) 
            registerBestPlay("GW", "I Wanna Stake You Dracula <3" );
        if (getCurrentPlayer(gameState) == 1)
            registerBestPlay("GA", "I Wanna Stake You Dracula <3" );
        if (getCurrentPlayer(gameState) == 2)
            registerBestPlay("AT", "I Wanna Stake You Dracula <3" );
        if (getCurrentPlayer(gameState) == 3)
            registerBestPlay("MA", "I Wanna Stake You Dracula <3" );

    } else {
        // stay in the same location
        if (getCurrentPlayer(gameState) == PLAYER_LORD_GODALMING) {
            //currLoc[PLAYER_LORD_GODALMING] = getCurrentLocation(gameState, PLAYER_LORD_GODALMING);
            registerBestPlay("GW", "I Wanna Stake You Dracula <3" );
        }

        // Hunter 1 will always move to Klausenburg if he is in Galatz of Galatz if he is klausenburg. Otherwise he will stay where he was last turn.
        if (getCurrentPlayer(gameState) == PLAYER_DR_SEWARD) {

            currLoc[PLAYER_DR_SEWARD] = getLocation(gameState, PLAYER_DR_SEWARD);
            if (currLoc[PLAYER_DR_SEWARD] == GALATZ) {
                registerBestPlay("KL", "blah");
            } else if (currLoc[PLAYER_DR_SEWARD] == KLAUSENBURG) {
                registerBestPlay("GA", "blah");
            } else {
                registerBestPlay(encodeLocationID(currLoc[PLAYER_DR_SEWARD]), "blah");
            }
        }

        // Hunter 2 will always pick a random city adjacent by LAND
        if (getCurrentPlayer(gameState) == PLAYER_VAN_HELSING ) {

            currLoc[PLAYER_VAN_HELSING] = getLocation(gameState, PLAYER_VAN_HELSING);
            int numAdjLandH2;

            LocationID * arrAdjLandH2 = connectedLocations(gameState, &numAdjLandH2, currLoc[PLAYER_VAN_HELSING] ,PLAYER_VAN_HELSING,getRound(gameState),1,1,0);

            registerBestPlay(encodeLocationID(arrAdjLandH2[rand() % numAdjLandH2]), "I Wanna Stake You Dracula <3" );

        }

        // Hunter 3 will always pick a random city adjacent by LAND or SEA
        if (getCurrentPlayer(gameState) == PLAYER_MINA_HARKER) {

            currLoc[PLAYER_MINA_HARKER] = getLocation(gameState, PLAYER_MINA_HARKER);
            int numAdjLandH3;

            LocationID * arrAdjLandH3 = connectedLocations(gameState, &numAdjLandH3, currLoc[PLAYER_MINA_HARKER] ,PLAYER_MINA_HARKER,getRound(gameState),1,1,1);

            registerBestPlay(encodeLocationID(arrAdjLandH3[rand() % numAdjLandH3]), "I Wanna Stake You Dracula <3" );

        }
    }
 
}


char* arrLocationCodes2[NUM_LOCATIONS] = {
    // Cities
    "AL", "AM", "AT", "BA", "BI", "BE", "BR", "BO", "BU", "BC",
    "BD", "CA", "CG", "CD", "CF", "CO", "CN", "DU", "ED", "FL",
    "FR", "GA", "GW", "GE", "GO", "GR", "HA", "JM", "KL", "LE",
    "LI", "LS", "LV", "LO", "MA", "MN", "MR", "MI", "MU", "NA",
    "NP", "NU", "PA", "PL", "PR", "RO", "SA", "SN", "SR", "SJ",
    "SO", "ST", "SW", "SZ", "TO", "VA", "VR", "VE", "VI", "ZA",
    "ZU",
    // Seas
    "NS", "EC", "IS", "AO", "BB", "MS", "TS", "IO", "AS", "BS",
    // Other
    "C?", "S?", "HI", "D1", "D2", "D3", "D4", "D5", "TP"};

static char* encodeLocationID (LocationID locationCode) {
    return arrLocationCodes2[locationCode];
}