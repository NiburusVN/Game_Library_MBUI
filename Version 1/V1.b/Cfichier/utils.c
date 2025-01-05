#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include "../Hfichier/utils.h"

// Variables globales pour la gestion des processus et des jeux

extern jeu *jeux;
extern int nbJeux;

extern threadResult* threadsNonBloquants;
extern int nbThreadsNonBloquants;

extern char memoire[1000];


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
void* execute_demande(void* arg) {


    int* result = malloc(sizeof(int));
    if (result == NULL) {
        perror("malloc");
        pthread_exit(NULL);
    }

    *result = -1;
    
    demandeOperation* demand = (demandeOperation*)arg;
    // Si c'est une opération non bloquante
    if( demand->flag == 0 ){
        // Ajouter les infos du thread à la liste des thread non bloquants
        nbThreadsNonBloquants++;

        threadsNonBloquants = realloc(threadsNonBloquants, nbThreadsNonBloquants * sizeof(int));
        if (!threadsNonBloquants) {
            perror("realloc");
            return result;
        }

        threadResult threadActuel;

        threadActuel.estFini = false;
        threadActuel.tid = syscall(SYS_gettid);

        threadsNonBloquants[nbThreadsNonBloquants - 1] = threadActuel;


        printf("[!INFORMATION!] - Opération (demande n°%d) fils non bloquante de TID : %d s'est lancée en arrière-plan. Vous pouvez continuer vos autres tâches !\n\n\n", demand->codeOp, threadActuel.tid);
    }

    switch (demand->codeOp) {
        case 1: {
            bool estTrouve = false;
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                estTrouve = true;
                if (strcmp(demand->nomJeu, jeux[i].nomJeu) == 0) {
                    *result = 0;
                    break;
                }
            }

            if(!estTrouve && demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Jeu non existant dans votre bibliothèque !\n", demand->nomJeu, demand->codeOp);
            break;
        }

        case 2: {
            // Allocation initiale pour text
            size_t allocated_size = 1024;
            char *text = malloc(allocated_size);
            if (!text) {
                perror("malloc");
                pthread_exit(NULL);
            }
            text[0] = '\0'; // Initialisation de la chaîne

            // Construction du message initial
            snprintf(text, allocated_size, "System | opération bloquante n°%d | - \n\n\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n", demand->codeOp);

            if (nbJeux == 0) {
                // Aucun jeu disponible
                strncat(text, "Aucun jeu disponible ni téléchargé dans la base de données de jeux.\n\n\n", allocated_size - strlen(text) - 1);
            } else {
                bool estTelechargeable = false;

                for (int i = 0; i < nbJeux; i++) {
                    if (jeux[i].code != NULL) {
                        estTelechargeable = true;

                        // Calcul de la nouvelle taille nécessaire
                        size_t needed_size = strlen(text) + strlen(jeux[i].nomJeu) + 4; // +4 pour " | " ou "\n"
                        if (needed_size >= allocated_size) {
                            // Augmentation de la taille de l'allocation
                            allocated_size = needed_size + 1; // +1 pour le terminateur '\0'
                            char *newText = realloc(text, allocated_size);
                            if (!newText) {
                                perror("realloc");
                                free(text);
                                pthread_exit(NULL);
                            }
                            text = newText;
                        }

                        // Ajout du nom du jeu
                        strcat(text, jeux[i].nomJeu);

                        // Ajout d'un séparateur ou d'une fin de ligne
                        if (i != nbJeux - 1) {
                            strcat(text, " | ");
                        } else {
                            strcat(text, "\n\n-----------------------------------------------------------------------------------------\n\n\n");
                        }
                    }
                }

                if (!estTelechargeable) {
                    strncat(text, "Aucun jeu téléchargé dans la base de données de jeux.\n\n\n", allocated_size - strlen(text) - 1);
                } else {
                    sleep(1); // Simulation d'une opération bloquante
                    *result = nbJeux;

                    if (demand->flag == 1) {
                        printf("%s", text);
                    }

                    free(text);
                    break;
                }
            }

            if (demand->flag == 1) {
                printf("%s", text);
            }

            free(text);
            break;
        }




        case 3: {

            jeu nouveauJeu;
            strcpy(nouveauJeu.nomJeu, demand->nomJeu);

            // On alloue un nombre aléatoire compris entre 1 et 1000 de caractères (+1 car si jamais on se retrouve à 0 ou à 1000, 2000... voire 999)
            int tailleCode = rand() % 1000 + 1;
            nouveauJeu.code = malloc(tailleCode * sizeof(char));

            // S'il y a une erreur du malloc == erreur de téléchargement
            if (!nouveauJeu.code)
                return result;

            // On simule le temps de téléchargement en comblant le code avec des * et en bloquant le serveur de 10 secondes.
            memset(nouveauJeu.code, '*', tailleCode);

            if(demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Ajout du jeu en cours : ...\n\n\n", demand->nomJeu, demand->codeOp);

            sleep(10);

            nbJeux++;
            jeux = realloc(jeux, nbJeux * sizeof(jeu));
            jeux[nbJeux-1] = nouveauJeu; 

            if(demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Ajout du jeu terminé ! : %s\n\n\n", demand->nomJeu, demand->codeOp, nouveauJeu.code);

            *result = tailleCode;

            break;

        }

        case 4: {
            bool estTrouve = false;
            if (nbJeux != 0) {
                for (int i = 0; i < nbJeux; i++) {
                    // Si le jeu a été trouvé.
                    if (strcmp(demand->nomJeu, jeux[i].nomJeu) == 0) {
                        estTrouve = true;
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

                        *result = tailleJeu;

                        break;
                    }
                }
            }

            if(!estTrouve && demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Jeu non existant dans votre bibliothèque !\n", demand->nomJeu, demand->codeOp);

            break;
        }

        case 5: {
            bool estTrouve = false;
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(demand->nomJeu, jeux[i].nomJeu) == 0) {
                    estTrouve = true;
                    strcpy(memoire, jeux[i].code);
                    
                    if(demand->flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Simulation d'un combat entre J1 et J2...\n\n\n", jeux[i].nomJeu, demand->codeOp);

                    sleep(20);

                    if(demand->flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Le gagnant du combat est : %s\n\n\n", jeux[i].nomJeu, demand->codeOp, rand()%2 == 0 ? "J2" : "J1");

                    *result = 0;
                    break;
                }
            }

            if(!estTrouve && demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Jeu non existant dans votre bibliothèque !\n", demand->nomJeu, demand->codeOp);

            break;
        }

        case 6: {
            bool estTrouve = false;
            for (int i = 0; i < nbJeux; i++) {
                // Si le jeu a été trouvé.
                if (strcmp(demand->nomJeu, jeux[i].nomJeu) == 0) {
                    estTrouve = true;
                    strcpy(memoire, jeux[i].code);

                    if(demand->flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Chargement du jeu %s...\n\n\n", jeux[i].nomJeu, demand->codeOp ,jeux[i].nomJeu);
                    
                    sleep(5);

                    if(demand->flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Le jeu est lancé.\n\n\n", jeux[i].nomJeu, demand->codeOp);

                    printf("[%s] System |opération bloquante n°%d | - Votre tour, veuillez choisir un caractère du clavier pour continuer : ", jeux[i].nomJeu, demand->codeOp);

                    char input[1];
                    scanf("%c", input);

                    // On prétend qu'une partie de ce jeu peut durer entre 10 secondes à 1 minute.
                    sleep(rand() % 60 + 10);

                    if(demand->flag == 1)
                        printf("[%s] System |opération bloquante n°%d | - Le gagnant de ce jeu est : %s\n\n\n", jeux[i].nomJeu, demand->codeOp, rand() % 2 == 0 ? "Serveur" : "Joueur");

                    *result = 0;
                    break;
                }
            }

            // Si le jeu n'a pas été trouvé
            if(!estTrouve && demand->flag == 1)
                printf("[%s] System |opération bloquante n°%d | - Jeu non existant dans votre bibliothèque !\n", demand->nomJeu, demand->codeOp);

            break;
        }

    }

    if(demand->flag == 0){
        threadsNonBloquants[nbThreadsNonBloquants - 1].estFini = true;
        threadsNonBloquants[nbThreadsNonBloquants - 1].result = *result;
        return NULL;
    }

    return result;
    
}
