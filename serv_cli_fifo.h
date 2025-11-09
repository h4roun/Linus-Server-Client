#ifndef SERV_CLI_FIFO_H
#define SERV_CLI_FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

/* Constantes */
#define NMAX 100
#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"

/* Structure pour la communication */
typedef struct {
    pid_t pid_client;
    int n;  /* nombre de valeurs aléatoires demandées */
} Question;

typedef struct {
    int nb_valeurs;  /* nombre de valeurs à lire */
    int valeurs[NMAX];  /* tableau des valeurs aléatoires */
} Reponse;

#endif