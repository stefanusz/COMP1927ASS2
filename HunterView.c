#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include <string.h>

#define PLAYLEN 7
#define TRUE 1
#define FALSE 0

#define LAND 0
#define SEA 1
#define RAIL 2

static LocationID translateLocationID(char* locationCode);
static int calculateScore (HunterView currentView);
static int calculateHealth (HunterView currentView, PlayerID player);
static void makeMap(HunterView g);
static void showGraph(HunterView g);

     
typedef struct _node *Node;
typedef struct _playerStruct *playerStruct;
     
typedef struct _node{
  LocationID location;
  Node next;
  int type;
}node;     
     
typedef struct _playerStruct {
    int health;
    int numDied;
}ps;     

struct hunterView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int score;
    
    // stores the total amount of turns that have occured within the game
    int totalTurns; 
    // all the pastPlays(PP) seperated, so that they can be individually accessed through the array
    char** seperatedPP;
    playerStruct playerStruct[NUM_PLAYERS];

    // if died[player] then the player has died within that turn
    int died[NUM_PLAYERS];

    Node  trail[NUM_PLAYERS]; 
    Node  connections[NUM_MAP_LOCATIONS]; 
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


    // intialise all values to 0 (false)
    for (i = 0; i < NUM_PLAYERS; i++) {
        // MALLOC FOR PLAYERSTRUCT
        hunterView->playerStruct[i] = malloc(sizeof(struct _playerStruct));
        
        hunterView->died[i] = FALSE;

        hunterView->playerStruct[i]->health = calculateHealth(hunterView, i);
        // initialise died to 0
        hunterView->playerStruct[i]->numDied = 0;
        
    }
    
    // store latest score into struct
        
    hunterView->score = calculateScore(hunterView);
       

        makeMap(hunterView);
        showGraph(hunterView);
        
       
    
    return hunterView;
}


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
    return currentView->score;    
}


static int calculateScore (HunterView currentView) {
    int score;
    
    score = GAME_START_SCORE - currentView->totalTurns/5;
   
    //FOR PLAYERS THAT HAVE DIED.
    
    int i;
    int totalDied = 0;
    
    for(i=0; i<NUM_PLAYERS; i++){
        
        totalDied += currentView -> playerStruct[i] -> numDied;
    }
    totalDied *= SCORE_LOSS_HUNTER_HOSPITAL;
    score = score - totalDied;
    for (i=0; i<getRound(currentView); i++) {
        if (currentView->seperatedPP[(i*NUM_PLAYERS)+PLAYER_DRACULA][5]=='V') {
            score -= SCORE_LOSS_VAMPIRE_MATURES;
        }
    }
    return score;
}

//  ####   ######   #####  #    #  ######    ##    #        #####  #    #
// #    #  #          #    #    #  #        #  #   #          #    #    #
// #       #####      #    ######  #####   #    #  #          #    ######
// #  ###  #          #    #    #  #       ######  #          #    #    #
// #    #  #          #    #    #  #       #    #  #          #    #    #
//  ####   ######     #    #    #  ######  #    #  ######     #    #    #

//Get the current health points for a given player
// 'player' specifies which players's life/blood points to return
//    and must be a value in the interval [0...4] (see 'player' type)
int getHealth(HunterView currentView, PlayerID player) {
    return currentView->playerStruct[player]->health;
}

