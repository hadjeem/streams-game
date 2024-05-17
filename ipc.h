#ifndef _IPC_CONF_H_
#define _IPC_CONF_H_

#include "config.h"

#define KEYIPC 36
#define KEYSEM 86
#define PERM 0666


/**
 * POST: create shared memory and semaphore
*/
void createIPC();

/**
 * POST: delete shared memory and semaphore
*/
void deleteIPC();

/**
* POST: get the table of players
* RES: return the table of players
*/
Player* getRankingTab();

#endif