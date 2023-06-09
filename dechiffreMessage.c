#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#define NB_FILS 25

/**
 * @brief Affiche le nombre de caractères du message et le retourne
 * @param descr Le descripteur du fichier
 * @param nomFichier Le nom du fichier
 * @return Le nombre de caractères du message
 */
int nbCharMessage(int descr, char* nomFichier){
    int tailleMsg = 0;
    int nbCharLu = read(descr, &tailleMsg, sizeof(int));

    if(nbCharLu <= 0){
        fprintf(stderr, "ERREUR : Le fichier a mal été lu ou est vide\n");
        return -1;
    }
    return tailleMsg;
}

/**
 * @brief Affiche le nombre d'octets avant le message et le retourne
 * @param descr Le descripteur du fichier
 * @param nomFichier Le nom du fichier
 * @return Le nombre d'octets avant le message
 */
int nbOctets(int descr, char* nomFichier){
    int nbOctets = 0;
    int nbCharLu = read(descr, &nbOctets, sizeof(int));

    if(nbCharLu <= 0){
        fprintf(stderr, "ERREUR : Le fichier a mal été lu ou est vide\n");
        return -1;
    }
    return nbOctets;
}

/**
 * @brief Renvoie la taille du fichier
 * @param descr Le descripteur du fichier
 * @return La taille du fichier
 */
int getTailleFichier(int descr){
    struct stat pStats;
    fstat(descr, &pStats);

    return pStats.st_size;
}

/**
 * @brief Vérifie que le fichier est bien chiffré
 * @param descr Le descripteur du fichier
 */
bool fichierEstChiffre(int descr){
    // Lecture des deux premières lettres du fichier
    char lettreC, lettreR;
    read(descr, &lettreC, sizeof(char));
    read(descr, &lettreR, sizeof(char));
    
    // Verification du chiffrage du fichier
    return (lettreC == 'C' && lettreR == 'R');
}

/**
 * @brief Verifie que le fichier a une entete valide
 * @param descr Le descripteur du fichier
 * @param nomFichier Le nom du fichier
 * @param tailleMessage La taille du message
 * @param nbOctetsAvantMsg Le nombre d'octets avant le message
 */
void verificationFichier(int descr, char* nomFichier, int* tailleMessage, int* nbOctetsAvantMsg){
    // Vérification de l'ouverture du fichier
    if (descr == -1){
        perror("Erreur lors de l'ouverture du fichier");
        exit(-1);
    }

    // Verification du chiffrage du fichier
    if(!fichierEstChiffre(descr)){
        fprintf(stderr, "ERREUR : Le fichier %s n'est pas chiffré\n", nomFichier);
        close(descr);
        exit(-1);
    }

    *tailleMessage = nbCharMessage(descr, nomFichier);  // Lecture de la taille du message
    *nbOctetsAvantMsg = nbOctets(descr, nomFichier);    // Lecture du nombre d'octets avant le message

    // Verification de la taille du message et du nombre d'octets avant le message
    if (*nbOctetsAvantMsg < 0 || *tailleMessage < 0 || *nbOctetsAvantMsg + *tailleMessage > getTailleFichier(descr)-10){
        fprintf(stderr, "ERREUR : Le fichier %s a mal été chiffré\n", nomFichier);
        close(descr);
        exit(-1);
    }
}

/**
 * @brief Renvoie l'indice du PID dans le tableau des PID
 * @param pid Le PID à chercher
 * @param tabFils Le tableau des PID
 * @return L'indice du PID dans le tableau des PID, -1 si le PID n'est pas dans le tableau
 */
int getIndicePID(int pid, pid_t* tabFils){
    for(int i = 0; i < NB_FILS; i++){
        if(tabFils[i] == pid){
            return i;
        }
    }
    return -1;
}

int main (int argc, char** argv){

    int descr = open(argv[1], O_RDONLY);  // Ouverture du fichier en lecture seule

    int tailleMessage, nbOctetsAvantMsg;
    verificationFichier(descr, argv[1], &tailleMessage, &nbOctetsAvantMsg);
    
    lseek(descr, nbOctetsAvantMsg, SEEK_CUR);   // Deplacement du curseur au debut du message
    
    int tubes[NB_FILS][2];  // Tableau de tubes
    char tube_l[10];        // Descripteur de lecture du tube
    char decalage[3];       // Decalage du message
    pid_t tabFils[NB_FILS]; // Tableau des PID des fils

    for(int i = 0; i < NB_FILS; i++){

        pipe(tubes[i]);     // Creation du tube

        // Conversions en chaine de caractères
        sprintf(tube_l, "%d", tubes[i][0]);
        sprintf(decalage, "%d", i+1);

        // Creation du processus fils
        switch (tabFils[i] = fork()){
            case (pid_t) -1:
                perror("ERREUR: fork invalide");
                exit(-1);
                break;

            case (pid_t) 0:             // Processus fils
                close(tubes[i][1]);     // Fermeture du descripteur d'ecriture du tube
                execl("./dechiffreMessageAvecDecalage", "dechiffreMessageAvecDecalage", tube_l, argv[2], decalage, NULL);
                perror("ERREUR: recouvrement impossible");
                exit(-1);
                break;

            default:                    // Processus père
                close(tubes[i][0]);     // Fermeture du descripteur de lecture du tube
                break;
        }
    }

    // Lecture du message et ecriture dans le tube
    char lettre;
    for(int i = 0; i < tailleMessage; i++){
        read(descr, &lettre, sizeof(char));

        // Ecriture dans les tubes
        for(int j = 0; j < NB_FILS; j++){
            write(tubes[j][1], &lettre, sizeof(char));
        }
    }

    // Fermeture du descripteur d'ecriture des tubes
    for(int i = NB_FILS; i >= 0 ; i--){
        close(tubes[i][1]); 
    }

    // Attente de la fin du processus fils
    int pStatus;
    bool aTrouve = false;
    pid_t pid;

    for(int i = 0; i < NB_FILS; i++){
        pid = waitpid(0, &pStatus, 0);
        if(WEXITSTATUS(pStatus) == 1){
            fprintf(stdout, "Fichier %s déchiffré avec pour décalage %d\n", argv[1], getIndicePID(pid, tabFils)+1);
            aTrouve = true;
        }
    }

    close(descr); // Fermeture du fichier
    fflush(stdout); // Vide le buffer de sortie

    exit(aTrouve);
}