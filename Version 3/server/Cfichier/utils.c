#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "../Hfichier/utils.h"

// Tableau dynamique contenant des jeux et agissant en tant que "B.D.D des jeux possédés par l'utilisateur." 
extern jeu *jeux;
extern int nbJeux;

// Fonction traitant les demandes des clients dans un thread
void *execute_demande(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int new_fd = data->new_fd;
    demandeOperation op = data->op;
    free(data); // Libérer la mémoire allouée pour le thread

    printf("Opération reçue : Code %d, Jeu : %s\n", op.codeOp, op.nomJeu);

    // Allocation initiale pour text
    size_t allocated_size = 1024;
    char *text = malloc(allocated_size);
    if (!text) {
        perror("malloc");
        pthread_exit(NULL);
    }
    text[0] = '\0'; // Initialisation de la chaîne

    char memoire[1000];

    char flagStr[20];

    if(op.flag == 0)
        strcpy(flagStr, "non bloquante");

    else{
        strcpy(flagStr, "bloquante");
    }

    // Traitement de l'opération
    switch(op.codeOp){
        
        //Teste si le nom du jeu contenu dans la demande d'opération se trouve dans la B.D.D des jeux de l'utilisateur.
        case 1:
        {
            bool jeuTrouve = false;
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){
                    jeuTrouve = true;
                    snprintf(text, allocated_size, "System | opération %s n°%d | - Le jeu %s se trouve bien dans votre bibliothèque de jeu.\n\n", flagStr, op.codeOp, op.nomJeu);
                }
            }
            
            if(!jeuTrouve)
                snprintf(text, allocated_size, "System | opération %s n°%d | - Le jeu %s n'existe pas dans votre bibliothèque de jeu.\n\n\r\n", flagStr, op.codeOp, op.nomJeu);

            break;
        }

        //Affiche les jeux disponibles et téléchargés dans la B.D.D des jeux de l'utilisateur
        case 2:
        {   
            // Construction du message initial
            snprintf(text, allocated_size, "System | opération %s n°%d | - \n\n\t\t--- Votre bibliothèque de jeux téléchargés disponibles ---\n\n", flagStr, op.codeOp);

            // Si il n'y a aucun jeu dans la B.D.D, on prévient l'utilisateur et on return -1 .
            if(nbJeux == 0){
                strcat(text, "Aucun jeu disponible ni téléchargé dans la base de données de jeux.\n\n\r\n");
            }

            else{

                // Affichage soit de la liste des jeux, soit d'un message de prévention d'une nullité de jeux téléchargés.
                bool estTelechargeable = false;

                for(int i = 0; i<nbJeux; i++){

                    if(jeux[i].code != NULL){
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
                        
                        // Séparateur des noms de jeux
                        if(i != nbJeux -1)
                            strcat(text, " | ");
                        else{
                            strcat(text, "\n\n-----------------------------------------------------------------------------------------\n\n\r\n");
                        }
                    }

                }

                // si aucun jeu n'est téléchargé
                if(!estTelechargeable){
                    strcat(text, "Aucun jeu téléchargé dans la base de données de jeux.\n\n\r\n");
                }
                
            }

            break;
        }

        // Demande pour ajouter un nouveau jeu et le télécharger dans la B.D.D des jeux de l'utilisateur
        case 3:
        {
            jeu nouveauJeu;
            strcpy(nouveauJeu.nomJeu, op.nomJeu);

            // On alloue un nombre aléatoire compris entre 1 et 1000 de caractères (+1 car si jamais on se retrouve à 0 ou à 1000, 2000... voire 999)
            int tailleCode = rand() % 1000 + 1;
            nouveauJeu.code = malloc(tailleCode * sizeof(char));

            // On simule le temps de téléchargement en comblant le code avec des * et en bloquant le serveur de 10 secondes.
            memset(nouveauJeu.code, '*', tailleCode);

            nbJeux++;
            jeux = realloc(jeux, nbJeux * sizeof(jeu));
            jeux[nbJeux-1] = nouveauJeu; 

            sleep(10);

            snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Ajout du jeu terminé ! : %s\n\n\r\n", op.nomJeu, flagStr, op.codeOp, nouveauJeu.code);

            break;
        }
        
        // Demande pour supprimer un jeu choisi par l'utilisateur de sa B.D.D des jeux.
        case 4:
        {
            if(nbJeux != 0){
                bool jeuTrouve = false;
                for(int i=0; i<nbJeux; i++){

                    //Si le jeu a été trouvé.
                    if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){

                        jeuTrouve = true;
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

                        snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Désinstallation du jeu terminée ! (Taille : %dMO)\n\n\r\n", op.nomJeu, flagStr, op.codeOp, tailleJeu);
                    }
                    
                }

                // Auquel cas si le jeu n'a pas été trouvé ou qu'il n'y a aucun jeu dans la B.D.D des jeux de l'utilisateur

                if(!jeuTrouve)
                    snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Le jeu %s n'existe pas dans votre bibliothèque !\n\n\r\n", op.nomJeu, flagStr, op.codeOp, op.nomJeu); 

                }

            else{
                snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Aucun jeu disponible ni téléchargé dans votre bibliothèque !\n\n\r\n", op.nomJeu, flagStr, op.codeOp);       
            }

            break;
        }

        // Demande de simulation d'un combat contre l'ordinateur lui-même sur un jeu donné par l'utilisateur
        case 5:
        {
            bool jeuTrouve = false;
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){

                    jeuTrouve = true;
                    
                    strcpy(memoire, jeux[i].code);
    
                    snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Simulation d'un combat entre J1 et J2...\n", jeux[i].nomJeu, flagStr, op.codeOp);
                    
                    sleep(20);

                    strcat(text, "Le gagnant du combat est : ");

                    // Si c'est le joueur 2 qui gagne
                    if(rand() % 2 == 0)
                        strcat(text, "J2 !\n\n\r\n");
                    else{
                        strcat(text, "J1 !\n\n\r\n");
                    }

                }
            }


            // Auquel cas si le jeu n'a pas été trouvé ou qu'il n'y a aucun jeu dans la B.D.D des jeux de l'utilisateur

            if(!jeuTrouve)
                snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Le jeu %s n'existe pas dans votre bibliothèque !\n\n\r\n", op.nomJeu, flagStr, op.codeOp, op.nomJeu);

            break;
        }

        // Demande pour lancer le jeu choisi par l'utilisateur.
        case 6:
        {
            bool jeuTrouve = false;
            for(int i=0; i<nbJeux; i++){
                //Si le jeu a été trouvé.
                if(strcmp(op.nomJeu, jeux[i].nomJeu) == 0){

                    jeuTrouve = true;

                    strcpy(memoire, jeux[i].code);

                    snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Chargement du jeu %s ...\n", jeux[i].nomJeu, flagStr, op.codeOp, op. nomJeu);
                    sleep(5);
                    strcat(text, "Le jeu est lancé.\n");

                    // On prétend qu'une partie de ce jeu peut durer entre 10 secondes à 1 minute.
                    sleep(rand()%60 + 10);

                    strcat(text, "Le gagnant de ce jeu est : ");
                    if(rand() % 2 == 0)
                        strcat(text, "Serveur !\n\n\r\n");
                    else{
                        strcat(text, "Joueur !\n\n\r\n");
                    }
                }
            }

            // Auquel cas si le jeu n'a pas été trouvé ou qu'il n'y a aucun jeu dans la B.D.D des jeux de l'utilisateur

            if(!jeuTrouve)
                snprintf(text, allocated_size, "[%s] System |opération %s n°%d | - Le jeu %s n'existe pas dans votre bibliothèque !\n\n\r\n", op.nomJeu, flagStr, op.codeOp, op.nomJeu);
            
            break;
        }
    }


    // Envoyer la réponse au client
    if (send(new_fd, text, strlen(text), 0) == -1) {
        perror("Erreur lors de l'envoi");
    }

    printf("[THREAD %ld] Réponse envoyée : %s\n", pthread_self(), text);

    free(text);
    text = NULL;

    return NULL;   // Fin du thread
}
