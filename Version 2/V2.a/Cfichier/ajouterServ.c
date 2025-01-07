#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../Hfichier/utils.h"


int main(int argc, char *argv[]) {

    // ps: A faire dans tous les serveurs opérations

    char memoire[1000] = "";

    int result = -1;
    int codeOp = atoi(argv[1]);
    int flag = atoi(argv[2]);
    int fdResult = atoi(argv[3]);
    int fdMemoire = atoi(argv[4]);

    char nomJeu[25];
    strcpy(nomJeu, argv[5]);

    // 

    int tailleJeu = rand() % 1000 + 1;
    // Simuler le téléchargement du jeu
    char *code = malloc(tailleJeu * sizeof(char));
    if (!code) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if(flag == 1)
        printf("[%s] System |opération bloquante n°%d | - Ajout du jeu en cours : ...\n\n\n", nomJeu, codeOp);

    memset(code, '*', tailleJeu);

    sleep(10);

    if(flag == 1)
        printf("[%s] System |opération bloquante n°%d | - Ajout du jeu terminé ! : %s\n\n\n", nomJeu, codeOp, code);

    strcpy(memoire, code);

    result = strlen(memoire);
    write(fdResult, &result, sizeof(result));
    write(fdMemoire, &memoire, sizeof(memoire));

    close(fdResult);

    free(code);

    exit(EXIT_SUCCESS);
}