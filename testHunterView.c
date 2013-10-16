#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"

#include "HunterView.h"


int main(int argc, char * argv[]) {
	playerMessage messages[] = {};
	
	printf("Test basic empty initialisation\n");
	printf("new\n");
	HunterView hv = newHunterView("", messages);
	printf("getCurrentPlayer\n");
	assert(getCurrentPlayer(hv) == PLAYER_LORD_GODALMING);
	printf("getRound\n");
	assert(getRound(hv) == 0);
	printf("getHealth\n");
	assert(getHealth(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
	assert(getHealth(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
        //assert(getScore(hv) == GAME_START_SCORE);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
        
	disposeHunterView(hv);
	printf("passed\n");

	return 0;

}