int calculateHealth(HunterView currentView, PlayerID player) {

    int i,k;
    int health;
    char encounter;
    char location[3];
    char pastLocation[3] = "";
    location[2] = '\0';

    Round roundsPlayed = getRound(currentView);
    if (currentView->totalTurns % 5 > player)
        roundsPlayed++;

    // ================================================ 
    //                   HUNTERS
    // ================================================
    if (player >= 0 && player <= 3) {
        health = GAME_START_HUNTER_LIFE_POINTS;

        // loop through all the plays for the specified player
        for (i = 0; i < roundsPlayed; i++) {

            // store the previous location of the hunter
            if (roundsPlayed >= 2)
                strcpy(pastLocation, location);

            location[0] = currentView->seperatedPP[(i*5)+player][1];
            location[1] = currentView->seperatedPP[(i*5)+player][2];


            // if the hunter hasn't moved, health += 3
            if (strcmp(location, pastLocation) == 0) {
                // hunters can't exceed 9 health
                if (health <= 6) {
                    health += 3;
                } else {
                    health = GAME_START_HUNTER_LIFE_POINTS;
                }
            }

            // loop through all four characters  at the end of the playstring
            for (k = 3; k <= 6; k++) {
                encounter = currentView->seperatedPP[(i*5)+player][k];
                if (encounter == 'T')
                    health -= LIFE_LOSS_TRAP_ENCOUNTER;
                if (encounter == 'D')
                    health -= LIFE_LOSS_DRACULA_ENCOUNTER;
            }
            // check if hunter is in the hospital and reset hp if true
            if (health < 1) {
                currentView->died[player] = TRUE;
                currentView->playerStruct[player]->numDied++;
                health = GAME_START_HUNTER_LIFE_POINTS;
            } else {
                currentView->died[player] = FALSE;
            }
        }
    

    // ================================================
    //                   DRACULA
    // ================================================
    } else {//(player == PLAYER_DRACULA) {
        health = GAME_START_BLOOD_POINTS;

        // loop through all the plays for the specified player
        for (i = 0; i < roundsPlayed; i++) {

            // store the location
            location[0] = currentView->seperatedPP[(i*5)+player][1];
            location[1] = currentView->seperatedPP[(i*5)+player][2];

            LocationID locDracula = translateLocationID(location);

            // handle doubleback
            if (locDracula >= 74 && locDracula <= 78) {
                LocationID history[TRAIL_SIZE];
                getHistory(currentView, PLAYER_DRACULA, history);
                locDracula = history[location[1] - '0'];
            }

            // Reduce health by 10 if a hunter was encountered
            if (locDracula >= 0 && locDracula <= 70 && locDracula != CASTLE_DRACULA) {
                health -= LIFE_LOSS_HUNTER_ENCOUNTER;
            }

            // Reduce health by 2 if at sea
            if ((locDracula >= 61 && locDracula <= 70) 
            || (locDracula == 72)) {
                health -= LIFE_LOSS_SEA;
            }

            // Increase health by 10 if Dracula is at castle
            if (locDracula == CASTLE_DRACULA) {
                health += LIFE_GAIN_CASTLE_DRACULA;
            }
        }
    }
    return health;

}

// #####                          ######
//#     #  #    #  #####   #####  #     #  #         ##     #   #   #####
//#        #    #  #    #  #    # #     #  #        #  #     # #    #    #
//#        #    #  #    #  #    # ######   #       #    #     #     #    #
//#        #    #  #####   #####  #        #       ######     #     #####
//#     #  #    #  #   #   #   #  #        #       #    #     #     #   #
// #####    ####   #    #  #    # #        ######  #    #     #     #    #

//Get the id of current player - ie whose turn is it?
// Only returns a 'playerID' which is one of:
//   LORD_GODALMING (0): Lord Godalming's turn
//   DR_SEWARD      (1): Dr. Seward's turn
//   VAN_HELSING    (2): Van Helsing's turn
//   MINA_HARKER    (3): Mina Harker's turn
//   DRACULA        (4): Dracula's turn
PlayerID getCurrentPlayer (HunterView currentView){
    if (currentView->totalTurns == 0)
        return PLAYER_LORD_GODALMING;

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

// #####    #    ####   #####    ####    ####   ######
// #    #   #   #       #    #  #    #  #       #
// #    #   #    ####   #    #  #    #   ####   #####
// #    #   #        #  #####   #    #       #  #
// #    #   #   #    #  #       #    #  #    #  #
// #####    #    ####   #        ####    ####   ######
     
// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView( HunterView toBeDeleted ) {
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}



//CREATE MAP

static Node newPlace(LocationID place, int connectionType){
  Node newNode = malloc(sizeof(struct _node));
  assert(newNode != NULL);
  newNode->type = connectionType;
  newNode->location = place;
  newNode->next = NULL;

  return newNode;
}

static void addLink(HunterView currentView, LocationID start, LocationID end, int type){
  Node startNode = newPlace(start, type);
  Node endNode = newPlace(end, type);
  
  //updating list for start node
  endNode->next = currentView->connections[start];
  currentView->connections[start] = endNode;      
  
  //updating list for end node
  startNode->next = currentView->connections[end];
  currentView->connections[end] = startNode;      
      
}


