#define _POSIX_C_SOURCE 200809L
#include "Hfichier/utils.h"
#include <stdio.h>

jeu *jeux = NULL;
int nbJeux = 0;
char memoire[1000];

int main() {
    // Exemple avec une seule demande d'ajout
    demandeOperation DeO1 = {1, "Echec", "", 0};
    int res = execute_demande(DeO1);
    printf("Résultat : %d \n", res);

    // Ajouter un jeu
    demandeOperation DeO2 = {3, "Echec", "http://echecetmat.com/", 0};
    res = execute_demande(DeO2);
    printf("Résultat : %d \n", res);

    // Afficher les jeux disponibles
    demandeOperation DeO3 = {2, "", "", 0};
    res = execute_demande(DeO3);
    printf("Résultat : %d \n", res);

    // Simuler un combat
    demandeOperation DeO5 = {5, "Echec", "", 0};
    res = execute_demande(DeO5);
    printf("Résultat : %d \n", res);

    // Lancer un jeu
    demandeOperation DeO6 = {6, "Echec", "", 0};
    res = execute_demande(DeO6);
    printf("Résultat : %d \n", res);

    // Supprimer un jeu
    demandeOperation DeO4 = {4, "Echec", "", 0};
    res = execute_demande(DeO4);
    printf("Résultat : %d \n", res);

    // Libérer la mémoire avant la fin du programme
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);

    return 0;
}
