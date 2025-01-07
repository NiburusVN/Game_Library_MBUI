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

    int result = -1;
    int codeOp = atoi(argv[1]);
    int flag = atoi(argv[2]);
    int fdResult = atoi(argv[3]);
    int fdJeux = atoi(argv[4]);

    jeu *jeux = NULL;
    int nbJeux;

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

    char *text = malloc(1024 * sizeof(char));
    if (!text) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sprintf(text, "System |opération bloquante n°%d | - \n\n\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n", codeOp);

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

    if(flag == 1)
        printf("%s", text);
    
    write(fdResult, &result, sizeof(result));

    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);

    close(fdResult);
    close(fdJeux);
    free(text);

    exit(EXIT_SUCCESS);
}