static void makeMap(HunterView g){

    addLink(g, ALICANTE, BARCELONA, RAIL );
    addLink(g, ALICANTE, MEDITERRANEAN_SEA, SEA);
    addLink(g, AMSTERDAM, NORTH_SEA, SEA);
    addLink(g, ATHENS, IONIAN_SEA, SEA );

    addLink(g, BARCELONA, MEDITERRANEAN_SEA, SEA );
    addLink(g, BARCELONA, TOULOUSE, LAND );
    addLink(g, BARI, ADRIATIC_SEA, SEA );
    addLink(g, BAY_OF_BISCAY, ATLANTIC_OCEAN, SEA);
    addLink(g, BELGRADE, BUCHAREST, LAND );    
    addLink(g, BELGRADE, KLAUSENBURG, LAND );
    addLink(g, BERLIN, LEIPZIG, LAND );
    addLink(g, BERLIN, LEIPZIG, RAIL );
    addLink(g, BERLIN, PRAGUE, LAND );
    addLink(g, BERLIN, PRAGUE, RAIL );
    addLink(g, BORDEAUX, BAY_OF_BISCAY, SEA);
    addLink(g, BORDEAUX, CLERMONT_FERRAND, LAND );
    addLink(g, BORDEAUX, NANTES, LAND );
    addLink(g, BORDEAUX, TOULOUSE, LAND );
    addLink(g, BUCHAREST, SOFIA, LAND );
    addLink(g, BUDAPEST, KLAUSENBURG, LAND );
    addLink(g, BUDAPEST, SZEGED, LAND );
    addLink(g, BUDAPEST, SZEGED, RAIL );
    addLink(g, BUDAPEST, ZAGREB, LAND );
    addLink(g, CADIZ, ATLANTIC_OCEAN, SEA );
    addLink(g, CADIZ, GRANADA, LAND );
    addLink(g, CAGLIARI, MEDITERRANEAN_SEA, SEA );
    addLink(g, CAGLIARI, TYRRHENIAN_SEA, SEA );
    addLink(g, CASTLE_DRACULA, GALATZ, LAND );
    addLink(g, CLERMONT_FERRAND, GENEVA, LAND );
    addLink(g, CLERMONT_FERRAND, MARSEILLES, LAND );
    addLink(g, CLERMONT_FERRAND, TOULOUSE, LAND );
    addLink(g, COLOGNE, AMSTERDAM, LAND );
    addLink(g, COLOGNE, BRUSSELS, LAND );
    addLink(g, COLOGNE, BRUSSELS, RAIL );
    addLink(g, COLOGNE, FRANKFURT, LAND );
    addLink(g, COLOGNE, FRANKFURT, RAIL );
    addLink(g, COLOGNE, HAMBURG, LAND );
    addLink(g, COLOGNE, LEIPZIG, LAND );
    addLink(g, COLOGNE, STRASBOURG, LAND );
    addLink(g, CONSTANTA, BLACK_SEA, SEA );
    addLink(g, CONSTANTA, BUCHAREST, LAND );
    addLink(g, CONSTANTA, BUCHAREST, RAIL );
    addLink(g, CONSTANTA, VARNA, LAND );
    addLink(g, DUBLIN, IRISH_SEA, SEA);
    addLink(g, EDINBURGH, NORTH_SEA, SEA);
    addLink(g, FLORENCE, ROME, LAND );
    addLink(g, FLORENCE, ROME, RAIL);
    addLink(g, FRANKFURT, LEIPZIG, LAND );
    addLink(g, FRANKFURT, LEIPZIG, RAIL );
    addLink(g, FRANKFURT, NUREMBURG, LAND );
    addLink(g, GALATZ, BUCHAREST, LAND );
    addLink(g, GALATZ, BUCHAREST, RAIL );
    addLink(g, GALATZ, CONSTANTA, LAND );
    addLink(g, GALWAY, ATLANTIC_OCEAN, SEA);
    addLink(g, GALWAY, DUBLIN, LAND);
    addLink(g, GENEVA, MARSEILLES, LAND );
    addLink(g, GENEVA, MILAN, RAIL );
    addLink(g, GENEVA, ZURICH, LAND );
    addLink(g, GENOA, FLORENCE, LAND );
    addLink(g, GENOA, MARSEILLES, LAND );
    addLink(g, GENOA, TYRRHENIAN_SEA, SEA );
    addLink(g, GENOA, VENICE, LAND );
    addLink(g, GRANADA, ALICANTE, LAND );
    addLink(g, HAMBURG, BERLIN, LAND );
    addLink(g, HAMBURG, BERLIN, RAIL );
    addLink(g, HAMBURG, LEIPZIG, LAND );
    addLink(g, HAMBURG, NORTH_SEA, SEA);
    addLink(g, IONIAN_SEA, ADRIATIC_SEA, SEA);
    addLink(g, IONIAN_SEA, BLACK_SEA, SEA);
    addLink(g, KLAUSENBURG, BUCHAREST, LAND );
    addLink(g, KLAUSENBURG, CASTLE_DRACULA, LAND );
    addLink(g, KLAUSENBURG, GALATZ, LAND );
    addLink(g, LEIPZIG, NUREMBURG, LAND );
    addLink(g, LEIPZIG, NUREMBURG, RAIL );
    addLink(g, LE_HAVRE, BRUSSELS, LAND );
    addLink(g, LE_HAVRE, ENGLISH_CHANNEL, SEA);
    addLink(g, LISBON, ATLANTIC_OCEAN, SEA );
    addLink(g, LISBON, CADIZ, LAND );
    addLink(g, LIVERPOOL, IRISH_SEA, SEA);
    addLink(g, LIVERPOOL, SWANSEA, LAND );
    addLink(g, LONDON, ENGLISH_CHANNEL, SEA);
    addLink(g, LONDON, MANCHESTER, LAND );
    addLink(g, LONDON, MANCHESTER, RAIL );
    addLink(g, LONDON, PLYMOUTH, LAND );
    addLink(g, LONDON, SWANSEA, LAND );
    addLink(g, LONDON, SWANSEA, RAIL);
    addLink(g, MADRID, ALICANTE, LAND );
    addLink(g, MADRID, ALICANTE, RAIL );
    addLink(g, MADRID, CADIZ, LAND );
    addLink(g, MADRID, GRANADA, LAND );
    addLink(g, MADRID, LISBON, LAND );
    addLink(g, MADRID, LISBON, RAIL );
    addLink(g, MADRID, SANTANDER, LAND );
    addLink(g, MADRID, SANTANDER, RAIL );
    addLink(g, MADRID, SARAGOSSA, LAND );
    addLink(g, MADRID, SARAGOSSA, RAIL );
    addLink(g, MANCHESTER, EDINBURGH, LAND);
    addLink(g, MANCHESTER, EDINBURGH, RAIL);
    addLink(g, MANCHESTER, LIVERPOOL, LAND);
    addLink(g, MANCHESTER, LIVERPOOL, RAIL);
    addLink(g, MARSEILLES, MEDITERRANEAN_SEA, SEA );
    addLink(g, MARSEILLES, ZURICH, LAND );
    addLink(g, MEDITERRANEAN_SEA, TYRRHENIAN_SEA, SEA );
    addLink(g, MILAN, FLORENCE, RAIL );
    addLink(g, MILAN, GENOA, LAND );
    addLink(g, MILAN, MARSEILLES, LAND );
    addLink(g, MILAN, VENICE, LAND );
    addLink(g, MILAN, ZURICH, LAND );
    addLink(g, MILAN, ZURICH, RAIL );
    addLink(g, MUNICH, MILAN, LAND );
    addLink(g, MUNICH, VENICE, LAND );
    addLink(g, MUNICH, VIENNA, LAND );
    addLink(g, MUNICH, ZAGREB, LAND );
    addLink(g, NANTES, BAY_OF_BISCAY, SEA);
    addLink(g, NANTES, CLERMONT_FERRAND, LAND );
    addLink(g, NANTES, LE_HAVRE, LAND );
    addLink(g, NANTES, PARIS, LAND );
    addLink(g, NAPLES, BARI, LAND );
    addLink(g, NAPLES, BARI, RAIL );
    addLink(g, NAPLES, TYRRHENIAN_SEA, SEA);
    addLink(g, NORTH_SEA, ENGLISH_CHANNEL, SEA);
    addLink(g, NUREMBURG, MUNICH, LAND );
    addLink(g, NUREMBURG, MUNICH, RAIL );
    addLink(g, NUREMBURG, PRAGUE, LAND );
    addLink(g, PARIS, BORDEAUX, RAIL );
    addLink(g, PARIS, BRUSSELS, LAND );
    addLink(g, PARIS, BRUSSELS, RAIL );
    addLink(g, PARIS, CLERMONT_FERRAND, LAND );
    addLink(g, PARIS, GENEVA, LAND );
    addLink(g, PARIS, LE_HAVRE, LAND );
    addLink(g, PARIS, LE_HAVRE, RAIL );
    addLink(g, PARIS, MARSEILLES, RAIL );
    addLink(g, PLYMOUTH, ENGLISH_CHANNEL, SEA);
    addLink(g, PRAGUE, VIENNA, LAND );
    addLink(g, PRAGUE, VIENNA, RAIL );
    addLink(g, ROME, BARI, LAND );
    addLink(g, ROME, NAPLES, LAND );
    addLink(g, ROME, NAPLES, RAIL );
    addLink(g, ROME, TYRRHENIAN_SEA, SEA );
    addLink(g, SALONICA, IONIAN_SEA, SEA );
    addLink(g, SANTANDER, BAY_OF_BISCAY, SEA);
    addLink(g, SANTANDER, LISBON, LAND );
    addLink(g, SANTANDER, SARAGOSSA, LAND );
    addLink(g, SARAGOSSA, ALICANTE, LAND );
    addLink(g, SARAGOSSA, ALICANTE, RAIL );
    addLink(g, SARAGOSSA, BARCELONA, LAND );
    addLink(g, SARAGOSSA, BARCELONA, RAIL );
    addLink(g, SARAGOSSA, BORDEAUX, LAND );
    addLink(g, SARAGOSSA, BORDEAUX, RAIL );
    addLink(g, SARAGOSSA, TOULOUSE, LAND );
    addLink(g, SARAJEVO, BELGRADE, LAND );
    addLink(g, SARAJEVO, SOFIA, LAND );
    addLink(g, SARAJEVO, SOFIA, RAIL );
    addLink(g, SARAJEVO, VALONA, LAND );
    addLink(g, SOFIA, SALONICA, LAND );
    addLink(g, SOFIA, SALONICA, RAIL );
    addLink(g, SOFIA, VARNA, LAND );
    addLink(g, SOFIA, VARNA, RAIL );
    addLink(g, STRASBOURG, BRUSSELS, LAND );
    addLink(g, STRASBOURG, FRANKFURT, LAND );
    addLink(g, STRASBOURG, FRANKFURT, RAIL );
    addLink(g, STRASBOURG, GENEVA, LAND );
    addLink(g, STRASBOURG, MUNICH, LAND ); 
    addLink(g, STRASBOURG, NUREMBURG, LAND );
    addLink(g, STRASBOURG, PARIS, LAND );
    addLink(g, STRASBOURG, ZURICH, LAND );
    addLink(g, STRASBOURG, ZURICH, RAIL );
    addLink(g, ST_JOSEPH_AND_ST_MARYS, BELGRADE, LAND);
    addLink(g, ST_JOSEPH_AND_ST_MARYS, SARAJEVO, LAND );
    addLink(g, SZEGED, BELGRADE, LAND );
    addLink(g, SZEGED, BELGRADE, RAIL );
    addLink(g, SZEGED, BUCHAREST, RAIL );
    addLink(g, SZEGED, KLAUSENBURG, LAND );
    addLink(g, SZEGED, ST_JOSEPH_AND_ST_MARYS, LAND );
    addLink(g, TOULOUSE, MARSEILLES, LAND );
    addLink(g, TYRRHENIAN_SEA, IONIAN_SEA, SEA);
    addLink(g, VALONA, ATHENS, LAND);
    addLink(g, VALONA, IONIAN_SEA, SEA );
    addLink(g, VALONA, SALONICA, LAND);
    addLink(g, VALONA, SOFIA, LAND );
    addLink(g, VARNA, BLACK_SEA, SEA );
    addLink(g, VENICE, ADRIATIC_SEA, SEA );
    addLink(g, VENICE, FLORENCE, LAND );
    addLink(g, VIENNA, BUDAPEST, LAND );
    addLink(g, VIENNA, BUDAPEST, RAIL );
    addLink(g, VIENNA, VENICE, RAIL );
    addLink(g, VIENNA, ZAGREB, LAND );
    addLink(g, ZAGREB, SARAJEVO, LAND );
    addLink(g, ZAGREB, ST_JOSEPH_AND_ST_MARYS, LAND );
    addLink(g, ZAGREB, SZEGED, LAND );
    addLink(g, ZURICH, MUNICH, LAND );
   

}

