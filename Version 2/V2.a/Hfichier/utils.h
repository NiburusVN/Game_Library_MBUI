#pragma once
#include <stdint.h>

//structure de jeu
typedef struct jeu{
        char nomJeu[25];
        char * code;
}jeu;

//structure de demandeOperation
typedef struct demandeOperation{
        int codeOp; //Code de l'opération (cf. Sujet ; 1->Sujet.1.1 ...)
        char nomJeu[25];
        char param[200];
        int flag;
}demandeOperation;

char* urlConforme(const char* str);

//	fonction execute
//renvoie 0 si la fonction est bien terminé, sinon -1
int execute_demande(demandeOperation op);
