#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../Hfichier/utils.h"

// Variables globales pour la gestion des processus et des jeux
extern int nbFilsNonBloquants;
extern int *resultF;
extern int *pidF;
extern jeu *jeux;
extern int nbJeux;

char* urlConforme(const char* str) {
    // Allocation dynamique pour la chaîne de résultat
    char* paramConforme = malloc(strlen(str) + 1);  // +1 pour le caractère de fin de chaîne
    if (paramConforme == NULL) {
        perror("malloc");
        return NULL;
    }

    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {  // Si c'est un espace
            paramConforme[i] = '-';  // Remplacer par un tiret
        } else {
            paramConforme[i] = str[i];  // Copier le caractère dans le nom original
        }
    }

    paramConforme[i] = '\0';  // Ajouter le caractère de fin de chaîne

    return paramConforme;
}

// Fonction execute
// Renvoie 0 si la fonction est bien terminée, sinon -1
int execute_demande(demandeOperation op) {
    int result = -1;
    char *text = NULL;
    char memoire[1000] = "";
    int fdResult[2];
    int fdMemoire[2];
    pid_t pid;

    if (pipe(fdMemoire) == -1 || pipe(fdResult) == -1) {
        perror("pipe");
        return -1;
    }

    // Les opérations 3 et 4 sont des opérations qui nécessitent d'apporter des modifications au tableau de jeux, donc il est plus judicieux de donner cette responsabilité au processus père
    // pour éviter toute complication avec les copies générées par les fils.
    // L'opération 4 (supprimer) ne peut être fait que par le père.
    if (op.codeOp == 4) {
        if (nbJeux != 0) {
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                    // On sauvegarde la taille du jeu supprimé
                    int tailleJeu = jeux[i].code ? strlen(jeux[i].code) : 0;

                    // On désinstalle le jeu
                    free(jeux[i].code);

                    // On décale les jeux suivants après l'emplacement du jeu supprimé d'une case à gauche
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

        // Auquel cas si le jeu n'a pas été trouvé ou qu'il n'y a aucun jeu dans la B.D.D des jeux de l'utilisateur.
        return -1;
    }

    // Les autres opérations peuvent être déléguées aux fils du processus père.
    else {
        // On fork pour créer un processus fils
        if ((pid = fork()) == 0) {
            // On ferme la lecture des deux pipes au fils
            close(fdResult[0]);

            switch (op.codeOp) {
                case 1: {
                    for (int i = 0; i < nbJeux; i++) {
                        // Si le jeu a été trouvé.
                        if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                            result = 0;
                            break;
                        }
                    }
                    break;
                }

                case 2: {
                    text = malloc(1024 * sizeof(char));
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
                    break;
                }

                case 3: {
                    int tailleJeu = rand() % 1000 + 1;
                    // Simuler le téléchargement du jeu
                    char *code = malloc(tailleJeu * sizeof(char));
                    if (!code) {
                        perror("malloc");
                        exit(EXIT_FAILURE);
                    }
                    printf("[%s] System |opération bloquante n°%d | - Ajout du jeu en cours : ...\n\n\n", op.nomJeu, op.codeOp);
                    memset(code, '*', tailleJeu);
                    sleep(10);
                    printf("[%s] System |opération bloquante n°%d | - Ajout du jeu terminé ! : %s\n\n\n", op.nomJeu, op.codeOp, code);
                    strcpy(memoire, code);
                    free(code);

                    write(fdMemoire[1], &memoire, sizeof(memoire));
                    result = strlen(memoire);
                    break;
                }

                case 5: {
                    // On ferme l'écriture du pipe
                    close(fdMemoire[1]);

                    for (int i = 0; i < nbJeux; i++) {
                        // Si le jeu a été trouvé.
                        if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                            read(fdMemoire[0], &memoire, sizeof(memoire) - 1);
                            
                            if(op.flag == 1)
                                printf("[%s] System |opération bloquante n°%d | - Simulation d'un combat entre J1 et J2...\n\n\n", jeux[i].nomJeu, op.codeOp);

                            sleep(20);

                            if(op.flag == 1)
                                printf("[%s] System |opération bloquante n°%d | - Le gagnant du combat est : %s\n\n\n", jeux[i].nomJeu, op.codeOp, rand()%2 == 0 ? "J2" : "J1");

                            result = 0;
                            break;
                        }
                    }
                    break;
                }

                case 6: {
                    close(fdMemoire[1]);
                    for (int i = 0; i < nbJeux; i++) {
                        // Si le jeu a été trouvé.
                        if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                            read(fdMemoire[0], &memoire, sizeof(memoire) - 1);

                            if(op.flag == 1)
                                printf("[%s] System |opération bloquante n°%d | - Chargement du jeu %s...\n\n\n", jeux[i].nomJeu, op.codeOp ,jeux[i].nomJeu);
                            
                            sleep(5);

                            if(op.flag == 1)
                                printf("[%s] System |opération bloquante n°%d | - Le jeu est lancé.\n\n\n", jeux[i].nomJeu, op.codeOp);

                            printf("[%s] System |opération bloquante n°%d | - Votre tour, veuillez choisir un caractère du clavier pour continuer : ", jeux[i].nomJeu, op.codeOp);

                            char input[1];
                            scanf("%c", input);

                            // On prétend qu'une partie de ce jeu peut durer entre 10 secondes à 1 minute.
                            sleep(rand() % 60 + 10);

                            if(op.flag == 1)
                                printf("[%s] System |opération bloquante n°%d | - Le gagnant de ce jeu est : %s\n\n\n", jeux[i].nomJeu, op.codeOp, rand() % 2 == 0 ? "Serveur" : "Joueur");

                            result = 0;
                            break;
                        }
                    }

                    // Si le jeu n'a pas été trouvé
                    break;
                }
            }

            // Pipe du résultat
            write(fdResult[1], &result, sizeof(result));
            close(fdResult[1]);

            if (op.codeOp == 5 || op.codeOp == 6) {
                close(fdMemoire[0]);
            } else {
                // Dans le cas des autres codeOp qui n'ont pas besoin de mémoire
                close(fdMemoire[1]);
                close(fdMemoire[0]);
            }

            exit(EXIT_SUCCESS);
        }

        // Si c'est le père.
        else if (pid > 0) {
            // On lit le résultat de l'opération
            close(fdResult[1]);

            if (op.codeOp == 5 || op.codeOp == 6) {
                close(fdMemoire[0]);

                // On recherche le jeu puis on passe son code au fils
                for (int i = 0; i < nbJeux; i++) {
                    if (strcmp(op.nomJeu, jeux[i].nomJeu) == 0) {
                        write(fdMemoire[1], jeux[i].code, strlen(jeux[i].code) + 1);
                        close(fdMemoire[1]);
                        break;
                    }
                }
            } 
        
            else 
            {
                close(fdMemoire[1]);
                close(fdMemoire[0]);
            }

            // Si l'opération est non bloquante
            if (op.flag == 0) {
                // Ajouter le PID du fils à la liste des fils non bloquants
                nbFilsNonBloquants++;

                pidF = realloc(pidF, nbFilsNonBloquants * sizeof(int));
                if (!pidF) {
                    perror("realloc");
                    return -1;
                }

                pidF[nbFilsNonBloquants - 1] = pid;

                resultF = realloc(resultF, nbFilsNonBloquants * sizeof(int));
                if (!resultF) {
                    perror("realloc");
                    return -1;
                }

                resultF[nbFilsNonBloquants - 1] = fdResult[0];

                printf("[!INFORMATION!] - Opération (demande n°%d) fils non bloquante de PID : %d s'est lancée en arrière-plan. Vous pouvez continuer vos autres tâches !\n\n\n", op.codeOp, pid);

            }


            // Si l'opération est bloquante (CA FONCTIONNE)
            else {
                // Attendre la fin de l'opération bloquante
                waitpid(pid, NULL, 0);

                if (op.codeOp == 3) {
                    close(fdMemoire[1]);
                    read(fdMemoire[0], &memoire, sizeof(memoire) - 1);
                    read(fdResult[0], &result, sizeof(result));
                    close(fdMemoire[0]);

                    // On crée un nouveau jeu et on l'ajoute au tableau jeux
                    jeu nouveauJeu;
                    strcpy(nouveauJeu.nomJeu, op.nomJeu);
                    nouveauJeu.code = malloc((strlen(memoire) + 1) * sizeof(char));
                    if (!nouveauJeu.code) {
                        perror("malloc");
                        return -1;
                    }
                    strcpy(nouveauJeu.code, memoire);

                    nbJeux++;
                    jeux = realloc(jeux, nbJeux * sizeof(jeu));
                    if (!jeux) {
                        perror("realloc");
                        return -1;
                    }
                    jeux[nbJeux - 1] = nouveauJeu;
                }

                close(fdResult[1]);
                read(fdResult[0], &result, sizeof(result));
                close(fdResult[0]);
            }

            return result;
        }

        // S'il y a erreur du Fork, c'est-à-dire strictement inférieur à 0.
        else {
            perror("fork");
            return -1;
        }
    }
}