/* FOR DEBUGGING PURPOSES.
  ####   #    #   ####   #    #          #    #    ##    #####
 #       #    #  #    #  #    #          ##  ##   #  #   #    #
  ####   ######  #    #  #    #          # ## #  #    #  #    #
      #  #    #  #    #  # ## #          #    #  ######  #####
 #    #  #    #  #    #  ##  ##          #    #  #    #  #
  ####   #    #   ####   #    # #######  #    #  #    #  #
*/

 void showGraph(HunterView g) { 
    assert(g != NULL); 
    
    int i; 
    printf("TOP PAGE"); 
    for (i = 0; i < 70; i++) { 
            Node n = g->connections[i]; 
            while (n != NULL) { 
                printf("%d-%d->",i,n->location); 
                //printf("TYPE IS %d\n",n->type); 
                if(n->type == LAND){
                    printf("LAND");
                } else if(n->type == SEA){
                    printf("SEA");
                } else if(n->type == RAIL){
                    printf("RAIL");
                    
                } else {
                    exit(0);
                }

                printf("success number %d\n", i );
                n = n->next; 
            } 

        if (g->connections [i] != NULL) {
            printf("\n"); 
        } 

           
    }
} 

/*
  ####    ####   #    #  #    #  ######   ####    #####  ######  #####
 #    #  #    #  ##   #  ##   #  #       #    #     #    #       #    #
 #       #    #  # #  #  # #  #  #####   #          #    #####   #    #
 #       #    #  #  # #  #  # #  #       #          #    #       #    #
 #    #  #    #  #   ##  #   ##  #       #    #     #    #       #    #
  ####    ####   #    #  #    #  ######   ####      #    ######  #####


//Functions that query the map to find information about connectivity

//This function returns an array of LocationID that represent all locations that are connected 
//to the given LocationID. 
//road, rail and sea are connections should only be considered if the road, rail, sea parameters 
//are TRUE.
//The size of the array should be stored in the variable pointed to by numLocations
//The array can be in any order but must contain unique entries
//Your function must take into account the round and player id for rail travel
//Your function must take into account that dracula can't move to the hospital or travel by rail
//but need not take into account draculas trail
//Any location that the player is currently in, should be included.
LocationID * connectedLocations(HunterView currentView, int * numLocations, LocationID from, 
                                PlayerID player, Round round, int road, int rail, int sea){

  LocationID *array = FALSE; 
  return array;
} */



