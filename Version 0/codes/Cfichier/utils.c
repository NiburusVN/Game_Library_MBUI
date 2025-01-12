#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../Hfichier/utils.h"

// Tableau dynamique contenant des jeux et agissant en tant que "B.D.D des jeux possédés par l'utilisateur."
extern jeu *jeux;
extern int nbJeux;
extern char memoire[1000];

// Fonction execute
// Renvoie 0 si la fonction est bien terminée, sinon -1
int execute_demande(demandeOperation op) {
    switch (op.codeOp) {
        // Teste si le nom du jeu contenu dans la demande d'opération se trouve dans la B.D.D des jeux de l'utilisateur.
        case 1: {
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0)
                    return 0;
            }
            return -1;
        }

        // Affiche les jeux disponibles et téléchargés dans la B.D.D des jeux de l'utilisateur
        case 2: {
            printf("\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n");

            // Si aucun jeu dans la B.D.D, prévenir l'utilisateur et return -1.
            if (nbJeux == 0) {
                printf("Aucun jeu disponible ni téléchargé dans la base de données de jeux.\n");
            } else {
                bool estTelechargeable = false;

                for (int i = 0; i < nbJeux; i++) {
                    if (jeux[i].code != NULL) {
                        estTelechargeable = true;
                        printf("[%d. %s]", i + 1, jeux[i].nomJeu);

                        // Séparateur des noms de jeux
                        if (i != nbJeux - 1)
                            printf(" | ");
                        else
                            printf("\n");
                    }
                }

                if (!estTelechargeable) {
                    printf("Aucun jeu téléchargé dans la base de données de jeux.\n");
                } else {
                    sleep(1);
                    return nbJeux;
                }
            }
            return -1;
        }

        // Demande pour ajouter un nouveau jeu et le télécharger dans la B.D.D des jeux de l'utilisateur
        case 3: {
            jeu nouveauJeu;
            strcpy(nouveauJeu.nomJeu, op.nomJeu);

            // Allouer un nombre aléatoire de caractères entre 1 et 1000 pour simuler le code du jeu
            int tailleCode = rand() % 1000 + 1;
            nouveauJeu.code = malloc(tailleCode * sizeof(char));

            // Si malloc échoue, retourner -1
            if (!nouveauJeu.code)
                return -1;

            // Simuler le temps de téléchargement
            memset(nouveauJeu.code, '*', tailleCode);

            nbJeux++;
            jeux = realloc(jeux, nbJeux * sizeof(jeu));
            jeux[nbJeux - 1] = nouveauJeu;

            sleep(10);

            return tailleCode;
        }

        // Demande pour supprimer un jeu choisi par l'utilisateur de sa B.D.D des jeux.
        case 4: {
            if (nbJeux != 0) {
                for (int i = 0; i < nbJeux; i++) {
                    // Si le jeu est trouvé
                    if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                        int tailleJeu;
                        if (jeux[i].code != NULL)
                            tailleJeu = strlen(jeux[i].code);
                        else
                            tailleJeu = 0;

                        // Désinstaller le jeu
                        free(jeux[i].code);

                        // Décaler les jeux suivants
                        for (int e = i; e < nbJeux - 1; e++) {
                            jeux[e] = jeux[e + 1];
                        }

                        nbJeux--;
                        jeux = realloc(jeux, nbJeux * sizeof(jeu));

                        sleep(2);

                        return tailleJeu;
                    }
                }
            }

            // Si jeu non trouvé ou B.D.D vide
            return -1;
        }

        // Demande de simulation d'un combat contre l'ordinateur
        case 5: {
            printf("nbJeux = %d\n", nbJeux);
            for (int i = 0; i < nbJeux; i++) {
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    strcpy(memoire, jeux[i].code);

                    printf("Simulation d'un combat entre J1 et J2...\n");

                    sleep(20);

                    printf("Le gagnant du combat est : ");
                    if (rand() % 2 == 0)
                        printf("J2 !\n");
                    else
                        printf("J1 !\n");

                    return 0;
                }
            }

            // Si le jeu n'a pas été trouvé
            return -1;
        }

        // Demande pour lancer le jeu choisi par l'utilisateur.
        case 6: {
            for (int i = 0; i < nbJeux; i++) {
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    strcpy(memoire, jeux[i].code);

                    printf("Chargement du jeu %s...\n", jeux[i].nomJeu);
                    sleep(5);
                    printf("Le jeu est lancé.\n");

                    printf("Votre tour, veuillez choisir un caractère du clavier pour continuer : ");
                    char input[1];

                    // Vérification du retour de scanf
                    if (scanf("%c", input) != 1) {
                        fprintf(stderr, "Erreur de lecture de l'entrée\n");
                        return -1;
                    }

                    // Durée simulée entre 10 et 60 secondes
                    sleep(rand() % 60 + 10);

                    printf("Le gagnant de ce jeu est : ");
                    if (rand() % 2 == 0)
                        printf("Serveur !\n");
                    else
                        printf("Joueur !\n");

                    return 0;
                }
            }

            // Si le jeu n'a pas été trouvé
            return -1;
        }

        default:
            return -1; // Code non reconnu
    }
}
