#ifndef _SERVER_H_
#define _SERVER_H_

#define MIN_PLAYERS 2
#define BACKLOG 5
#define TIME_INSCRIPTION 30

/**
* PRE: arg1 : a pointer to the write pipe
*      arg2 : a pointer to the read pipe
*      arg3 : a pointer to the socket file descriptor
* POST: the child process will read from the pipe and write to the socket
*       the child process will read from the socket and write to the pipe
*/
void childHandler(void *arg1,void *arg2, void *arg3);

/**
* PRE: arg : the signal number
* POST: set the global variable end_inscriptions to 1
*/
void endServerHandler(int sig);

/**
* POST: register players
*       if the number of players is less than MIN_PLAYERS, the game is canceled
*       else the game starts
* RES: return the table of players
*/
bool registerPlayer(int sockfd);

#endif