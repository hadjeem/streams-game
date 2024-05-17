/*** Groupe 2 : 
Emroska Hadjera, 
Haouat Hajar, 
José Fechado Didia ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "utils_v1.h"
#include "network.h"
#include "server.h"
#include "game.h"
#include "ipc.h"

/*** globals variables ***/
int nbPLayers;
volatile sig_atomic_t end_registrations = 0;
volatile sig_atomic_t end_game = 0;

void childHandler(void *arg1, void *arg2, void *arg3)
{
    int *pipeWrite = (int *)arg1;
    int *pipeRead = (int *)arg2;
    int *sockfd = (int *)arg3;
    StructMessage msg;
    sclose(pipeWrite[0]);
    sclose(pipeRead[1]);

    for (int i = 0; i < 20; ++i)
    {
        sread(pipeRead[0], &msg, sizeof(msg));
        if (msg.code == TILE_SENT)
        {
            swrite(*sockfd, &msg, sizeof(msg));
        }
        sread(*sockfd, &msg, sizeof(msg));
        if (msg.code == HAS_PLAYED)
        {
            swrite(pipeWrite[1], &msg, sizeof(msg));
        }
    }

    sread(*sockfd, &msg, sizeof(msg));
    if (msg.code == SCORE_ENTERED)
    {
        swrite(pipeWrite[1], &msg, sizeof(msg));
    }

    int sid = sem_get(KEYSEM, 1);
    sem_down0(sid);
    Player *rankingTab = getRankingTab();
    for (int i = 0; i < nbPLayers; i++)
    {
        msg.rankingTab[i] = rankingTab[i];
    }
    msg.code = RANKING_SENT;
    msg.nbr_players = nbPLayers;
    sshmdt(rankingTab);
    sem_up0(sid);
    swrite(*sockfd, &msg, sizeof(msg));

    sclose(pipeRead[0]);
    sclose(pipeWrite[1]);
}

void endRegistrations(int sig)
{
    end_registrations = 1;
}

void handleEndGame(int sig)
{
    end_game = 1;
}

bool registerPlayer(int sockfd)
{
    int newsockfd, i;
    StructMessage msg;
    Player *tabPlayers = getRankingTab();

    ssigaction(SIGALRM, endRegistrations);


    i = 0;
    nbPLayers = 0;
    alarm(TIME_INSCRIPTION);

    while (!end_registrations)
    {
        /* client trt */
        newsockfd = accept(sockfd, NULL, NULL); // saccept() exit le programme si accept a été interrompu par l'alarme
        if (newsockfd > 0)                      /* no error on accept */
        {

            sread(newsockfd, &msg, sizeof(msg));

            if (msg.code == INSCRIPTION_REQUEST)
            {
                printf("Inscription demandée par le joueur : %s\n", msg.messageText);

                strcpy(tabPlayers[i].pseudo, msg.messageText);
                tabPlayers[i].sockfd = newsockfd;
                i++;

                if (nbPLayers < MAX_PLAYERS)
                {
                    msg.code = INSCRIPTION_OK;
                    nbPLayers++;
                    if (nbPLayers == MAX_PLAYERS)
                    {
                        alarm(0); // cancel alarm
                        end_registrations = 1;
                    }
                }
                else
                {
                    msg.code = INSCRIPTION_KO;
                }
                swrite(newsockfd, &msg, sizeof(msg));
                printf("Nb Inscriptions : %i\n", nbPLayers);
            }
        }
    }
    end_registrations = 0;
    printf("FIN DES INSCRIPTIONS\n");
    if (nbPLayers < MIN_PLAYERS)
    {
        printf("PARTIE ANNULEE .. PAS ASSEZ DE JOUEURS\n");
        msg.code = CANCEL_GAME;
        for (i = 0; i < nbPLayers; i++)
        {
            swrite(tabPlayers[i].sockfd, &msg, sizeof(msg));
        }
        disconnect_players(tabPlayers, nbPLayers);
        return false;
    }
    else
    {
        printf("Voici les joueurs présent :\n");
        for (int i = 0; i < nbPLayers; ++i)
        {

            printf("%s est inscrit\n", tabPlayers[i].pseudo);
        }
        printf("PARTIE VA DEMARRER ... \n");
        msg.code = START_GAME;
        for (i = 0; i < nbPLayers; i++)
            swrite(tabPlayers[i].sockfd, &msg, sizeof(msg));
    }
    return true;
}

