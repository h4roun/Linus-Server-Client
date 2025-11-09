#include "serv_cli_fifo.h"
#include "Handlers_Cli.h"

int main(int argc, char *argv[]) {
    int fd_fifo1, fd_fifo2;
    Question question;
    Reponse reponse;
    struct sigaction sa;
    pid_t serveur_pid;
    int n;
    int j, somme = 0;
    float moyenne;
    
    /* Vérification des arguments */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <PID_serveur> <nombre_valeurs>\n", argv[0]);
        exit(1);
    }
    
    serveur_pid = atoi(argv[1]);
    n = atoi(argv[2]);
    
    /* Validation */
    if (n <= 0 || n > NMAX) {
        fprintf(stderr, "Erreur: n doit être entre 1 et %d\n", NMAX);
        exit(1);
    }
    
    /* Ouverture des tubes nommés */
    fd_fifo1 = open(FIFO1, O_WRONLY);
    if (fd_fifo1 == -1) {
        perror("open FIFO1");
        exit(1);
    }
    
    fd_fifo2 = open(FIFO2, O_RDONLY | O_NONBLOCK);
    if (fd_fifo2 == -1) {
        perror("open FIFO2");
        close(fd_fifo1);
        exit(1);
    }
    /* Repasser en mode bloquant */
    int flags = fcntl(fd_fifo2, F_GETFL);
    fcntl(fd_fifo2, F_SETFL, flags & ~O_NONBLOCK);
    
    /* Installation du Handler pour SIGUSR1 */
    sa.sa_handler = hand_reveil;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    
    /* Construction et envoi de la question */
    question.pid_client = getpid();
    question.n = n;
    
    if (write(fd_fifo1, &question, sizeof(Question)) == -1) {
        perror("write FIFO1");
        close(fd_fifo1);
        close(fd_fifo2);
        exit(1);
    }
    
    /* Attente de la réponse */
    reveil = 0;
    while (!reveil) {
        pause();
    }
    
    /* Lecture de la réponse */
    if (read(fd_fifo2, &reponse, sizeof(Reponse)) <= 0) {
        perror("read FIFO2");
        close(fd_fifo1);
        close(fd_fifo2);
        exit(1);
    }
    
    /* Affichage des résultats (format parsable) */
    printf("Réponse reçue: %d valeurs\n", reponse.nb_valeurs);
    printf("Valeurs reçues: ");
    for (j = 0; j < reponse.nb_valeurs; j++) {
        printf("%d ", reponse.valeurs[j]);
        somme += reponse.valeurs[j];
    }
    printf("\n");
    
    moyenne = (float)somme / reponse.nb_valeurs;
    printf("Somme = %d\n", somme);
    printf("Moyenne = %.2f\n", moyenne);
    
    /* Envoi du signal SIGUSR1 au serveur (acquittement) */
    if (kill(serveur_pid, SIGUSR1) == -1) {
        perror("kill SIGUSR1");
    }
    
    /* Fermeture des tubes */
    close(fd_fifo1);
    close(fd_fifo2);
    
    return 0;
}