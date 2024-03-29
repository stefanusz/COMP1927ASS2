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
#define ANY 1588

static LocationID translateLocationID(char* locationCode);
static int calculateScore (HunterView currentView);
static int calculateHealth (HunterView currentView, PlayerID player);
static void makeMap(HunterView g);
void showGraph(HunterView g);
void destroyGraph(HunterView g);
void canReachInN(HunterView g, LocationID start, int type, int n, int locs[]);

     
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
        return PLAYER_DRACULA
;    } else {
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
    int i;

    //showGraph(toBeDeleted);

    // Free separtatedPP
    for(i = 0; i <  toBeDeleted->totalTurns; i++) {
        free(toBeDeleted->seperatedPP[i]);
    }
    free(toBeDeleted->seperatedPP);

    // Free Player struct
    for (i = 0; i < NUM_PLAYERS; i++) {
        free(toBeDeleted->playerStruct[i]);
    }    
    //Free
    // FREE ALL MALLOC
    //destroyGraph(toBeDeleted);
    // Free HunterView struct
    free(toBeDeleted);
}


//CREATE MAP
static Node newNode(LocationID place, int connectionType){
  Node newNode = malloc(sizeof(struct _node));
  assert(newNode != NULL);
  newNode->type = connectionType;
  newNode->location = place;
  newNode->next = NULL;

  return newNode;
}