//   ####   ######   #####  #####    ####   #    #  #    #  #####
//  #    #  #          #    #    #  #    #  #    #  ##   #  #    #
//  #       #####      #    #    #  #    #  #    #  # #  #  #    #
//  #  ###  #          #    #####   #    #  #    #  #  # #  #    #
//  #    #  #          #    #   #   #    #  #    #  #   ##  #    #
//   ####   ######     #    #    #   ####    ####   #    #  #####
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

    // check whether player has died within the turn, set location if died
    if (currentView->died[player] == TRUE)
        return ST_JOSEPH_AND_ST_MARYS;

    Round roundsPlayed = getRound(currentView);
    if (currentView->totalTurns % 5 > player)
        roundsPlayed++;

    // check that a move has been made otherwise return UNKNOWN_LOCATION
    if (roundsPlayed > 0) { 
        char location[3];
        location[2] = '\0';


        location[0] = currentView->seperatedPP[((roundsPlayed-1)*5)+player][1];
        location[1] = currentView->seperatedPP[((roundsPlayed-1)*5)+player][2];

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



//#####  #####     ##    #    #   ####   #         ##     #####  ######
//  #    #    #   #  #   ##   #  #       #        #  #      #    #
//  #    #    #  #    #  # #  #   ####   #       #    #     #    #####
//  #    #####   ######  #  # #       #  #       ######     #    #
//  #    #   #   #    #  #   ##  #    #  #       #    #     #    #
//  #    #    #  #    #  #    #   ####   ######  #    #     #    ######

static LocationID translateLocationID (char* locationCode) {
    // ==== Cities ====
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
