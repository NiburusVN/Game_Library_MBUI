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

//	fonction execute
//renvoie 0 si la fonction est bien terminé, sinon -1
int execute_demande(demandeOperation op){
    switch(op.codeOp){
        
        //Teste si le nom du jeu contenu dans la demande d'opération se trouve dans la B.D.D des jeux de l'utilisateur.
        case 1:
        {
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0)
                    return 0;
            }
            return -1;
        }

        //Affiche les jeux disponibles et téléchargés dans la B.D.D des jeux de l'utilisateur
        case 2:
        {   
            printf("\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n");

            // Si il n'y a aucun jeu dans la B.D.D, on prévient l'utilisateur et on return -1 .
            if(nbJeux == 0){
                printf("Aucun jeux disponibles ni téléchargés dans la base de données de jeux.\n");
            }

            else{

                // Affichage soit de la liste des jeux, soit d'un message de prévention d'une nullité de jeux téléchargés.
                bool estTelechargeable = false;

                for(int i = 0; i<nbJeux; i++){

                    if(jeux[i].code != NULL){
                        estTelechargeable = true;
                        printf("[%d. %s]", i+1, jeux[i].nomJeu);
                        
                        // Séparateur des noms de jeux
                        if(i != nbJeux -1)
                            printf(" | ");
                        else{
                            printf("\n");
                        }
                    }

                }

                // si aucun jeu n'est téléchargé
                if(!estTelechargeable){
                    printf("Aucun jeu téléchargé dans la base de données de jeux.\n");
                }

                else{
                    sleep(1);
                    return nbJeux;
                }
                
            }   

            return -1;
        }

        // Demande pour ajouter un nouveau jeu et le télécharger dans la B.D.D des jeux de l'utilisateur
        case 3:
        {
            jeu nouveauJeu;
            strcpy(nouveauJeu.nomJeu, op.nomJeu);

            // On alloue un nombre aléatoire compris entre 1 et 1000 de caractères (+1 car si jamais on se retrouve à 0 ou à 1000, 2000... voire 999)
            int tailleCode = rand() % 1000 + 1;
            nouveauJeu.code = malloc(tailleCode * sizeof(char));

            // S'il y a une erreur du malloc == erreur de téléchargement
            if (!nouveauJeu.code)
                return -1;

            // On simule le temps de téléchargement en comblant le code avec des * et en bloquant le serveur de 10 secondes.
            memset(nouveauJeu.code, '*', tailleCode);

            nbJeux++;
            jeux = realloc(jeux, nbJeux * sizeof(jeu));
            jeux[nbJeux-1] = nouveauJeu; 

            sleep(10);

            return tailleCode;
        }
        
        // Demande pour supprimer un jeu choisi par l'utilisateur de sa B.D.D des jeux.
        case 4:
        {
            if(nbJeux != 0){

                for(int i=0; i<nbJeux; i++){

                    //Si le jeu a été trouvé.
                    if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){
                        
                        // On sauvegarde la taille du jeu supprimé
                        int tailleJeu;
                        if(jeux[i].code != NULL)
                            tailleJeu = strlen(jeux[i].code);
                    
                        else{
                            tailleJeu = 0;
                        }


                        // On désinstalle le jeu
                        free(jeux[i].code);

                        // On décale les jeux suivants après l'emplacement du jeu supprimé d'une case à gauche 
                        for (int e = i; e<nbJeux - 1; e++) {
                            jeux[e] = jeux[e + 1];
                        }

                        nbJeux--;
                        jeux = realloc(jeux, nbJeux * sizeof(jeu));

                        sleep(2);

                        return tailleJeu;
                    }
                    
                }

            }

            // Auquel cas si le jeu n'a pas été trouvé ou qu'il n'y a aucun jeu dans la B.D.D des jeux de l'utilisateur
            return -1;
        }

        // Demande de simulation d'un combat contre l'ordinateur lui-même sur un jeu donné par l'utilisateur
        case 5:
        {
            printf("nbJeux = %d\n", nbJeux);
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){
                    
                    strcpy(memoire, jeux[i].code);

                    printf("Simulation d'un combat entre J1 et J2...\n");

                    sleep(20);

                    printf("Le gagnant du combat est : ");

                    // Si c'est le joueur 2 qui gagne
                    if(rand() % 2 == 0)
                        printf("J2 !\n");
                    else{
                        printf("J1 !\n");
                    }

                    return 0;
                }
            }

            // Si le jeu n'a pas été trouvé
            return -1;
        }

        // Demande pour lancer le jeu choisi par l'utilisateur.
        case 6:
        {
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){
                    strcpy(memoire, jeux[i].code);

                    printf("Chargement du jeu %s...\n", jeux[i].nomJeu);
                    sleep(5);
                    printf("Le jeu est lancé.\n");

                    printf("Votre tour, veuillez choisir un caractère du clavier pour continuer : ");

                    char input[1];
                    scanf("%c", input);

                    // On prétend qu'une partie de ce jeu peut durer entre 10 secondes à 1 minute.
                    sleep(rand()%60 + 10);

                    printf("Le gagnant de ce jeu est : ");
                    if(rand() % 2 == 0)
                        printf("Serveur !\n");
                    else{
                        printf("Joueur !\n");
                    }

                    return 0;
                }
            }

            // Si le jeu n'a pas été trouvé
            return -1;

        }

        // Demande non reconnu en raison du codeOp invalide. 
        default:
            return -1; // Code non reconnu
    }

}
