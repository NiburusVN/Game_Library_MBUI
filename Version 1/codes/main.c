#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Hfichier/utils.h"
#include "Cfichier/utils.c"

// Variables globales pour la gestion des processus et des jeux
int nbFilsNonBloquants = 0;
int *resultF = NULL;
int *pidF = NULL;
jeu *jeux = NULL;
int nbJeux = 0;

int main() {
    // Ajouter un jeu en mode non bloquant
    demandeOperation DeO1 = {3, "Echec", "http://echecetmat.com/", 1};  // flag = 0 pour non bloquant
    int res = execute_demande(DeO1);
    printf("Résultat ajout Echec : %d \n\n\n", res);

    // Afficher les jeux disponibles
    demandeOperation DeO2 = {2, "", "", 0};
    res = execute_demande(DeO2);

    // Ajouter un autre jeu en mode non bloquant
    demandeOperation DeO3 = {3, "Go", "http://goettic.com", 1};  // flag = 0 pour non bloquant
    res = execute_demande(DeO3);
    printf("Résultat ajout Go : %d \n\n\n", res);

    // Afficher les jeux disponibles
    demandeOperation DeO4 = {2, "", "", 1};
    res = execute_demande(DeO4);
    printf("Résultat du listage : %d \n\n\n", res);

    // Simuler un combat
    demandeOperation DeO5 = {5, "Echec", "", 1};
    res = execute_demande(DeO5);
    printf("Valeur de retour du combat : %d \n\n\n", res);

    // Vérifie si le fils a terminé
    int status;
    pid_t kidPid;

    do{

        kidPid = waitpid(-1, &status, WNOHANG);

        for(int i=0; i<nbFilsNonBloquants; i++){
            if(pidF[i] == kidPid){

                int result;
                read(resultF[i], &result, sizeof(int));
                printf("[!INFORMATION!] - Opération fils non bloquante de PID : %d s'est terminée. Valeur de retour : %d\n\n\n", pidF[i], result);
                if(nbFilsNonBloquants != 1){
                    // Remplace les éléments qui ne servent plus à la gestion comme c'est terminé.
                    pidF[i] = pidF[nbFilsNonBloquants - 1];
                    resultF[i] = resultF[nbFilsNonBloquants - 1];
                }

                nbFilsNonBloquants--;
            }
        }
    } while (nbFilsNonBloquants > 0);


    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);
    free(pidF);
    free(resultF);

    return 0;
}
