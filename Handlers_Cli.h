#ifndef HANDLERS_CLI_H
#define HANDLERS_CLI_H

#include <signal.h>
#include <stdio.h>

/* Variable globale pour la synchronisation */
volatile sig_atomic_t reveil = 0;

/* Handler pour SIGUSR1 - réveil du client */
void hand_reveil(int sig) {
    reveil = 1;
}

#endif