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

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int main() {
    
    int status;
    pid_t kidPid;
    int res;

    bool isLeaving = false;

    printf("[!INFORMATION!] - Bienvenue sur votre application de jeux !\n\n");
    while(!isLeaving){
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

            if(op.codeOp == 3 || op.codeOp == 4){
                op.flag = 1;
            }

            else{
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
            }

            res = execute_demande(op);

            if(op.flag == 1){
                printf("Valeur de retour de l'opération bloquante n°%d : %d\n\n\n", op.codeOp, res);
            }
            
            // On vérifie si un thread non bloquant s'est terminé

            if(nbFilsNonBloquants > 0){
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
            }
        }
    }


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
