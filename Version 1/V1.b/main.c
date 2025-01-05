#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "Hfichier/utils.h"
#include "Cfichier/utils.c"

// Variables globales pour la gestion des processus et des jeux
jeu *jeux = NULL;
int nbJeux = 0;

threadResult* threadsNonBloquants = NULL;
int nbThreadsNonBloquants = 0;

char memoire[1000];

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main() {
    
    bool isLeaving = false;

    printf("[!INFORMATION!] - Bienvenue sur votre application de jeux !\n\n");
    while(!isLeaving){
        pthread_t thread;
        char inputChoix;
        int inputCodeOp;
        int inputFlag;
        char inputNomJeu[26] = "";
        char inputParam[100] = "";

        sleep(2);
        printf("[!INFORMATION!] - Voulez-vous formuler une demande d'opération ? ( q ) pour quitter sinon ( p ) pour poursuivre.\n\n");

        sleep(1);
        printf("Votre choix : ");

        scanf("%c", &inputChoix);
        clear_input_buffer();

        printf("\n\n\n");

        if(inputChoix == 'q'){
            isLeaving = true;
        }


        else{
            demandeOperation op;

            sleep(1);
            printf("[!INFORMATION!] - Veuillez formuler votre demande pour une opération :\n- ( 1 ) Tester si le jeu demandé est présent dans votre bibliothèque\n- ( 2 ) Lister vos jeux disponibles et téléchargés dans votre bibliothèque.\n- ( 3 ) Ajouter un nouvau jeu dans votre bibliothèque.\n- ( 4 ) Supprimer le jeu demandé de votre bibliothèque.\n- ( 5 ) Simuler un combat automatique sur un jeu demandé.\n- ( 6 ) Lancer le jeu demandé.\n\n");


            do{
                sleep(1);
                printf("Votre choix : ");

                scanf("%d", &inputCodeOp);
                clear_input_buffer();

                if(inputCodeOp < 1 || inputCodeOp > 6){
                    sleep(1);
                    printf("\n\nNuméro d'opération invalide ! (RAPPEL : numéro compris entre 1 et 6)\n");
                }
            } while(inputCodeOp < 1 || inputCodeOp > 6);

            printf("\n\n\n");

            op.codeOp = inputCodeOp;

            if(inputCodeOp != 2){


                do{
                    sleep(1);
                    printf("Donnez le nom du jeu que vous voulez choisir pour votre opération :\n\n");

                    sleep(1);
                    printf("Votre choix : ");

                    fgets(inputNomJeu, sizeof(inputNomJeu), stdin);
                    clear_input_buffer();

                    if (inputNomJeu[strlen(inputNomJeu) - 1] == '\n') {
                        inputNomJeu[strlen(inputNomJeu) - 1] = '\0';  
                    }


                    if(strlen(inputNomJeu) > 25){
                        sleep(1);
                        printf("\n\nNom de jeu trop long ! (RAPPEL : nom du jeu doit être au plus de 25 caractères)\n");
                    }
                } while(strlen(inputNomJeu) > 25);

                printf("\n\n\n");

                strcpy(op.nomJeu, inputNomJeu);

                strcat(inputParam, "https://");
                strcat(inputParam, urlConforme(inputNomJeu));
                strcat(inputParam, ".com");

                strcpy(op.param, inputParam);
            }

            do{
                sleep(1);
                printf("Voulez-vous que votre demande d'opération soit bloquante ou non bloquante, c'est-à-dire en arrière-plan ? :\n( 0 ) Pour rendre l'opération non bloquante.\n( 1 ) Pour rendre l'opération bloquante.\n\n");

                sleep(1);
                printf("Votre choix : ");
                scanf("%d", &inputFlag);
                clear_input_buffer();

                if(inputCodeOp < 1 || inputCodeOp > 6){
                    sleep(1);
                    printf("\n\nNuméro choisi invalide ! (RAPPEL : 0 ou 1)\n");
                }
            } while(inputCodeOp < 1 || inputCodeOp > 6);

            printf("\n\n\n");

            op.flag = inputFlag;


            pthread_create(&thread, NULL, execute_demande, &op);

            if(op.flag == 1){
                void* result;
                pthread_join(thread, &result);
                sleep(1);
                printf("Valeur de retour de l'opération bloquante n°%d: %d\n\n\n", op.codeOp, *(int*)result);
            }
            

            // On vérifie si un thread non bloquant s'est terminé

            if(nbThreadsNonBloquants > 0){
                // Transformer peut-être en fct
                for(int i=0; i<nbThreadsNonBloquants; i++){
                    if(threadsNonBloquants[i].estFini){

                        sleep(1);
                        printf("[!INFORMATION!] - Opération thread non bloquante de TID : %d s'est terminée. Valeur de retour : %d\n\n\n", threadsNonBloquants[i].tid, threadsNonBloquants[i].result);
                        if(nbThreadsNonBloquants != 1){
                            threadsNonBloquants[i] = threadsNonBloquants[nbThreadsNonBloquants - 1];
                        }

                        threadsNonBloquants[i].estFini = true;
                        nbThreadsNonBloquants--;

                    }
                }
            }
        }
    }

    do{

        // Transformer peut-être en fct
        for(int i=0; i<nbThreadsNonBloquants; i++){
            if(threadsNonBloquants[i].estFini){

                threadsNonBloquants[i].estFini = true;

                sleep(1);
                printf("[!INFORMATION!] - Opération thread non bloquante de TID : %d s'est terminée. Valeur de retour : %d\n\n\n", threadsNonBloquants[i].tid, threadsNonBloquants[i].result);

                if(nbThreadsNonBloquants != 1){
                    threadsNonBloquants[i] = threadsNonBloquants[nbThreadsNonBloquants - 1];
                }

                nbThreadsNonBloquants--;

            }
        }
    } while (nbThreadsNonBloquants > 0);


    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);
    free(threadsNonBloquants);

    return 0;
}
