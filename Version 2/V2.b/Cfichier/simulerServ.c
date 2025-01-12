#define _POSIX_C_SOURCE 200809L
#include <signal.h>   // Pour signal et kill
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
    const char *pipeSimuler = "pipeSimuler";

    while (1) {
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
            perror("Erreur ouverture pipe nommé pipeSimuler");
            return -1;
        }

        if (read(fd1, &op, sizeof(op)) == -1) {
            perror("Erreur lecture op du pipe nommé");
            close(fd1);
            return -1;
        }

        if (read(fd1, idOp, sizeof(idOp)) == -1) {
            perror("Erreur lecture idOp du pipe nommé");
            close(fd1);
            return -1;
        }

        if (read(fd1, &pidRecepteur, sizeof(pidRecepteur)) == -1) {
            perror("Erreur lecture pidRecepteur du pipe nommé");
            close(fd1);
            return -1;
        }

        if (read(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
            perror("Erreur lecture nbJeux du pipe nommé");
            close(fd1);
            return -1;
        }

        jeu *jeux = malloc(nbJeux * sizeof(jeu));
        if (!jeux) {
            perror("Erreur allocation mémoire pour jeux");
            close(fd1);
            return -1;
        }

        for (int i = 0; i < nbJeux; i++) {
            if (read(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1) {
                perror("Erreur lecture nomJeu du pipe nommé");
                close(fd1);
                free(jeux);
                return -1;
            }
            int codeLen;
            if (read(fd1, &codeLen, sizeof(codeLen)) == -1) {
                perror("Erreur lecture codeLen du pipe nommé");
                close(fd1);
                free(jeux);
                return -1;
            }

            jeux[i].code = malloc(codeLen * sizeof(char));
            if (!jeux[i].code) {
                perror("Erreur allocation mémoire pour code");
                close(fd1);
                free(jeux);
                return -1;
            }

            if (read(fd1, jeux[i].code, codeLen) == -1) {
                perror("Erreur lecture jeuCode du pipe nommé");
                close(fd1);
                free(jeux[i].code);
                free(jeux);
                return -1;
            }
        }

        close(fd1);

        strcat(pipeResult, idOp);

        int fd2 = open(pipeResult, O_WRONLY);
        if (fd2 == -1) {
            perror("Erreur ouverture pipe nommé pipeResult");
            free(jeux);
            return -1;
        }

        if ((pidEmetteur = fork()) == 0) {
            bool jeuTrouve = false;
            for (int i = 0; i < nbJeux; i++) {
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    jeuTrouve = true;
                    strcpy(memoire, jeux[i].code);

                    if (op.flag == 1) {
                        printf("[%s] System | opération bloquante n°%d | Chargement du jeu...\n", jeux[i].nomJeu, op.codeOp);
                        sleep(2);
                        printf("[%s] System | opération bloquante n°%d | Simulation en cours...\n", jeux[i].nomJeu, op.codeOp);
                    }

                    sleep(20);

                    if (op.flag == 1) {
                        printf("[%s] System | opération bloquante n°%d | Gagnant : %s\n", jeux[i].nomJeu, op.codeOp, rand() % 2 == 0 ? "J1" : "J2");
                    }

                    result = 0;
                    break;
                }
            }

            if (!jeuTrouve && op.flag == 1) {
                printf("[%s] System | opération bloquante n°%d | Jeu introuvable\n", op.nomJeu, op.codeOp);
            }

            sleep(1);

            if (op.flag == 0) {
                if (kill(pidRecepteur, SIGUSR1) == -1) {
                    perror("Erreur envoi signal SIGUSR1");
                }
            }

            if (write(fd2, &result, sizeof(result)) == -1) {
                perror("Erreur écriture dans pipeResult");
            }

            close(fd2);
            exit(EXIT_SUCCESS);
        } else if (pidEmetteur > 0) {
            if (op.flag == 0) {
                if (write(fd2, &pidEmetteur, sizeof(pidEmetteur)) == -1) {
                    perror("Erreur écriture du pidEmetteur dans pipeResult");
                }
                close(fd2);
            }
        }

        if (unlink(pipeSimuler) == -1) {
            perror("Erreur suppression pipeSimuler");
            return -1;
        }

        for (int i = 0; i < nbJeux; i++) {
            free(jeux[i].code);
        }
        free(jeux);
    }
}
