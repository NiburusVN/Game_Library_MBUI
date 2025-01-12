#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include "Hfichier/utils.h"
#include "Cfichier/utils.c"

// Variable globale pour l'état de l'exécution
bool estBloquant = true;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Il faut 2 arguments : l'ip ou le nom du serveur et le port.\n");
        exit(1);
    }

    char *server_ip = argv[1];
    char *server_port = argv[2];

    struct addrinfo hints, *res;
    int sockfd;

    pthread_t thread;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server_ip, server_port, &hints, &res) != 0) {
        perror("Erreur getaddrinfo");
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("Erreur création du socket");
        freeaddrinfo(res);
        exit(1);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Erreur de connexion au serveur");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);

    printf("[!INFORMATION!] Vous êtes connecté au serveur %s sur le port %s\n\n", server_ip, server_port);

    client_data_t client_data;
    client_data.sockfd = sockfd;
    client_data.running = 1; // Initialisation de running

    if (pthread_create(&thread, NULL, receive_responses, &client_data) != 0) {
        perror("Erreur création thread");
        close(sockfd);
        exit(1);
    }

    bool isLeaving = false;

    while (!isLeaving) {
        char inputChoix;
        int inputCodeOp;
        int inputFlag;
        char inputNomJeu[26] = "";
        char inputParam[100] = "";

        sleep(2);

        printf("[!INFORMATION!] - Voulez-vous formuler une demande d'opération ? ( q ) pour quitter sinon ( p ) pour poursuivre.\n\n");
        sleep(1);

        do {
            printf("Votre choix : ");
            scanf("%c", &inputChoix);
            clear_input_buffer();

            printf("\n\n\n");

            if (inputChoix == 'q') {
                // On arrête le thread en annulant explicitement
                client_data.running = 0; // Signale à l'autre thread d'arrêter
                pthread_cancel(thread); // Annule le thread de réception
                isLeaving = true;
                break;
            } else if (inputChoix == 'p') {
                demandeOperation op;

                sleep(1);
                printf("[!INFORMATION!] - Veuillez formuler votre demande pour une opération :\n- ( 1 ) Tester si le jeu demandé est présent dans votre bibliothèque\n- ( 2 ) Lister vos jeux disponibles et téléchargés dans votre bibliothèque.\n- ( 3 ) Ajouter un nouveau jeu dans votre bibliothèque.\n- ( 4 ) Supprimer le jeu demandé de votre bibliothèque.\n- ( 5 ) Simuler un combat automatique sur un jeu demandé.\n- ( 6 ) Lancer le jeu demandé.\n- ( 0 ) Quitter l'application.\n\n");

                do {
                    sleep(1);
                    printf("Votre choix : ");
                    scanf("%d", &inputCodeOp);
                    clear_input_buffer();

                    if (inputCodeOp < 0 || inputCodeOp > 6) {
                        sleep(1);
                        printf("\n\nNuméro d'opération invalide ! (RAPPEL : numéro compris entre 1 et 6)\n");
                    }
                } while (inputCodeOp < 0 || inputCodeOp > 6);

                printf("\n\n\n");

                if (inputCodeOp == 0) {
                    // On arrête le thread en annulant explicitement
                    client_data.running = 0; // Signale à l'autre thread d'arrêter
                    pthread_cancel(thread); // Annule le thread de réception
                    isLeaving = true;
                    break;
                }

                op.codeOp = inputCodeOp;

                if (inputCodeOp != 2) {
                    do {
                        sleep(1);
                        printf("Donnez le nom du jeu que vous voulez choisir pour votre opération :\n\n");
                        sleep(1);
                        printf("Votre choix : ");
                        fgets(inputNomJeu, sizeof(inputNomJeu), stdin);
                        clear_input_buffer();

                        if (inputNomJeu[strlen(inputNomJeu) - 1] == '\n') {
                            inputNomJeu[strlen(inputNomJeu) - 1] = '\0';
                        }

                        if (strlen(inputNomJeu) > 25) {
                            sleep(1);
                            printf("\n\nNom de jeu trop long ! (RAPPEL : nom du jeu doit être au plus de 25 caractères)\n");
                        }
                    } while (strlen(inputNomJeu) > 25);

                    printf("\n\n\n");

                    strcpy(op.nomJeu, inputNomJeu);

                    strcat(inputParam, "https://");
                    strcat(inputParam, urlConforme(inputNomJeu));
                    strcat(inputParam, ".com");

                    strcpy(op.param, inputParam);
                }

                if (op.codeOp == 3 || op.codeOp == 4) {
                    op.flag = 1;
                } else {
                    do {
                        sleep(1);
                        printf("Voulez-vous que votre demande d'opération soit bloquante ou non bloquante ? :\n( 0 ) Pour rendre l'opération non bloquante.\n( 1 ) Pour rendre l'opération bloquante.\n\n");

                        sleep(1);
                        printf("Votre choix : ");
                        scanf("%d", &inputFlag);
                        clear_input_buffer();

                        if (inputFlag < 0 || inputFlag > 1) {
                            sleep(1);
                            printf("\n\nNuméro choisi invalide ! (RAPPEL : 0 ou 1)\n");
                        }
                    } while (inputFlag < 0 || inputFlag > 1);

                    printf("\n\n\n");

                    op.flag = inputFlag;
                }

                // Envoyer la demande d'opération au serveur
                if (send(sockfd, &op, sizeof(op), 0) == -1) {
                    perror("Erreur envoi message");
                }

                if (op.flag == 1) {
                    char buffer[4];
                    char *message = NULL;
                    size_t totalSize = 0; // Taille totale du message reçu
                    int bytes_received;
                    while((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0){
                        buffer[bytes_received] = '\0';
                        message = realloc(message, totalSize + bytes_received);

                        // Ajouter les données reçues au message
                        memcpy(message + totalSize, buffer, bytes_received);
                        totalSize += bytes_received;
                        message[totalSize] = '\0'; // Terminer la chaîne

                        if (strchr(message, '\n') != NULL && strchr(message, '\r') != NULL) {
                            break; // On a trouvé un '\n' et on arrête
                        }
                    }
            
                    printf("\n[Réponse serveur bloquante] : %s\n", message);
                    free(message);
                    message = NULL;
                    fflush(stdout);

                    if (bytes_received == 0) {
                        printf("Le serveur a fermé la connexion.\n");
                        // On arrête le thread en annulant explicitement
                        client_data.running = 0; // Signale à l'autre thread d'arrêter
                        pthread_cancel(thread); // Annule le thread de réception
                        isLeaving = true;
                        break;
                    }
                }

                else{
                    estBloquant = false;
                }

                break;
            } else {
                printf("\n\nChoix invalide ! (RAPPEL: soit ( p ) pour continuer soit ( q ) pour quitter)\n");
            }
        } while (inputChoix != 'q' && inputChoix != 'p');
    }

    // Attendre la fin du thread
    pthread_join(thread, NULL);

    // Fermeture de la connexion
    close(sockfd);
    printf("Client déconnecté.\n");

    return 0;
}
