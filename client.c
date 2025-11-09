#include "serv_cli_fifo.h"
#include "Handlers_Cli.h"

int main(int argc, char *argv[]) {
    int fd_fifo1, fd_fifo2;
    Question question;
    Reponse reponse;
    struct sigaction sa;
    int nb_requetes;
    int i;
    pid_t serveur_pid;
    char continuer;
    
    if (argc > 1) {
        serveur_pid = atoi(argv[1]);
    } else {
        printf("Usage: %s <PID_serveur>\n", argv[0]);
        printf("Veuillez démarrer le serveur d'abord et noter son PID.\n");
        exit(1);
    }
    
    printf("[CLIENT] Démarrage du client (PID: %d)...\n", getpid());
    
    /* Ouverture des tubes nommés */
    printf("[CLIENT] Ouverture de FIFO1 en écriture...\n");
    fd_fifo1 = open(FIFO1, O_WRONLY);
    if (fd_fifo1 == -1) {
        perror("open FIFO1");
        exit(1);
    }
    
    printf("[CLIENT] Ouverture de FIFO2 en lecture (non-bloquant)...\n");
    fd_fifo2 = open(FIFO2, O_RDONLY | O_NONBLOCK);
    if (fd_fifo2 == -1) {
        perror("open FIFO2");
        exit(1);
    }
    /* Repasser en mode bloquant pour les lectures */
    int flags = fcntl(fd_fifo2, F_GETFL);
    fcntl(fd_fifo2, F_SETFL, flags & ~O_NONBLOCK);
    
    /* Installation du Handler pour SIGUSR1 */
    sa.sa_handler = hand_reveil;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    
    printf("[CLIENT] Client prêt.\n\n");
    
    /* Boucle pour envoyer plusieurs requêtes */
    i = 1;
    do {
        /* Demander à l'utilisateur le nombre de valeurs souhaitées */
        printf("========== Requête #%d ==========\n", i);
        printf("Combien de nombres aléatoires voulez-vous (1-%d) ? ", NMAX);
        
        if (scanf("%d", &question.n) != 1) {
            printf("Erreur de saisie.\n");
            while(getchar() != '\n'); /* Vider le buffer */
            continue;
        }
        while(getchar() != '\n'); /* Vider le buffer */
        
        /* Vérification de la validité */
        if (question.n <= 0 || question.n > NMAX) {
            printf("Erreur: le nombre doit être entre 1 et %d\n\n", NMAX);
            continue;
        }
        
        /* Construction et envoi d'une question */
        question.pid_client = getpid();
        
        printf("[CLIENT] Envoi de la requête (n=%d nombres demandés)...\n", 
               question.n);
        
        if (write(fd_fifo1, &question, sizeof(Question)) == -1) {
            perror("write FIFO1");
            continue;
        }
        
        /* Attente de la réponse (attente du signal SIGUSR1) */
        printf("[CLIENT] Mise en attente (pause)...\n");
        reveil = 0;
        while (!reveil) {
            pause(); /* Attend le signal SIGUSR1 du serveur */
        }
        printf("[CLIENT] Réveillé par SIGUSR1 du serveur\n");
        
        /* Lecture de la réponse */
        if (read(fd_fifo2, &reponse, sizeof(Reponse)) > 0) {
            printf("[CLIENT] Réponse reçue: %d valeurs\n", reponse.nb_valeurs);
            
            /* Traitement local de la réponse (calcul de la somme) */
            int j, somme = 0;
            printf("[CLIENT] Valeurs reçues: ");
            for (j = 0; j < reponse.nb_valeurs; j++) {
                printf("%d ", reponse.valeurs[j]);
                somme += reponse.valeurs[j];
            }
            printf("\n");
            
            float moyenne = (float)somme / reponse.nb_valeurs;
            printf("[CLIENT] Traitement local:\n");
            printf("          - Somme = %d\n", somme);
            printf("          - Moyenne = %.2f\n", moyenne);
            
            /* Envoi du signal SIGUSR1 au serveur (acquittement) */
            if (kill(serveur_pid, SIGUSR1) == -1) {
                perror("kill SIGUSR1");
            } else {
                printf("[CLIENT] Signal SIGUSR1 envoyé au serveur (acquittement)\n");
            }
        } else {
            perror("read FIFO2");
        }
        
        /* Demander si l'utilisateur veut continuer */
        printf("\nVoulez-vous envoyer une autre requête ? (o/n) : ");
        scanf(" %c", &continuer);
        while(getchar() != '\n'); /* Vider le buffer */
        printf("\n");
        
        i++;
    } while (continuer == 'o' || continuer == 'O');
    
    /* Fermeture des tubes */
    close(fd_fifo1);
    close(fd_fifo2);
    
    printf("[CLIENT] Client terminé.\n");
    return 0;
}