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
    int fdJeux = atoi(argv[5]);
    char nomJeu[25];
    strcpy(nomJeu, argv[6]);

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

    for (int i = 0; i < nbJeux; i++) {
        // Si le jeu a été trouvé.
        if (strcmp(nomJeu, jeux[i].nomJeu) == 0) {
            read(fdMemoire, &memoire, sizeof(memoire) - 1);

            if(flag == 1){
                printf("[%s] System |opération bloquante n°%d | - Chargement du jeu en cours : ...\n\n\n", jeux[i].nomJeu, codeOp);
            
                sleep(5);

                printf("[%s] System |opération bloquante n°%d | - Chargement du jeu terminé : %s", jeux[i].nomJeu, codeOp, memoire);

                sleep(1);
            
                printf("[%s] System |opération bloquante n°%d | - Le jeu est lancé.\n\n\n", jeux[i].nomJeu, codeOp);

            }

            printf("[%s] System |opération bloquante n°%d | - Votre tour, veuillez choisir un caractère du clavier pour continuer : ", jeux[i].nomJeu, codeOp);

            char input[1];
            scanf("%c", input);

            // On prétend qu'une partie de ce jeu peut durer entre 10 secondes à 1 minute.
            sleep(rand() % 60 + 10);

            if(flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Le gagnant de ce jeu est : %s\n\n\n", jeux[i].nomJeu, codeOp, rand() % 2 == 0 ? "Serveur" : "Joueur");

            result = 0;

            break;
        }
    }

    write(fdResult, &result, sizeof(result));

    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);

    close(fdResult);
    close(fdMemoire);
    close(fdJeux);

    exit(EXIT_SUCCESS);
}