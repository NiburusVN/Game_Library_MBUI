#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include "../Hfichier/utils.h"

extern bool estBloquant;

// Fonction exécutée par le thread pour recevoir les réponses
void *receive_responses(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    char buffer[2];
    int bytes_received;


    while (data->running) {
        char *message = NULL;
        size_t totalSize = 0; // Taille totale du message reçu

        if(!estBloquant){
            while((bytes_received = recv(data->sockfd, buffer, sizeof(buffer) - 1, 0)) > 0){
                message = realloc(message, totalSize + bytes_received + 1);

                memcpy(message + totalSize, buffer, bytes_received);
                totalSize += bytes_received;
                message[totalSize] = '\0'; // Terminer la chaîne

                if (strchr(message, '\n') != NULL && strchr(message, '\r') != NULL) {
                    break; // On a trouvé un '\n' et on arrête
                }
            }
    
            printf("\n[Réponse serveur non bloquante] : %s\n", message);
            free(message);
            message = NULL;
            estBloquant = false;
            fflush(stdout);
            

            if (bytes_received == 0) {
                printf("\n[INFO] Le serveur a fermé la connexion.\n");
                data->running = 0;
                break;
            }
        }
    }

    return NULL;
}

char* urlConforme(const char* str) {
    char* paramConforme = malloc(strlen(str) + 1);
    if (paramConforme == NULL) {
        perror("malloc");
        return NULL;
    }

    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            paramConforme[i] = '-';
        } else {
            paramConforme[i] = str[i];
        }
    }

    paramConforme[i] = '\0';
    return paramConforme;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}