#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../Hfichier/utils.h"

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
    char memoire[1000] = "";
    int fdResult[2];
    int fdMemoire[2];
    int fdJeux[2];
    char fdResultStr[20];
    char fdJeuxStr[20];
    char fdMemoireStr[20];
    char codeOpStr[10];
    char flagStr[10];
    pid_t pid;

    if (pipe(fdMemoire) == -1 || pipe(fdResult) == -1 || pipe(fdJeux) == -1) {
        perror("pipe");
        return -1;
    }

    snprintf(codeOpStr, sizeof(codeOpStr), "%d", op.codeOp);
    snprintf(flagStr, sizeof(flagStr), "%d", op.flag);
    snprintf(fdResultStr, sizeof(fdResultStr), "%d", fdResult[1]);
    snprintf(fdJeuxStr, sizeof(fdJeuxStr), "%d", fdJeux[0]);


    // Les opérations 3 et 4 sont des opérations qui nécessitent d'apporter des modifications au tableau de jeux, donc il est plus judicieux de donner cette responsabilité au processus père
    // pour éviter toute complication avec les copies générées par les fils.

    char *args[8];

    switch (op.codeOp) {
        case 1: {
            args[0] = "./Cfichier/testerServ";
            args[1] = codeOpStr;
            args[2] = op.nomJeu;
            args[3] = flagStr;
            args[4] = fdResultStr;
            args[5] = fdJeuxStr;
            args[6] = NULL;
            break;
        }

        case 2: {
            args[0] = "./Cfichier/listerServ";
            args[1] = codeOpStr;
            args[2] = flagStr;
            args[3] = fdResultStr;
            args[4] = fdJeuxStr;
            args[5] = NULL;
            break;
        }

        case 3: {
            snprintf(fdMemoireStr, sizeof(fdMemoireStr), "%d", fdMemoire[1]);
            args[0] = "./Cfichier/ajouterServ";
            args[1] = codeOpStr;
            args[2] = flagStr;
            args[3] = fdResultStr;
            args[4] = fdMemoireStr;
            args[5] = op.nomJeu;
            args[6] = NULL;
            break;
        }

        case 4: {
            args[0] = "./Cfichier/supprimerServ";
            args[1] = codeOpStr;
            args[2] = flagStr;
            args[3] = fdResultStr;
            args[4] = fdJeuxStr;
            args[5] = op.nomJeu;
            args[6] = NULL;
            break;
        }

        case 5: {
            snprintf(fdMemoireStr, sizeof(fdMemoireStr), "%d", fdMemoire[0]);
            args[0] = "./Cfichier/simulerServ";
            args[1] = codeOpStr;
            args[2] = flagStr;
            args[3] = fdResultStr;
            args[4] = fdMemoireStr;
            args[5] = fdJeuxStr;
            args[6] = op.nomJeu;
            args[7] = NULL;
            break;
        }

        case 6: {
            snprintf(fdMemoireStr, sizeof(fdMemoireStr), "%d", fdMemoire[0]);
            args[0] = "./Cfichier/lancerServ";
            args[1] = codeOpStr;
            args[2] = flagStr;
            args[3] = fdResultStr;
            args[4] = fdMemoireStr;
            args[5] = fdJeuxStr;
            args[6] = op.nomJeu;
            args[7] = NULL;
            break;
        }
    }

    // On fork pour créer un processus fils
    if ((pid = fork()) == 0) {
        execv(args[0], args);
        // Si jamais ça n'a pas fait d'exec pour réécrire le code:
        perror("execv");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {

        close(fdResult[1]); 
        close(fdJeux[0]);

        // Envoyer les données du tableau de jeux via le pipe
        write(fdJeux[1], &nbJeux, sizeof(nbJeux));
        for (int i = 0; i < nbJeux; i++) {
            write(fdJeux[1], jeux[i].nomJeu, sizeof(jeux[i].nomJeu));
            int codeLen = strlen(jeux[i].code) + 1;
            write(fdJeux[1], &codeLen, sizeof(codeLen));
            write(fdJeux[1], jeux[i].code, codeLen);
        }

        close(fdJeux[1]);

        // On lit le résultat de l'opération
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
        
        else if (op.codeOp != 3) {
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
        
        else {
            // Si l'opération est bloquante
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
            
            else if (op.codeOp == 4) {
                int index;
                read(fdResult[0], &index, sizeof(index));
                close(fdResult[0]);

                // Si jamais result a bien l'index du jeu auquel on cherche à supprimer
                if (index != -1) {
                    int tailleJeu = jeux[index].code ? strlen(jeux[index].code) : 0;
                    free(jeux[index].code);
                    for (int e = index; e < nbJeux - 1; e++) {
                        jeux[e] = jeux[e + 1];
                    }
                    nbJeux--;
                    jeux = realloc(jeux, nbJeux * sizeof(jeu));
                    return tailleJeu;
                }
            }
            read(fdResult[0], &result, sizeof(result));
            close(fdResult[0]);
        }
        return result;
    } 
    else {
        // S'il y a erreur du Fork, c'est-à-dire strictement inférieur à 0.
        perror("fork");
        return -1;
    }

    return -1;
}
