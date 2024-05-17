#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "config.h"


/**
 * PRE: serverIP : a valid IP address
 *      serverPort: a valid port number
 * POST: on success, connects a client socket to serverIP:serverPort
 *       on failure, displays error cause and quits the program
 * RES: return socket file descriptor
 */
int initSocketClient(char * serverIP, int serverPort);

/**
 * PRE:  serverPort: a valid port number
 * POST: on success, binds a socket to 0.0.0.0:serverPort and listens to it ;
 *       on failure, displays error cause and quits the program
 * RES: return socket file descriptor
 */
int initSocketServer(int serverPort);

/**
  * PRE: tabPlayers : a table of players
  *      nbPlayers : the number of players
  * POST: disconnect all players
*/
void disconnect_players(Player *tabPlayers, int nbPlayers);

#endif