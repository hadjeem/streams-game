#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network.h"
#include "game.h"
#include "config.h"
#include "utils_v1.h"

void displayPlayerTileTab(int* playerTileTab){
    for (int i = 0; i < MAX_TILE_PLAYER; i++){
      if(playerTileTab[i] != -1) {
        printf(" %d ",playerTileTab[i]);
      } else {
        printf(" _ ");
      }
    }
    printf("\n");
}

void enterPosition(int position,int* positionTab,int tile){
    if(positionTab[position] == -1){
        positionTab[position] = tile;
    } else {
        int i = position;
        while(positionTab[i] != -1){
            i++;
            if(i == MAX_TILE_PLAYER){
                i = 0;
            }
        }
        positionTab[i] = tile;
    }
}

int main(int argc, char **argv)
{
  /* retrieve player name */
  printf("Bienvenue dans le programe d'inscription au serveur de jeu\n");
  printf("Pour participer entrez votre nom :\n");
  StructMessage msg;
  char* ret = fgets(msg.messageText, sizeof(msg.messageText)-1, stdin);
  if (ret == NULL)
  {
    perror("fgets");
    exit(EXIT_FAILURE);
  }
  msg.messageText[strlen(msg.messageText) - 1] = '\0';
  msg.code = INSCRIPTION_REQUEST;

  int sockfd = initSocketClient(SERVER_IP, atoi(argv[1]));
  swrite(sockfd, &msg, sizeof(msg));

  /* wait server response */
  sread(sockfd, &msg, sizeof(msg));

  if (msg.code == INSCRIPTION_OK)
  {
    printf("Réponse du serveur : Inscription acceptée\n");
  }
  else if (msg.code == INSCRIPTION_KO)
  {
    printf("Réponse du serveur : Inscription refusée\n");
  }

    /* wait server start game */
    sread(sockfd, &msg, sizeof(msg));
    if(msg.code == CANCEL_GAME){
        printf("La partie a été annulée\n");
        sclose(sockfd);
        return 0;
    }
    if (msg.code == START_GAME)
    {
        printf("Début de la partie\n");
    }

    int tour = 0;
    int playerTileTab[MAX_TILE_PLAYER] = {-1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1,-1, -1,};
    while (tour < 20){
    /* wait server tile */
    sread(sockfd, &msg, sizeof(msg));
    if (msg.code == TILE_SENT)
    {
        printf("Tu as reçu la tuile %d\n", msg.tile);
    }

    /* wait player action */
    printf("Où veux-tu placer ta tuile ?\n");
    displayPlayerTileTab(playerTileTab);
    int tile;
    scanf("%d", &tile);
    enterPosition(tile-1, playerTileTab, msg.tile);
    displayPlayerTileTab(playerTileTab);
    
    /* send player action */
    msg.code = HAS_PLAYED;
    msg.tile = tile;
    swrite(sockfd, &msg, sizeof(msg));
    tour++;
    }

    /* calculating the gamer's score */
    int score = calculateScore(playerTileTab);
    printf("Ton score est de %d\n", score);
    msg.code = SCORE_ENTERED;
    msg.score = score;
    printf("Envoi de ton score%d\n",msg.score);
    swrite(sockfd, &msg, sizeof(msg));
    printf("Ton score a été envoyé\n");

    /* wait server ranking */
    sread(sockfd, &msg, sizeof(msg));
    if (msg.code == RANKING_SENT)
    {
        printf("Le classement est :\n");
        for (int i = 0; i < msg.nbr_players; i++)
        {
            printf("%d - %s : %d\n", i + 1, msg.rankingTab[i].pseudo, msg.rankingTab[i].score);
        }
    }

  sclose(sockfd);
  return 0;
}