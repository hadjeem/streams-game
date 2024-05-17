#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "config.h"

/**
* POST: create a table of player's tiles
* RES: return the table
*/
int* createPlayerTileTab();

/**
*PRE: playerTileTab : a table of player's tiles
*POST: display the table of player's tiles
*/
void displayPlayerTileTab(int* playerTileTab);

#endif