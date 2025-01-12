#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>  // Pour mkfifo
#include "Hfichier/utils.h"

// Variables globales pour la gestion des processus et des jeux
int nbFilsNonBloquants = 0;
char **resultF = NULL;
int *pidF = NULL;
jeu *jeux = NULL;
int nbJeux = 0;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void signal_handler(int sig, siginfo_t *info, void *context) {
    // printf("Signal reçu: %d\n", sig);
    // printf("PID de l'émetteur: %d\n", info->si_pid);

    // Traitement des fils non bloquants
    for(int i = 0; i < nbFilsNonBloquants; i++) {
        if(pidF[i] == info->si_pid) {

            char fdStr[20];
            strcpy(fdStr, resultF[i]);
        
            //fd
            int fd = open(fdStr, O_RDONLY | O_NONBLOCK);
            if (fd == -1) {
                perror("open\n");
                exit(EXIT_FAILURE);
            }

            int result;
            read(fd, &result, sizeof(result));
            

            printf("[!INFORMATION!] - Opération fils non bloquante de PID : %d s'est terminée. Valeur de retour : %d\n", pidF[i], result);

            // Nettoyage
            if (nbFilsNonBloquants > 1) {
                pidF[i] = pidF[nbFilsNonBloquants - 1];
                strcpy(resultF[i], resultF[nbFilsNonBloquants - 1]);
            }

            free(resultF[nbFilsNonBloquants - 1]);

            nbFilsNonBloquants--;

            close(fd);

            if (unlink(fdStr) == -1) {
                perror("erreur de unlink pipeResult");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;  // Permet de recevoir des informations supplémentaires
    sa.sa_sigaction = signal_handler; 
    sigemptyset(&sa.sa_mask);

    // Associer le signal SIGUSR1 à notre gestionnaire
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    int res;

    bool isLeaving = false;

    // Lancement des différents serveurs opérations
    if (fork() == 0) {
        execlp("./Cfichier/testerServ", "./Cfichier/testerServ", NULL);
        perror("execlp testerServ");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        execlp("./Cfichier/listerServ", "./Cfichier/listerServ", NULL);
        perror("execlp listerServ");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        execlp("./Cfichier/ajouterServ", "./Cfichier/ajouterServ", NULL);
        perror("execlp ajouterServ");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        execlp("./Cfichier/supprimerServ", "./Cfichier/supprimerServ", NULL);
        perror("execlp supprimerServ");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        execlp("./Cfichier/simulerServ", "./Cfichier/simulerServ", NULL);
        perror("execlp simulerServ");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        execlp("./Cfichier/lancerServ", "./Cfichier/lancerServ", NULL);
        perror("execlp lancerServ");
        exit(EXIT_FAILURE);
    }


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

        do{
            printf("Votre choix : ");
            
            scanf("%c", &inputChoix);
            clear_input_buffer();

            printf("\n\n\n");

            if(inputChoix == 'q'){
                isLeaving = true;
                break;
            }


            else if(inputChoix == 'p'){
                demandeOperation op;

                sleep(1);
                printf("[!INFORMATION!] - Veuillez formuler votre demande pour une opération :\n- ( 1 ) Tester si le jeu demandé est présent dans votre bibliothèque\n- ( 2 ) Lister vos jeux disponibles et téléchargés dans votre bibliothèque.\n- ( 3 ) Ajouter un nouvau jeu dans votre bibliothèque.\n- ( 4 ) Supprimer le jeu demandé de votre bibliothèque.\n- ( 5 ) Simuler un combat automatique sur un jeu demandé.\n- ( 6 ) Lancer le jeu demandé.\n- ( 0 ) Quitter l'application. \n\n");


                do{
                    sleep(1);
                    printf("Votre choix : ");

                    scanf("%d", &inputCodeOp);
                    clear_input_buffer();

                    if(inputCodeOp < 0 || inputCodeOp > 6){
                        sleep(1);
                        printf("\n\nNuméro d'opération invalide ! (RAPPEL : numéro compris entre 1 et 6)\n");
                    }
                } while(inputCodeOp < 0 || inputCodeOp > 6);

                printf("\n\n\n");

                if(inputCodeOp == 0){
                    isLeaving = true;
                    break; 
                }

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

                break;
            }

            else{
                 printf("\n\nChoix invalide ! (RAPPEL: soit ( p ) pour continuer soit ( q ) pour quitter)\n");
            }
        } while(inputChoix != 'q' || inputChoix != 'p');
    }


    while (nbFilsNonBloquants > 0){
        pause();
    }

    // Libérer la mémoire allouée
    for (int i = 0; i < nbJeux; i++) {
        free(jeux[i].code);
    }
    free(jeux);
    free(pidF);
    free(resultF);

    if (unlink("pipeTester") == -1) {
        perror("erreur de unlink pipeTester");
        return -1;
    }

    if (unlink("pipeLister") == -1) {
        perror("erreur de unlink pipeLister");
        return -1;
    }

    if (unlink("pipeAjouter") == -1) {
        perror("erreur de unlink pipeAjouter");
        return -1;
    }

    if (unlink("pipeSupprimer") == -1) {
        perror("erreur de unlink pipeAjouter");
        return -1;
    }

    if (unlink("pipeSimuler") == -1) {
        perror("erreur de unlink pipeAjouter");
        return -1;
    }

    if (unlink("pipeLancer") == -1) {
        perror("erreur de unlink pipeAjouter");
        return -1;
    }

    return 0;
}
