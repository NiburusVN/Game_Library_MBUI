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

    const char *pipeTester = "pipeTester";

    // Ouverture des pipes nommés pour LIRE les demandes d'opération ainsi que préparer le canal pour renvoyer le résultat au serveur demandeur.

    while(1){
        int result = -1;
        pid_t pidEmetteur;
        char idOp[1024];
        pid_t pidRecepteur;
        demandeOperation op;
        char pipeResult[1024] = "pipeResult";
        int nbJeux;

        if (mkfifo(pipeTester, 0666) == -1) {
            perror("mkfifo");
            return -1;
        }

        int fd1 = open(pipeTester, O_RDONLY);
        if (fd1 == -1) {
            perror("Erreur ouverture pipe nommé pipeTester\n");
        }

        if (read(fd1, &op, sizeof(op)) == -1) {
            perror("Erreur lecture op du pipe nommé\n");
            close(fd1);
        }

        if (read(fd1, idOp, sizeof(idOp)) == -1) {
            perror("Erreur lecture idOp du pipe nommé\n");
            close(fd1);
        }


        if (read(fd1, &pidRecepteur, sizeof(pidRecepteur)) == -1) {
            perror("Erreur lecture pidRecepteur du pipe nommé\n");
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

        if((pidEmetteur = fork()) == 0){
            sleep(4);
            if(op.flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Recherche du jeu en cours : ...\n\n\n", op.nomJeu, op.codeOp);

            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    result = 0;
                    if(op.flag == 1)
                        printf("[%s]\n\n\nSystem |opération bloquante n°%d | - Jeu trouvé !\n\n\n", op.nomJeu, op.codeOp);
                    break;         
                }
            }

            if(result == -1)
                printf("\n\n\n[%s] System |opération bloquante n°%d | - Jeu non trouvé !\n\n\n", op.nomJeu, op.codeOp);

            
            sleep(1);

            kill(pidRecepteur, SIGUSR1);

            write(fd2, &result, sizeof(result));
            close(fd2);

            exit(EXIT_SUCCESS);
        }

        else if(pidEmetteur > 0){

            if(op.flag == 0){

                write(fd2, &pidEmetteur, sizeof(pidEmetteur));
                close(fd2);
                
            }


        }

        if (unlink(pipeTester) == -1) {
            perror("erreur de unlink pipeTester");
            return -1;
        }

        // Libérer la mémoire allouée
        for (int i = 0; i < nbJeux; i++) {
            free(jeux[i].code);
        }
        free(jeux);
    }
}