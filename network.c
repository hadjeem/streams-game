#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config.h"
#include "network.h"
#include "utils_v1.h"


int initSocketClient(char * serverIP, int serverPort)
{
  int sockfd = ssocket();
  sconnect(serverIP, serverPort, sockfd);
  return sockfd;
}


int initSocketServer(int serverPort)
{
  int sockfd = ssocket();
  int option = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
  sbind(serverPort, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

void disconnect_players(Player *tabPlayers, int nbPlayers)
{
    for (int i = 0; i < nbPlayers; i++)
        sclose(tabPlayers[i].sockfd);
    return;
}