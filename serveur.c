#include "serv_cli_fifo.h"
#include "Handlers_Serv.h"

int main() {
    int fd_fifo1, fd_fifo2;
    Question question;
    Reponse reponse;
    struct sigaction sa_reveil, sa_fin;
    
    printf("[SERVEUR] Démarrage du serveur (PID: %d)...\n", getpid());
    
    /* Création des tubes nommés */
    if (mkfifo(FIFO1, 0666) == -1) {
        perror("mkfifo FIFO1");
        /* Si existe déjà, on continue */
    }
    if (mkfifo(FIFO2, 0666) == -1) {
        perror("mkfifo FIFO2");
        /* Si existe déjà, on continue */
    }
    
    /* Initialisation du générateur de nombres aléatoires */
    srand(getpid());
    
    /* Ouverture des tubes nommés */
    printf("[SERVEUR] Ouverture de FIFO1 en lecture...\n");
    fd_fifo1 = open(FIFO1, O_RDONLY);
    if (fd_fifo1 == -1) {
        perror("open FIFO1");
        exit(1);
    }
    
    printf("[SERVEUR] Ouverture de FIFO2 en écriture...\n");
    fd_fifo2 = open(FIFO2, O_WRONLY);
    if (fd_fifo2 == -1) {
        perror("open FIFO2");
        exit(1);
    }
    
    /* Installation des Handlers */
    sa_reveil.sa_handler = hand_reveil;
    sigemptyset(&sa_reveil.sa_mask);
    sa_reveil.sa_flags = 0;
    sigaction(SIGUSR1, &sa_reveil, NULL);
    
    sa_fin.sa_handler = fin_serveur;
    sigemptyset(&sa_fin.sa_mask);
    sa_fin.sa_flags = 0;
    sigaction(SIGINT, &sa_fin, NULL);
    sigaction(SIGTERM, &sa_fin, NULL);
    
    printf("[SERVEUR] Serveur prêt. En attente de requêtes...\n\n");
    
    while(1) {
        /* Lecture d'une question depuis FIFO1 */
        if (read(fd_fifo1, &question, sizeof(Question)) > 0) {
            printf("[SERVEUR] Question reçue du client PID=%d, n=%d\n", 
                   question.pid_client, question.n);
            
            /* Vérification que n est valide */
            if (question.n <= 0 || question.n > NMAX) {
                printf("[SERVEUR] Erreur: n doit être entre 1 et %d\n", NMAX);
                continue;
            }
            
            /* Construction de la réponse */
            /* Génère n nombres aléatoires entre 1 et NMAX */
            reponse.nb_valeurs = question.n;
            printf("[SERVEUR] Génération de %d nombres aléatoires:\n", question.n);
            int i;
            for (i = 0; i < question.n; i++) {
                reponse.valeurs[i] = (rand() % NMAX) + 1;
                printf("           valeur[%d] = %d\n", i, reponse.valeurs[i]);
            }
            
            /* Envoi de la réponse via FIFO2 */
            if (write(fd_fifo2, &reponse, sizeof(Reponse)) == -1) {
                perror("write FIFO2");
                continue;
            }
            printf("[SERVEUR] Réponse envoyée au client\n");
            
            /* Envoi du signal SIGUSR1 au client concerné pour le réveiller */
            if (kill(question.pid_client, SIGUSR1) == -1) {
                perror("kill SIGUSR1");
            } else {
                printf("[SERVEUR] Signal SIGUSR1 envoyé au client PID=%d (réveil)\n", 
                       question.pid_client);
            }
            
            /* Attente de l'acquittement du client */
            printf("[SERVEUR] Attente de l'acquittement du client...\n");
            reveil = 0;
            while (!reveil) {
                pause();
            }
            printf("[SERVEUR] Acquittement reçu du client\n\n");
        }
    }
    
    /* Fermeture et nettoyage */
    close(fd_fifo1);
    close(fd_fifo2);
    unlink(FIFO1);
    unlink(FIFO2);
    
    return 0;
}