static void addLink(HunterView g, LocationID start, LocationID end, int type){
/*  Node startNode = newPlace(start, type);
  Node endNode = newPlace(end, type);
  
  //updating list for start node
  endNode->next = currentView->connections[start];
  currentView->connections[start] = endNode;      
  
  //updating list for end node
  startNode->next = currentView->connections[end];
  currentView->connections[end] = startNode;   */

    Node s = newNode (start, type);
    Node e = newNode (end, type);
    Node oldS = g->connections[start];
    Node oldE = g->connections[end];

    g->connections[start] = e;
    e->next = oldS;
    g->connections[end] = s;
    s->next = oldE;   
      
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

void freeList(Node start){
    Node temp;
    printf("%p ", start);
    //int city = start->location;

    while (start != NULL){

        temp = start;
        printf("JBARJSAJD = %d\n", temp->location);
        start = start->next;
        free(temp);

    }

    //printf("Freed city %d\n", city);
}

/* FOR DEBUGGING PURPOSES.
  ####   #    #   ####   #    #          #    #    ##    #####
 #       #    #  #    #  #    #          ##  ##   #  #   #    #
  ####   ######  #    #  #    #          # ## #  #    #  #    #
      #  #    #  #    #  # ## #          #    #  ######  #####
 #    #  #    #  #    #  ##  ##          #    #  #    #  #
  ####   #    #   ####   #    # #######  #    #  #    #  #
*/
/*void showGraph(HunterView g) { 
    assert(g != NULL); 
    
    int i; 
    printf("TOP PAGE"); 
    for (i = 1; i < 70; i++) { 
            Node n = g->connections[i]; 
            //Node t;
            while (n != NULL) { 
                //printf("PASS BY HERE %d\n", i);
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

                //printf("success number %d", i );
                 if (i==0) break;
                t = n;
                //free(t);
                n = t->next; 
               
            } 
            freeList(n);
             //free(t);
        if (g->connections [i] != NULL) {
            //printf("\n"); 
        } 

           
    }
}*/

 void showGraph(HunterView g) { 
    assert(g != NULL); 
    
    int i; 
    printf("TOP PAGE"); 
    for (i = 0; i < 71; i++) { 
            printf("[###]->");
            Node n = g->connections[i]; 
            Node h = n;
            while (n != NULL) { 
                printf("%d-%d",i,n->location); 
                printf("n that segfails is %p\n", n);
                //printf("TYPE IS %d\n",n->type); 
                if(n->type == LAND){
                    printf("L");
                } else if(n->type == SEA){
                    printf("S");
                } else if(n->type == RAIL){
                    printf("R");
                    
                } else {
                    exit(0);
                }
                printf("->");

                //printf("success number %d", i );
                n = n->next; 
            }
            if (n == NULL) 
                printf("NULL");
 
       /* if (g->connections [i] != NULL) {
            printf("\n"); 
        }*/
        printf("h is %p\n", h);
        freeList(h); 

           
    }
} 


/*
  ####    ####   #    #  #    #  ######   ####    #####  ######  #####
 #    #  #    #  ##   #  ##   #  #       #    #     #    #       #    #
 #       #    #  # #  #  # #  #  #####   #          #    #####   #    #
 #       #    #  #  # #  #  # #  #       #          #    #       #    #
 #    #  #    #  #   ##  #   ##  #       #    #     #    #       #    #
  ####    ####   #    #  #    #  ######   ####      #    ######  #####

*/
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
    // TRUE 1
    // FALSE 0
    // TAKEN FROM GRAPH.C WEEK 9.
    // GET WHERE CURRENT PLAYER IS. 
    LocationID *result = malloc(sizeof(int)*NUM_MAP_LOCATIONS);

    Node current = currentView->connections[from];
    current = current->next;
    
    result[0]=from;
    int checker;
    int counter;
    // TO CHECK AND GIVE INDICATION WHETHER ITS ROAD, RAIL OR SEA.
    if(road == TRUE){
        checker = LAND;
    }else if(sea == TRUE){
        checker = SEA;
    }else if(road == TRUE && sea == TRUE){
        checker = ANY;
    }

    counter = 1;

    // MEANS THIS IS HUNTER
    if((road == TRUE && sea == FALSE ) || 
       (road == FALSE && sea == TRUE ) || 
       (road == TRUE && sea == TRUE  ) ) 
    {
        
        //printf ("it comes here\n");
        if(checker == ANY){
            
            while(current != NULL){

                if(current->type != RAIL){


                    int arrayChecker;
                    int somethingEqual = FALSE;
                    for(arrayChecker=0; arrayChecker<counter; arrayChecker++){
                        if (result[arrayChecker] == current->location)
                        {
                            somethingEqual = TRUE;
                        }
                    }

                    if (!somethingEqual) {
                        result[counter] = current->location;
                    counter++;
                    }



                    if(player == PLAYER_DRACULA && current->location == ST_JOSEPH_AND_ST_MARYS){
                        // TO SKIP THIS PART. 
                        current = current -> next;
                    }
                    
                }

                current = current -> next;
                
            }

            *numLocations = counter;


        }else{

           
            //printf ("it comes here\n");
            while(current != NULL){

                if (current->type == checker)
                {
                    //printf ("it comes here counter %d\n", counter);
                    // CHECK WHETHER THERE IS ANY DUPLICATE OR NOT.
                    
                    int arrayChecker;
                    int somethingEqual = FALSE;
                    for(arrayChecker=0; arrayChecker<counter; arrayChecker++){
                        if (result[arrayChecker] == current->location)
                        {
                            somethingEqual = TRUE;
                        }
                    }

                    if (!somethingEqual) {
                        result[counter] = current->location;
                        counter++;
                    }
                    
                    if(player == PLAYER_DRACULA && current->location == ST_JOSEPH_AND_ST_MARYS){
                        // TO SKIP THIS PART. 
                        //printf ("it comes here BLA BLA %d\n", counter);
                        current = current -> next;
                    }
                    
                    
                }
                current = current -> next;
            }
            //printf ("COUNTER %d\n", counter);
            *numLocations = counter;


        }
        
        // FOR DEBUGGING PURPOSES.
        
        /*    int counter99;

            for(counter99=0; counter99< NUM_MAP_LOCATIONS; counter99++){
                //printf("number zero is %d\n", result[0]);

                if(result[counter99] != 0){
                    printf("the CITY inside array[%d] result is %d\n", counter99, result[counter99]);
                }
                
            }
            printf("\n");*/
        
    }else if(rail == TRUE && player != PLAYER_DRACULA){

        int railSum;
        int sum;

        sum = round + player;

        railSum = sum % 4;

        printf("railSum is %d\n", railSum );


        
        if(railSum == 0){
            // NO MOVE AVAILABLE
            *numLocations = counter;
            return result;
        }else if(railSum == 1){
            printf("COMES HERE2\n");
            // MOVE ONE AWAY BY RAIL

            while(current != NULL){

                if (current->type == RAIL)
                {
                    int arrayChecker;
                    int somethingEqual = FALSE;
                    for(arrayChecker=0; arrayChecker<counter; arrayChecker++){
                        if (result[arrayChecker] == current->location)
                        {
                            somethingEqual = TRUE;
                        }
                    }

                    if (!somethingEqual) {
                    result[counter] = current->location;
                    counter++;
                    }
                    
                    
                }
            current = current -> next;

            *numLocations = counter;
            }

        }else if(railSum == 2){

            // MOVE UP TO 2 PLACES BY RAIL
           int *num2 = malloc(sizeof(int)*NUM_MAP_LOCATIONS);
           canReachInN(currentView, from, RAIL, 2, num2);
           int cityCounter;

           for(cityCounter=0; cityCounter<NUM_MAP_LOCATIONS; cityCounter++){

                if(num2[cityCounter] == 1){

                    int finalChecker;
                    int somethingEqual;

                    for(finalChecker=0; finalChecker<counter; finalChecker++){
                            if (result[finalChecker] == current->location)
                            {
                                somethingEqual = TRUE;
                            }
                        }

                        if (!somethingEqual) {
                        result[counter] = cityCounter;
                        counter++;
                        }
                }
           }
        

        *numLocations = counter;
            

        }else if(railSum == 3){

            // MOVE UP TO 3 PLACES BY RAIL
           int *num3 = malloc(sizeof(int)*NUM_MAP_LOCATIONS);
           canReachInN(currentView, from, RAIL, 3, num3);
           int cityCounter;

           for(cityCounter=0; cityCounter<NUM_MAP_LOCATIONS; cityCounter++){

                if(num3[cityCounter] == 1){

                    int finalChecker;
                    int somethingEqual;

                    for(finalChecker=0; finalChecker<counter; finalChecker++){
                            if (result[finalChecker] == current->location)
                            {
                                somethingEqual = TRUE;
                            }
                        }

                        if (!somethingEqual) {
                        result[counter] = cityCounter;
                        counter++;
                        }
                }
           }
            

        }
        
        // FOR DEBUGGING PURPOSES.
        int counter99;

            for(counter99=0; counter99< NUM_MAP_LOCATIONS; counter99++){
                //printf("number zero is %d\n", result[0]);

                if(result[counter99] != 0){
                    printf("the CITY inside array[%d] result is %d\n", counter99, result[counter99]);
                }
                
            }
        printf("\n");

  }
    
  
  return result;

} 

