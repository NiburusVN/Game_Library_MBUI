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

    const char *pipeSimuler = "pipeSimuler";

    // Ouverture des pipes nommés pour LIRE les demandes d'opération ainsi que préparer le canal pour renvoyer le résultat au serveur demandeur.

    while(1){
        char memoire[1000];
        int result = -1;
        pid_t pidEmetteur;
        char idOp[1024];
        pid_t pidRecepteur;
        demandeOperation op;
        char pipeResult[1024] = "pipeResult";
        int nbJeux;

        if (mkfifo(pipeSimuler, 0666) == -1) {
            perror("mkfifo");
            return -1;
        }

        int fd1 = open(pipeSimuler, O_RDONLY);
        if (fd1 == -1) {
            perror("Erreur ouverture pipe nommé pipeSimuler\n");
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
            bool jeuTrouve = false;
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    jeuTrouve = true;
                    strcpy(memoire, jeux[i].code);

                    if(op.flag == 1){
                        printf("[%s] System |opération bloquante n°%d | - Chargement du jeu en cours : ...", jeux[i].nomJeu, op.codeOp);
                        sleep(2);
                        printf("[%s] System |opération bloquante n°%d | - Chargement du jeu terminé : %s", jeux[i].nomJeu, op.codeOp, memoire);
                        sleep(1);
                        printf("[%s] System |opération bloquante n°%d | - Simulation d'un combat entre J1 et J2...\n\n\n", jeux[i].nomJeu, op.codeOp);
                    }

                    sleep(20);

                    if(op.flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Le gagnant du combat est : %s\n\n\n", jeux[i].nomJeu, op.codeOp, rand()%2 == 0 ? "J2" : "J1");

                    result = 0;
                    break;
                }
            }

            if(!jeuTrouve && op.flag == 1){
                printf("[%s] System |opération bloquante n°%d | - Le jeu n'existe pas dans la biblitohèque !\n\n\n", op.nomJeu, op.codeOp);
            }
            
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

        if (unlink(pipeSimuler) == -1) {
            perror("erreur de unlink pipeSimuler");
            return -1;
        }

        // Libérer la mémoire allouée
        for (int i = 0; i < nbJeux; i++) {
            free(jeux[i].code);
        }
        free(jeux);


    }
}
