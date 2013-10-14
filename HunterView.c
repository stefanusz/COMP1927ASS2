#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include <string.h>

#define PLAYLEN 7
     
     
typedef struct _node{
  LocationID location;
  Node next;
  int type;
}node;     
     
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
  
  //updating lsit for end node
  startNode->next = currentView->connections[end];
  currentView->connections[end] = startNode;      
      
}


static void makeMap(HunterView g){

    //RULE BRITTANIA!
    addLink(g, LONDON, SWANSEA, RAIL);
    addLink(g, LONDON, SWANSEA, LAND );
    addLink(g, LONDON, PLYMOUTH, LAND );
    addLink(g, LONDON, MANCHESTER, LAND );
    addLink(g, LONDON, MANCHESTER, RAIL );
    addLink(g, LONDON, ENGLISH_CHANNEL, SEA);
    addLink(g, LIVERPOOL, SWANSEA, LAND );
    addLink(g, LIVERPOOL, IRISH_SEA, SEA);

    addLink(g, MANCHESTER, EDINBURGH, LAND);
    addLink(g, MANCHESTER, EDINBURGH, RAIL);
    addLink(g, MANCHESTER, LIVERPOOL, LAND);
    addLink(g, MANCHESTER, LIVERPOOL, RAIL);
    addLink(g, GALWAY, DUBLIN, LAND);
    addLink(g, GALWAY, ATLANTIC_OCEAN, SEA);

    addLink(g, DUBLIN, IRISH_SEA, SEA);
    addLink(g, PLYMOUTH, ENGLISH_CHANNEL, SEA);
    addLink(g, EDINBURGH, NORTH_SEA, SEA);

    //IST ICH EIN BERLINER!
    addLink(g, STRASBOURG, PARIS, LAND );
    addLink(g, STRASBOURG, BRUSSELS, LAND );
    addLink(g, STRASBOURG, GENEVA, LAND );
    addLink(g, STRASBOURG, ZURICH, RAIL );
    addLink(g, STRASBOURG, ZURICH, LAND );
    addLink(g, STRASBOURG, NUREMBURG, LAND );
    addLink(g, STRASBOURG, MUNICH, LAND ); 
    addLink(g, COLOGNE, AMSTERDAM, LAND );
    addLink(g, COLOGNE, BRUSSELS, LAND );
    addLink(g, COLOGNE, BRUSSELS, RAIL );
    addLink(g, COLOGNE, STRASBOURG, LAND );
    addLink(g, COLOGNE, FRANKFURT, LAND );
    addLink(g, COLOGNE, FRANKFURT, RAIL );
    addLink(g, COLOGNE, HAMBURG, LAND );
    addLink(g, COLOGNE, LEIPZIG, LAND );
    addLink(g, STRASBOURG, FRANKFURT, LAND );
    addLink(g, STRASBOURG, FRANKFURT, RAIL );
    addLink(g, FRANKFURT, NUREMBURG, LAND );
    addLink(g, FRANKFURT, LEIPZIG, LAND );
    addLink(g, FRANKFURT, LEIPZIG, RAIL );
    addLink(g, HAMBURG, LEIPZIG, LAND );
    addLink(g, HAMBURG, BERLIN, RAIL );
    addLink(g, HAMBURG, BERLIN, LAND );
    addLink(g, BERLIN, LEIPZIG, LAND );
    addLink(g, BERLIN, LEIPZIG, RAIL );
    addLink(g, BERLIN, PRAGUE, LAND );
    addLink(g, BERLIN, PRAGUE, RAIL );
    addLink(g, LEIPZIG, NUREMBURG, LAND );
    addLink(g, LEIPZIG, NUREMBURG, RAIL );
    addLink(g, NUREMBURG, MUNICH, LAND );
    addLink(g, NUREMBURG, MUNICH, RAIL );
    addLink(g, ZURICH, MUNICH, LAND );
    addLink(g, MUNICH, MILAN, LAND );
    addLink(g, MUNICH, VENICE, LAND );
    addLink(g, NUREMBURG, PRAGUE, LAND );
    addLink(g, MUNICH, VIENNA, LAND );
    addLink(g, VIENNA, VENICE, RAIL );
    addLink(g, MUNICH, ZAGREB, LAND );


    //OTHER COUNTRIES, AKA, PEASANT FILTH

    //OCEAN LINKS
    addLink(g, ATLANTIC_OCEAN, NORTH_SEA, SEA);
    addLink(g, NORTH_SEA, ENGLISH_CHANNEL, SEA);
    addLink(g, BAY_OF_BISCAY, ATLANTIC_OCEAN, SEA);
    addLink(g, ATLANTIC_OCEAN, MEDITERRANEAN_SEA, SEA);
    addLink(g, ATLANTIC_OCEAN, MEDITERRANEAN_SEA, SEA);
    addLink(g, MEDITERRANEAN_SEA, TYRRHENIAN_SEA, SEA );
    addLink(g, TYRRHENIAN_SEA, IONIAN_SEA, SEA);
    addLink(g, IONIAN_SEA, ADRIATIC_SEA, SEA);
    addLink(g, IONIAN_SEA, BLACK_SEA, SEA);
    addLink(g, LE_HAVRE, ENGLISH_CHANNEL, SEA);
    addLink(g, AMSTERDAM, NORTH_SEA, SEA);
    addLink(g, HAMBURG, NORTH_SEA, SEA);
    addLink(g, NANTES, BAY_OF_BISCAY, SEA);
    addLink(g, BORDEAUX, BAY_OF_BISCAY, SEA);
    addLink(g, MARSEILLES, MEDITERRANEAN_SEA, SEA );
    addLink(g, GENOA, TYRRHENIAN_SEA, SEA );
    addLink(g, ROME, TYRRHENIAN_SEA, SEA );
    addLink(g, NAPLES, TYRRHENIAN_SEA, SEA);
    addLink(g, SANTANDER, BAY_OF_BISCAY, SEA);
    addLink(g, ALICANTE, MEDITERRANEAN_SEA, SEA);
    addLink(g, LISBON, ATLANTIC_OCEAN, SEA );
    addLink(g, CADIZ, ATLANTIC_OCEAN, SEA );
    addLink(g, BARCELONA, MEDITERRANEAN_SEA, SEA );
    addLink(g, BARI, ADRIATIC_SEA, SEA );
    addLink(g, VENICE, ADRIATIC_SEA, SEA );
    addLink(g, VALONA, IONIAN_SEA, SEA );
    addLink(g, ATHENS, IONIAN_SEA, SEA );
    addLink(g, SALONICA, IONIAN_SEA, SEA );
    addLink(g, VARNA, BLACK_SEA, SEA );
    addLink(g, CONSTANTA, BLACK_SEA, SEA );
    addLink(g, CAGLIARI, MEDITERRANEAN_SEA, SEA );
    addLink(g, CAGLIARI, TYRRHENIAN_SEA, SEA );

    //SPANISH SIESTA
    addLink(g, MADRID, SANTANDER, LAND );
    addLink(g, MADRID, SANTANDER, RAIL );
    addLink(g, MADRID, LISBON, LAND );
    addLink(g, MADRID, LISBON, RAIL );
    addLink(g, MADRID, CADIZ, LAND );
    addLink(g, MADRID, GRANADA, LAND );
    addLink(g, LISBON, CADIZ, LAND );
    addLink(g, CADIZ, GRANADA, LAND );
    addLink(g, GRANADA, ALICANTE, LAND );
    addLink(g, MADRID, ALICANTE, LAND );
    addLink(g, MADRID, ALICANTE, RAIL );
    addLink(g, ALICANTE, BARCELONA, RAIL );
    addLink(g, BARCELONA, TOULOUSE, LAND );
    addLink(g, SARAGOSSA, BARCELONA, RAIL );
    addLink(g, SARAGOSSA, BARCELONA, LAND );
    addLink(g, SARAGOSSA, ALICANTE, LAND );
    addLink(g, SARAGOSSA, ALICANTE, RAIL );
    addLink(g, MADRID, SARAGOSSA, LAND );
    addLink(g, MADRID, SARAGOSSA, RAIL );
    addLink(g, SANTANDER, LISBON, LAND );
    addLink(g, SARAGOSSA, BORDEAUX, LAND );

    //FRANCY PANTS
    addLink(g, SANTANDER, SARAGOSSA, LAND );
    addLink(g, SARAGOSSA, BORDEAUX, RAIL );
    addLink(g, SARAGOSSA, TOULOUSE, LAND );
    addLink(g, BORDEAUX, TOULOUSE, LAND );
    addLink(g, CLERMONT_FERRAND, TOULOUSE, LAND );
    addLink(g, BORDEAUX, NANTES, LAND );
    addLink(g, NANTES, LE_HAVRE, LAND );
    addLink(g, NANTES, PARIS, LAND );
    addLink(g, PARIS, BORDEAUX, RAIL );
    addLink(g, BORDEAUX, CLERMONT_FERRAND, LAND );
    addLink(g, NANTES, CLERMONT_FERRAND, LAND );
    addLink(g, PARIS, CLERMONT_FERRAND, LAND );
    addLink(g, CLERMONT_FERRAND, MARSEILLES, LAND );
    addLink(g, PARIS, MARSEILLES, RAIL );
    addLink(g, GENEVA, MARSEILLES, LAND );
    addLink(g, GENOA, MARSEILLES, LAND );
    addLink(g, CLERMONT_FERRAND, GENEVA, LAND );
    addLink(g, PARIS, LE_HAVRE, LAND );
    addLink(g, PARIS, LE_HAVRE, RAIL );
    addLink(g, LE_HAVRE, BRUSSELS, LAND );
    addLink(g, PARIS, BRUSSELS, LAND );
    addLink(g, PARIS, BRUSSELS, RAIL );
    addLink(g, TOULOUSE, MARSEILLES, LAND );
    addLink(g, PARIS, GENEVA, LAND );

    //VIVE ITALIA!
    addLink(g, MILAN, FLORENCE, RAIL );
    addLink(g, MILAN, GENOA, LAND );
    addLink(g, GENOA, FLORENCE, LAND );
    addLink(g, GENOA, VENICE, LAND );
    addLink(g, MILAN, VENICE, LAND );
    addLink(g, FLORENCE, ROME, LAND );
    addLink(g, FLORENCE, ROME, RAIL);
    addLink(g, ROME, BARI, LAND );
    addLink(g, ROME, NAPLES, LAND );
    addLink(g, ROME, NAPLES, RAIL );
    addLink(g, NAPLES, BARI, LAND );
    addLink(g, NAPLES, BARI, RAIL );
    addLink(g, GENEVA, MILAN, RAIL );
    addLink(g, MILAN, ZURICH, LAND );
    addLink(g, MILAN, ZURICH, RAIL );
    addLink(g, MILAN, MARSEILLES, LAND );
    addLink(g, MARSEILLES, ZURICH, LAND );
    addLink(g, GENEVA, ZURICH, LAND );
    addLink(g, VENICE, FLORENCE, LAND );

    //YOU ARE MAKING ME HUNGARY
    addLink(g, VIENNA, ZAGREB, LAND );
    addLink(g, VIENNA, BUDAPEST, LAND );
    addLink(g, VIENNA, BUDAPEST, RAIL );
    addLink(g, BUDAPEST, ZAGREB, LAND );
    addLink(g, BUDAPEST, SZEGED, LAND );
    addLink(g, BUDAPEST, SZEGED, RAIL );
    addLink(g, BUDAPEST, KLAUSENBURG, LAND );
    addLink(g, ZAGREB, SZEGED, LAND );
    addLink(g, ZAGREB, ST_JOSEPH_AND_ST_MARYS, LAND );
    addLink(g, SZEGED, ST_JOSEPH_AND_ST_MARYS, LAND );
    addLink(g, SZEGED, KLAUSENBURG, LAND );
    addLink(g, SZEGED, BUCHAREST, RAIL );
    addLink(g, SZEGED, BELGRADE, LAND );
    addLink(g, SZEGED, BELGRADE, RAIL );
    addLink(g, ZAGREB, SARAJEVO, LAND );
    addLink(g, ST_JOSEPH_AND_ST_MARYS, SARAJEVO, LAND );
    addLink(g, ST_JOSEPH_AND_ST_MARYS, BELGRADE, LAND);
    addLink(g, SARAJEVO, BELGRADE, LAND );
    addLink(g, SARAJEVO, VALONA, LAND );
    addLink(g, BELGRADE, KLAUSENBURG, LAND );
    addLink(g, SARAJEVO, SOFIA, LAND );
    addLink(g, SARAJEVO, SOFIA, RAIL );
    addLink(g, BELGRADE, BUCHAREST, LAND );    
    addLink(g, KLAUSENBURG, CASTLE_DRACULA, LAND );
    addLink(g, KLAUSENBURG, BUCHAREST, LAND );
    addLink(g, KLAUSENBURG, GALATZ, LAND );
    addLink(g, CASTLE_DRACULA, GALATZ, LAND );
    addLink(g, GALATZ, BUCHAREST, LAND );
    addLink(g, GALATZ, BUCHAREST, RAIL );
    addLink(g, GALATZ, CONSTANTA, LAND );
    addLink(g, CONSTANTA, BUCHAREST, LAND );
    addLink(g, CONSTANTA, BUCHAREST, RAIL );
    addLink(g, CONSTANTA, VARNA, LAND );
    addLink(g, BUCHAREST, SOFIA, LAND );
    addLink(g, SOFIA, VARNA, LAND );
    addLink(g, SOFIA, VARNA, RAIL );
    addLink(g, SOFIA, SALONICA, LAND );
    addLink(g, SOFIA, SALONICA, RAIL );
    addLink(g, VALONA, SOFIA, LAND );
    addLink(g, VALONA, SALONICA, LAND);
    addLink(g, VALONA, ATHENS, LAND);


    //CZECH THIS OUT!
    addLink(g, PRAGUE, VIENNA, RAIL );
    addLink(g, PRAGUE, VIENNA, LAND );

}

