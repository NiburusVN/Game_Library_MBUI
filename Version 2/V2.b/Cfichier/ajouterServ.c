#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>  // Pour mkfifo
#include "../Hfichier/utils.h"


int main(int argc, char *argv[]) {

    // ps: A faire dans tous les serveurs opérations

    const char *pipeAjouter = "pipeAjouter";

    // Ouverture des pipes nommés pour LIRE les demandes d'opération ainsi que préparer le canal pour renvoyer le résultat au serveur demandeur.

    while(1){
        int result = -1;
        pid_t pidEmetteur;
        char idOp[1024];
        demandeOperation op;
        char pipeResult[1024] = "pipeResult";
        char memoire[1000];

        if (mkfifo(pipeAjouter, 0666) == -1) {
            perror("mkfifo");
            return -1;
        }

        int fd1 = open(pipeAjouter, O_RDONLY);
        if (fd1 == -1) {
            perror("Erreur ouverture pipe nommé pipeAjouter\n");
        }

        if (read(fd1, &op, sizeof(op)) == -1) {
            perror("Erreur lecture op du pipe nommé\n");
            close(fd1);
        }

        if (read(fd1, idOp, sizeof(idOp)) == -1) {
            perror("Erreur lecture idOp du pipe nommé\n");
            close(fd1);
        }

        close(fd1);

        strcat(pipeResult, idOp);

        int fd2 = open(pipeResult, O_WRONLY);
        if (fd2 == -1) {
            perror("Erreur ouverture pipe nommé\n");
            return -1;
        }

        // 


        if((pidEmetteur = fork()) == 0){
            int tailleJeu = rand() % 1000 + 1;
            // Simuler le téléchargement du jeu
            char *code = malloc(tailleJeu * sizeof(char));
            if (!code) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            
            printf("[%s] System |opération bloquante n°%d | - Ajout du jeu en cours : ...\n\n\n", op.nomJeu, op.codeOp);

            memset(code, '*', tailleJeu);

            sleep(10);

            printf("[%s] System |opération bloquante n°%d | - Ajout du jeu terminé ! : %s\n\n\n", op.nomJeu, op.codeOp, code);

            strcpy(memoire, code);

            free(code);

            write(fd2, &memoire, sizeof(memoire));

            result = strlen(memoire);
            write(fd2, &result, sizeof(result));

            exit(EXIT_SUCCESS);
        }

        if (unlink(pipeAjouter) == -1) {
            perror("erreur de unlink pipeAjouter");
            return -1;
        }

    }
}