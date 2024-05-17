#ifndef _GAME_H_
#define _GAME_H_

#define MAX_TILE 41

#include "config.h"

/**
* POST: create a table of tiles
* RES: return the table
*/
int* createTileTab();

/**
* PRE: tileTab : a table of tiles
* POST: display the table of tiles
* RES: return the table
*/
int randomTile(int *tilesTab);

/**
* PRE: players : a table of players
*      NB_PLAYERS : the number of players
* POST: sort the table of players by score
* RES: return the table of players sorted
*/
Player* sortTabPlayer(Player *players, int NB_PLAYERS);

/**
* PRE: streams : a table of streams
* POST: calculate the score of a player
* RES: return the score
*/
int calculateScore(int streams[20]);
#endif