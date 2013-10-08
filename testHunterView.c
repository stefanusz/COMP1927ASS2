#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"


int main(int argc, char * argv[]) {
	playerMessage messages[] = {};
	
	printf("Test basic empty initialisation\n");
	HunterView hv = newHunterView("", messages);
	assert(getCurrentPlayer(hv) == PLAYER_LORD_GODALMING);
	assert(getRound(hv) == 0);
	assert(getHealth(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
	assert(getHealth(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
        assert(getScore(hv) == GAME_START_SCORE);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
        
	disposeHunterView(hv);
	printf("passed\n");

	return 0;
}

