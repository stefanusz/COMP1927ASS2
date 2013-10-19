#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void decideMove (HunterView gameState) {

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
    } else {
        if (getCurrentPlayer(gameState) == 0) {
            registerBestPlay("GW", "I Wanna Stake You Dracula <3" );
        }

        if (getCurrentPlayer(gameState) == 1) {
            registerBestPlay("GA", "I Wanna Stake You Dracula <3" );
        }

        if (getCurrentPlayer(gameState) == 2) {
            registerBestPlay("AT", "I Wanna Stake You Dracula <3" );
        }
        
        if (getCurrentPlayer(gameState) == 3) {
            registerBestPlay("MA", "I Wanna Stake You Dracula <3" );
        }
    }
 
}