int main(int argc, char const *argv[])
{
    ssigaction(SIGINT, handleEndGame);
    int sockfd = initSocketServer(atoi(argv[1]));
    printf("Le serveur tourne sur le port : %i \n", atoi(argv[1]));
    createIPC();
    FILE *file;
    if (argv[2] != NULL)
    {
        file = fopen(argv[2], "r");
        if (file == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }
    StructMessage msg;
    struct pollfd fds[MAX_PLAYERS];
    bool fds_invalid[MAX_PLAYERS];
    int pipeRead[MAX_PLAYERS][2];
    int pipeWrite[MAX_PLAYERS][2];
    Player *tabPlayers = getRankingTab();

    while (!end_game)
    {
        if (registerPlayer(sockfd))
        {

            for (int i = 0; i < nbPLayers; i++)
            {
                spipe(pipeRead[i]);
                spipe(pipeWrite[i]);
                fork_and_run3(childHandler, pipeRead[i], pipeWrite[i], &tabPlayers[i].sockfd);
                sclose(pipeRead[i][1]);
                sclose(pipeWrite[i][0]);
                fds_invalid[i] = false;
                fds[i].fd = pipeRead[i][0];
                fds[i].events = POLLIN;
            }
            printf("Les joueurs sont prêts\n");
            int *tileTab = createTileTab();
            printf("Le tableau de tuiles est prêt\n");
            for (int i = 0; i < 20; i++)
            {
                printf("Début du tour %d\n", (i + 1));
                if (argv[2] != NULL)
                {
                    fscanf(file, "%d", &msg.tile);
                }
                else
                {
                    msg.tile = randomTile(tileTab);
                }
                msg.code = TILE_SENT;
                for (int j = 0; j < nbPLayers; j++)
                {
                    swrite(pipeWrite[j][1], &msg, sizeof(msg));
                }
                int playerEnterTile = nbPLayers;
                while (playerEnterTile != 0)
                {
                    spoll(fds, nbPLayers, 0);
                    for (int j = 0; j < nbPLayers; j++)
                    {
                        if (fds[j].revents & POLLIN && !fds_invalid[j])
                        {
                            int ret = sread(pipeRead[j][0], &msg, sizeof(msg));
                            if (ret != 0 && msg.code == HAS_PLAYED)
                            {
                                fds_invalid[j] = true;
                                playerEnterTile--;
                            }
                        }
                    }
                }
                for (int j = 0; j < nbPLayers; j++)
                {
                    fds_invalid[j] = false;
                }
                printf("Fin du tour %d\n", (i + 1));
            }

            int playerEnterScore = nbPLayers;

            while (playerEnterScore != 0)
            {
                spoll(fds, nbPLayers, 0);
                for (int j = 0; j < nbPLayers; j++)
                {
                    if (fds[j].revents & POLLIN)
                    {
                        int ret = sread(pipeRead[j][0], &msg, sizeof(msg));
                        if (ret != 0 && msg.code == SCORE_ENTERED)
                        {
                            tabPlayers[j].score = msg.score;
                            playerEnterScore--;
                        }
                    }
                }
            }

            tabPlayers = sortTabPlayer(tabPlayers, nbPLayers);
            int sid = sem_get(KEYSEM, 1);
            sem_up0(sid);
            int status = 0;
            while (wait(&status) > 0)
                ;
            sem_down0(sid);
        }
    }
    deleteIPC(tabPlayers);
}