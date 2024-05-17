#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "utils_v1.h"


int randomTile(int *tilesTab){
    
    int tileIndex = randomIntBetween(0,40);
    while (tilesTab[tileIndex] == -1){
        tileIndex = randomIntBetween(0,40);
    }
    int tile = tilesTab[tileIndex];
    tilesTab[tileIndex] = -1;
    return tile;
}

int* createTileTab() {
    static int tileTab[MAX_TILE];
    int i;
    for (i = 0; i < 30; i++) {
        tileTab[i] = i + 1;
    }
    int doubler = 11;
    for (int j = i; j < MAX_TILE; j++) {
        tileTab[j] = doubler;
        doubler++;
    }
    return tileTab;
}


Player* sortTabPlayer(Player *players, int NB_PLAYERS){
    Player temp;
    for (int i = 0; i < NB_PLAYERS; i++){
        for (int j = i+1; j < NB_PLAYERS; j++){
            if (players[i].score < players[j].score){
                temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
    return players;
}
 
int calculateScore(int streams[20]){
  int points[20] = {0,1,3,5,7,9,11,15,20,25,30,35,40,50,60,70,85,100,150,300};  
  int score = 0;
  int suite = 0;
  int valeur = -1;
  for (int i = 0; i < 20; i++){
    if (streams[i] == 31) {
      score += points[suite-1];
      suite = 1;
    }
    else if(streams[i] >= valeur || streams[i-1] == 31) {
       suite++; 
    }
    else {
      score += points[suite-1];
      suite = 1;
    }
    valeur = streams[i];
  }
  score += points[suite-1];
  return score;
}
