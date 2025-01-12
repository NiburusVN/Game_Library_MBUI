#define _POSIX_C_SOURCE 200809L
#include <signal.h>
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

    const char *pipeLister = "pipeLister";

    // Ouverture des pipes nommés pour LIRE les demandes d'opération ainsi que préparer le canal pour renvoyer le résultat au serveur demandeur.

    while(1){
        int result = -1;
        pid_t pidEmetteur;
        char idOp[1024];
        pid_t pidRecepteur;
        demandeOperation op;
        char pipeResult[1024] = "pipeResult";
        int nbJeux;

        if (mkfifo(pipeLister, 0666) == -1) {
            perror("mkfifo");
            return -1;
        }

        int fd1 = open(pipeLister, O_RDONLY);
        if (fd1 == -1) {
            perror("Erreur ouverture pipe nommé pipeLister\n");
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
            if(read(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                perror("Erreur lecture nomJeu du pipe nommé\n");
                close(fd1);
            }
            int codeLen;
            if(read(fd1, &codeLen, sizeof(codeLen)) == -1){
                perror("Erreur lecture codeLen du pipe nommé\n");
                close(fd1);
            }

            jeux[i].code = malloc(codeLen * sizeof(char));

            if(read(fd1, jeux[i].code, codeLen) == -1){
                perror("Erreur lecture jeuCode du pipe nommé\n");
                close(fd1);
            }
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
            char *text = malloc(1024 * sizeof(char));
            if (!text) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            sprintf(text, "System |opération bloquante n°%d | - \n\n\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n", op.codeOp);

            // Si il n'y a aucun jeu dans la B.D.D, on prévient l'utilisateur et on return -1.
            if (nbJeux == 0) {
                strcat(text, "Aucun jeu disponible ni téléchargé dans la base de données de jeux.\n\n\n");
            } else {
                // Affichage soit de la liste des jeux, soit d'un message de prévention d'une nullité de jeux téléchargés.
                bool estTelechargeable = false;

                for (int i = 0; i < nbJeux; i++) {
                    if (jeux[i].code != NULL) {
                        estTelechargeable = true;
                        text = realloc(text, strlen(text) + strlen(jeux[i].nomJeu) + 3);
                        if (!text) {
                            perror("realloc");
                            exit(EXIT_FAILURE);
                        }
                        strcat(text, jeux[i].nomJeu);

                        // Séparateur des noms de jeux
                        if (i != nbJeux - 1)
                            strcat(text, " | ");
                        else {
                            strcat(text, "\n\n-----------------------------------------------------------------------------------------\n\n\n");
                        }
                    }
                }

                // si aucun jeu n'est téléchargé
                if (!estTelechargeable) {
                    strcat(text, "Aucun jeu téléchargé dans la base de données de jeux.\n\n\n");
                
                } else {
                    sleep(1);
                    result = nbJeux;

                }

            }

            if(op.flag == 1)
                printf("%s", text);
            
            free(text);

            sleep(1);

            if(op.flag == 0){
                kill(pidRecepteur, SIGUSR1);
            }

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

        if (unlink(pipeLister) == -1) {
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
