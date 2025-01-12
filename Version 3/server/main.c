#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "Hfichier/utils.h"
#include "Cfichier/utils.c"

jeu *jeux = NULL;
int nbJeux = 0;

int main(int argc, char *argv[]){

    if (argc != 2) {
        printf("Il faut 1 argument : le numéro du port.\n");
        exit(1);
    }

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *res; // Pointeur de résultat
    int status, sockfd, new_fd;

    memset(&hints, 0, sizeof hints); // Initialiser tous les attributs de la structure addrinfo
    hints.ai_family = AF_UNSPEC;     // Indifférent si c'est du IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // Comble l'adresse IP

    if ((status = getaddrinfo("127.0.0.1", argv[1], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // Création du socket

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Échec de l'attachement");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) == -1) { // Écoute sur le socket
        perror("Échec de l'écoute");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute...\n");

    // Acceptation des connexions
    while (1) {
        addr_size = sizeof their_addr;
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) < 0) {
            perror("Échec de l'acceptation");
            continue; // Continue à accepter d'autres connexions
        }

        printf("Client connecté, en attente de l'opération...\n");
        demandeOperation op;
        // Recevoir la demande d'opération du client
        while (1) {

            int bytes_received = recv(new_fd, &op, sizeof(op), 0);
            if (bytes_received > 0) {

                // Thread qui va traiter la demande de l'opération
                pthread_t thread_id;
                ThreadData *data = malloc(sizeof(ThreadData));
                data->new_fd = new_fd;
                data->op = op;

                if (pthread_create(&thread_id, NULL, execute_demande, data) != 0) {
                    perror("Erreur lors de la création du thread");
                    close(new_fd);
                } 

            }
            
            else if (bytes_received == 0) {
                printf("Client a fermé la connexion.\n");
                break;
            } else {
                perror("Erreur réception");
                break;
            }

        }

        close(new_fd); // On ferme la connexion après chaque demande traitée
    }

    close(sockfd); // On ferme le serveur
    return 0;
}
