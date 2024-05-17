#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SERVER_IP "127.0.0.1" /* localhost */
#define MAX_PSEUDO 256
#define MAX_PLAYERS 100
#define MAX_TILE_PLAYER 20
#define MAX_TAB 20
#define MAX_CHARACTERS 50
#define BACKLOG 5

#define INSCRIPTION_REQUEST 10
#define INSCRIPTION_OK 11
#define INSCRIPTION_KO 12
#define START_GAME 13
#define CANCEL_GAME 14
#define HAS_PLAYED 15
#define TILE_SENT 16
#define SCORE_ENTERED 17
#define RANKING_SENT 18


typedef struct
{
    char pseudo[MAX_CHARACTERS];
    int sockfd;
    int score;
} Player;

typedef struct
{
  char messageText[MAX_PSEUDO];
  int score;
  int code;
  int tile;
  int nbr_players;
  Player rankingTab[MAX_PLAYERS]; 
} StructMessage;
#endif
