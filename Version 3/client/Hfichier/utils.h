#pragma once
#include <stdint.h>

// Structure partagée entre threads
typedef struct {
    int sockfd;
    int running; // On garde la variable pour signaler au thread de continuer ou non
} client_data_t;

typedef struct {
    int codeOp;
    char nomJeu[25];
    char param[200];
    int flag;
} demandeOperation;

void *receive_responses(void *arg);

char* urlConforme(const char* str);

void clear_input_buffer();