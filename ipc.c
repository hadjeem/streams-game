#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "utils_v1.h"
#include "ipc.h"
#include "config.h"


void createIPC(){
    sshmget(KEYIPC, MAX_PLAYERS * sizeof(Player), IPC_CREAT | PERM);
    sem_create(KEYSEM, 1, IPC_CREAT | IPC_EXCL | PERM, 0);
}

void deleteIPC(Player* z){
  int shm_id = sshmget(KEYIPC,(MAX_PLAYERS * sizeof(Player)), 0);
  sshmdt(z);
    sshmdelete(shm_id);
  int sem_id = sem_get(KEYSEM, 1);
    sem_delete(sem_id);
}

Player* getRankingTab(){
    Player *z = sshmat(sshmget(KEYIPC,sizeof(Player), 0));
    return z;
}