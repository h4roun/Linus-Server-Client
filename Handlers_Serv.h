#ifndef HANDLERS_SERV_H
#define HANDLERS_SERV_H

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

/* Variable globale pour la synchronisation */
volatile sig_atomic_t reveil = 0;

/* Handler pour SIGUSR1 - réveil du serveur */
void hand_reveil(int sig) {
    reveil = 1;
}

/* Handler pour terminer proprement le serveur */
void fin_serveur(int sig) {
    printf("\n[SERVEUR] Arrêt du serveur...\n");
    unlink(FIFO1);
    unlink(FIFO2);
    exit(0);
}

#endif