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

    const char *pipeSupprimer = "pipeSupprimer";

    // Ouverture des pipes nommés pour LIRE les demandes d'opération ainsi que préparer le canal pour renvoyer le résultat au serveur demandeur.

    while(1){
        int result = -1;
        pid_t pidEmetteur;
        char idOp[1024];
        demandeOperation op;
        char pipeResult[1024] = "pipeResult";
        int nbJeux;

        if (mkfifo(pipeSupprimer, 0666) == -1) {
            perror("mkfifo");
            return -1;
        }

        int fd1 = open(pipeSupprimer, O_RDONLY);
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

        if (read(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
            perror("Erreur lecture nbJeux du pipe nommé\n");
            close(fd1);
        }

        jeu *jeux = NULL;

        jeux = malloc(nbJeux * sizeof(jeu));
        for (int i = 0; i < nbJeux; i++) {
            read(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu));
            int codeLen;
            read(fd1, &codeLen, sizeof(codeLen));
            jeux[i].code = malloc(codeLen * sizeof(char));
            read(fd1, jeux[i].code, codeLen);
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

            printf("[%s] System |opération bloquante n°%d | - Suppression du jeu en cours : ...\n\n\n", op.nomJeu, op.codeOp);

            sleep(2);

            if (nbJeux != 0) {
                for (int i = 0; i < nbJeux; i++) {
                    // Si le jeu a été trouvé.
                    if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                        
                        // On retourne l'indice du jeune
                        result = i;
 
                        printf("[%s] System |opération bloquante n°%d | - Suppression du jeu terminé !\n\n\n", op.nomJeu, op.codeOp);
                        break;
                    }
                }
            }

            if(result == -1)
                printf("[%s] System |opération bloquante n°%d | - Suppression du jeu non aboutie !\n\n\n", op.nomJeu, op.codeOp);

            write(fd2, &result, sizeof(result));

            exit(EXIT_SUCCESS);
        }

        if (unlink(pipeSupprimer) == -1) {
            perror("erreur de unlink pipeSupprimer");
            return -1;
        }

        // Libérer la mémoire allouée
        for (int i = 0; i < nbJeux; i++) {
            free(jeux[i].code);
        }
        free(jeux);

    }
}