#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>  // Pour mkfifo
#include "../Hfichier/utils.h"

// Déclaration des variables globales
extern int nbFilsNonBloquants;
extern char **resultF;
extern int *pidF;
extern jeu *jeux;
extern int nbJeux;

char* urlConforme(const char* str) {
    // Allocation dynamique pour la chaîne de résultat
    char* paramConforme = malloc(strlen(str) + 1);  // +1 pour le caractère de fin de chaîne
    if (paramConforme == NULL) {
        perror("malloc");
        return NULL;
    }

    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {  // Si c'est un espace
            paramConforme[i] = '-';  // Remplacer par un tiret
        } else {
            paramConforme[i] = str[i];  // Copier le caractère dans le nom original
        }
    }

    paramConforme[i] = '\0';  // Ajouter le caractère de fin de chaîne

    return paramConforme;
}

// Fonction execute
// Renvoie 0 ou autre en fonction du bloquant ou non si la fonction est bien terminée, sinon -1
int execute_demande(demandeOperation op) {

    int result = -1;

    char idOp[10];

    sprintf(idOp, "%d", nbFilsNonBloquants);

    char pipeResultStr[20] = "pipeResult";

    int fd1;

    int fd2;

    pid_t pid = getpid();


    strcat(pipeResultStr, idOp);

    if (mkfifo(pipeResultStr, 0666) == -1) {
        perror("mkfifo\n");
        return -1;
    }

    switch (op.codeOp) {

        case 1: {

            //fd1
            fd1 = open("pipeTester", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            if (write(fd1, &pid, sizeof(pid)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }


            sleep(2);

            if (write(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
                perror("write idOp nbjeux\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            for (int i = 0; i < nbJeux; i++) {
                if(write(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                    perror("erreur write nomJeu\n");
                    close(fd1);
                    return -1;
                }
                int codeLen = strlen(jeux[i].code) + 1;
                if(write(fd1, &codeLen, sizeof(codeLen)) == -1){
                    perror("erreur write codeLen\n");
                    close(fd1);
                    return -1;
                }

                if(write(fd1, jeux[i].code, codeLen) == -1){
                    perror("erreur write jeuCode\n");
                    close(fd1);
                    return -1;
                }
            }
            
            break;
        }

        case 2: {

            //fd1
            fd1 = open("pipeLister", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            if (write(fd1, &pid, sizeof(pid)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            // ---

            sleep(2);

            if (write(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
                perror("write idOp nbjeux\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            for (int i = 0; i < nbJeux; i++) {
                if(write(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                    perror("erreur write nomJeu\n");
                    close(fd1);
                    return -1;
                }
                int codeLen = strlen(jeux[i].code) + 1;
                if(write(fd1, &codeLen, sizeof(codeLen)) == -1){
                    perror("erreur write codeLen\n");
                    close(fd1);
                    return -1;
                }

                if(write(fd1, jeux[i].code, codeLen) == -1){
                    perror("erreur write jeuCode\n");
                    close(fd1);
                    return -1;
                }
            }
            
            break;
        }

        case 3: {

            //fd1
            fd1 = open("pipeAjouter", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            // ---

            break;
        }

        case 4: {

            //fd1
            fd1 = open("pipeSupprimer", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            // ---

            sleep(2);

            if (write(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
                perror("write idOp nbjeux\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            for (int i = 0; i < nbJeux; i++) {
                if(write(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                    perror("erreur write nomJeu\n");
                    close(fd1);
                    return -1;
                }
                int codeLen = strlen(jeux[i].code) + 1;
                if(write(fd1, &codeLen, sizeof(codeLen)) == -1){
                    perror("erreur write codeLen\n");
                    close(fd1);
                    return -1;
                }

                if(write(fd1, jeux[i].code, codeLen) == -1){
                    perror("erreur write jeuCode\n");
                    close(fd1);
                    return -1;
                }
            }

            break;
        }

        case 5: {


            //fd1
            fd1 = open("pipeSimuler", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            if (write(fd1, &pid, sizeof(pid)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            // ---

            sleep(2);

            if (write(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
                perror("write idOp nbjeux\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            for (int i = 0; i < nbJeux; i++) {
                if(write(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                    perror("erreur write nomJeu\n");
                    close(fd1);
                    return -1;
                }
                int codeLen = strlen(jeux[i].code) + 1;
                if(write(fd1, &codeLen, sizeof(codeLen)) == -1){
                    perror("erreur write codeLen\n");
                    close(fd1);
                    return -1;
                }

                if(write(fd1, jeux[i].code, codeLen) == -1){
                    perror("erreur write jeuCode\n");
                    close(fd1);
                    return -1;
                }
            }

            break;
        }

        case 6: {

            //fd1
            fd1 = open("pipeLancer", O_WRONLY);
            if (fd1 == -1) {
                perror("open\n");
                return -1;
            }

            if (write(fd1, &op, sizeof(op)) == -1) {
                perror("write op\n");
                close(fd1);
                return -1;
            }

            sleep(1); // Pour que le serveur opération ait le temps de lire

            if (write(fd1, &idOp, sizeof(idOp)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            if (write(fd1, &pid, sizeof(pid)) == -1) {
                perror("write idOp\n");
                close(fd1);
                return -1;
            }

            // ---

            sleep(2);

            if (write(fd1, &nbJeux, sizeof(nbJeux)) == -1) {
                perror("write idOp nbjeux\n");
                close(fd1);
                return -1;
            }

            sleep(2);

            for (int i = 0; i < nbJeux; i++) {
                if(write(fd1, jeux[i].nomJeu, sizeof(jeux[i].nomJeu)) == -1){
                    perror("erreur write nomJeu\n");
                    close(fd1);
                    return -1;
                }
                int codeLen = strlen(jeux[i].code) + 1;
                if(write(fd1, &codeLen, sizeof(codeLen)) == -1){
                    perror("erreur write codeLen\n");
                    close(fd1);
                    return -1;
                }

                if(write(fd1, jeux[i].code, codeLen) == -1){
                    perror("erreur write jeuCode\n");
                    close(fd1);
                    return -1;
                }
            }

            break;
        }

        default:{
            return -1;
        }

    }

    //fd2
    fd2 = open(pipeResultStr, O_RDONLY);
    if (fd2 == -1) {
        perror("open\n");
        return -1;
    }

    if(op.flag == 0){

        read(fd2, &result, sizeof(result));

        // Ajouter le PID du fils à la liste des fils non bloquants
        nbFilsNonBloquants++;

        pidF = realloc(pidF, nbFilsNonBloquants * sizeof(int));
        if (!pidF) {
            perror("realloc");
            return -1;
        }

        // On enregistre le pid fils du serveur opération qui s'occupe de la demande
        pidF[nbFilsNonBloquants - 1] = result;


        resultF = realloc(resultF, nbFilsNonBloquants * sizeof(char *));
        if (!resultF) {
            perror("realloc");
            return -1;
        }

        resultF[nbFilsNonBloquants - 1] = malloc(20*sizeof(char));

        strcpy(resultF[nbFilsNonBloquants - 1], pipeResultStr);

        printf("[!INFORMATION!] - Opération (demande n°%d) fils non bloquante de PID : %d s'est lancée en arrière-plan. Vous pouvez continuer vos autres tâches !\n\n\n", op.codeOp, result);

    }


    else{

        if(op.codeOp == 3){
            char memoire[1000];
            read(fd2, &memoire, sizeof(memoire));

            // On crée un nouveau jeu et on l'ajoute au tableau jeux
            jeu nouveauJeu;
            strcpy(nouveauJeu.nomJeu, op.nomJeu);
            nouveauJeu.code = malloc((strlen(memoire) + 1) * sizeof(char));
            if (!nouveauJeu.code) {
                perror("malloc");
                return -1;
            }
            strcpy(nouveauJeu.code, memoire);

            nbJeux++;
            jeux = realloc(jeux, nbJeux * sizeof(jeu));
            if (!jeux) {
                perror("realloc");
                return -1;
            }
            jeux[nbJeux - 1] = nouveauJeu;
            
            read(fd2, &result, sizeof(result));
        }

        else if(op.codeOp == 4){
            int index;
            read(fd2, &index, sizeof(index));

            // Si jamais result a bien l'index du jeu auquel on cherche à supprimer
            if (index != -1) {
                int tailleJeu = jeux[index].code ? strlen(jeux[index].code) : 0;
                free(jeux[index].code);
                for (int e = index; e < nbJeux - 1; e++) {
                    jeux[e] = jeux[e + 1];
                }
                nbJeux--;
                jeux = realloc(jeux, nbJeux * sizeof(jeu));
                result = tailleJeu;
            }
        }

        else{
            read(fd2, &result, sizeof(result));
        }


        close(fd2);
        if (unlink(pipeResultStr) == -1) {
            perror("erreur de unlink pipeResult");
            return -1;
        }
    }
    
    close(fd1);

    return result;
}
