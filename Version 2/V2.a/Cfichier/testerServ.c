#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../Hfichier/utils.h"

// Déclaration des variables globales
extern int nbFilsNonBloquants;
extern int *resultF;
extern int *pidF;
extern jeu *jeux;
extern int nbJeux;

int main(int argc, char *argv[]) {

    // ps: A faire dans tous les serveurs opérations

    int result = -1;
    int codeOp = atoi(argv[1]);
    
    char nomJeu[25];
    strcpy(nomJeu, argv[2]);

    int flag = atoi(argv[3]);
    int fdResult = atoi(argv[4]);
    int fdJeux = atoi(argv[5]);

    jeu *jeux = NULL;
    int nbJeux = 0;

    // Lire les données du pipe
    read(fdJeux, &nbJeux, sizeof(nbJeux));
    jeux = malloc(nbJeux * sizeof(jeu));
    for (int i = 0; i < nbJeux; i++) {
        read(fdJeux, jeux[i].nomJeu, sizeof(jeux[i].nomJeu));
        int codeLen;
        read(fdJeux, &codeLen, sizeof(codeLen));
        jeux[i].code = malloc(codeLen * sizeof(char));
        read(fdJeux, jeux[i].code, codeLen);
    }

    //

    if(flag == 1)
        printf("[%s] System |opération bloquante n°%d | - Recherche du jeu en cours : ...\n\n\n", nomJeu, codeOp);

    for (int i = 0; i < nbJeux; i++) {
        // Si le jeu a été trouvé.
        if (strcmp(nomJeu, jeux[i].nomJeu) == 0) {
            result = 0;
            if(flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Jeu trouvé !\n\n\n", nomJeu, codeOp);
            break;         
        }
    }

    if(result == -1)
        printf("[%s] System |opération bloquante n°%d | - Jeu non trouvé !\n\n\n", nomJeu, codeOp);
    
    write(fdResult, &result, sizeof(result));
    
    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);

    close(fdResult);
    close(fdJeux);

    exit(EXIT_SUCCESS);
}