/*
  ####     ##    #    #          #####   ######    ##     ####   #    #
 #    #   #  #   ##   #          #    #  #        #  #   #    #  #    #
 #       #    #  # #  #          #    #  #####   #    #  #       ######
 #       ######  #  # #          #####   #       ######  #       #    #
 #    #  #    #  #   ##          #   #   #       #    #  #    #  #    #
  ####   #    #  #    # #######  #    #  ######  #    #   ####   #    #

*/


void canReachInN(HunterView g, LocationID start, int type, int n, int locs[]){
   //IMPLEMENT FOR TASK 3
   
    locs[start] = 1;
    Node curr = g->connections[start];
    while (curr != NULL){
        if (type == curr->type || type == ANY) {
            int m;
            for (m = 0; m < n; m++) {
                canReachInN(g, curr->location, type, m, locs);
            }
        }
        curr = curr->next;
    }
    
}



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

const char arrLocationCodes[NUM_LOCATIONS][3] = {
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


static LocationID translateLocationID (char* locationCode) {
    int i;
    for (i = 0; i < NUM_LOCATIONS; i++) {
        if (strncmp(locationCode, arrLocationCodes[i],2) == 0)
            return i;
    }
    return UNKNOWN_LOCATION;
}


// DESTROY GRAPH.
void destroyGraph(HunterView g){
    int i;
    Node curr;
    Node next;
    assert(g != NULL);
    assert(g->connections != NULL);
  
    for (i = ALICANTE; i <= BLACK_SEA; i++){         
         curr = g->connections[i];
         while(curr!=NULL){
            printf("%p\n I is %d\n", curr, i);
            next = curr->next;

             free(curr);
              
             curr=next;
         }

        // free(curr->next);
    }
    
}   