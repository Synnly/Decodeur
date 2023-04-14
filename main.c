#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

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
        printf("ERREUR : Le fichier a mal été lu ou est vide\n");
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
        printf("ERREUR : Le fichier a mal été lu ou est vide\n");
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

int main (int argc, char** argv){

    int descr = open(argv[1], O_RDONLY);  // Ouverture du fichier en lecture seule

    // Vérification de l'ouverture du fichier
    if (descr == -1){
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Verification du chiffrage du fichier
    if(fichierEstChiffre(descr)){
        printf("Le fichier %s est bien un fichier chiffré\n", argv[1]);
    }
    else{
        printf("ERREUR : Le fichier %s n'est pas chiffré\n", argv[1]);
        close(descr);
        exit(EXIT_FAILURE);
    }
    
    int tailleMessage = nbCharMessage(descr, argv[1]);  // Lecture de la taille du message
    int nbOctetsAvantMsg = nbOctets(descr, argv[1]);    // Lecture du nombre d'octets avant le message

    // Verification de la taille du message et du nombre d'octets avant le message
    if (nbOctetsAvantMsg < 0 || tailleMessage < 0 || nbOctetsAvantMsg + tailleMessage > getTailleFichier(descr)-10){
        printf("ERREUR : Le fichier a mal été chiffré\n");
        close(descr);
        exit(EXIT_FAILURE);
    }
    
    lseek(descr, nbOctetsAvantMsg, SEEK_CUR);   // Deplacement du curseur au debut du message

    // Creation des tubes
    int tubes[2];
    pipe(tubes);  

    // Conversion des descripteurs du tube en chaine de caractères
    char tube_e[10], tube_l[10];
    sprintf(tube_l, "%d", tubes[0]);
    sprintf(tube_e, "%d", tubes[1]);

    // Creation du processus fils
    pid_t pid;
    switch (pid = fork()){
        case (pid_t) -1:
            perror("ERREUR: fork invalide");
            exit(EXIT_FAILURE);
            break;

        case (pid_t) 0:     // Processus fils
            execl("./dechiffreMessage", "dechiffreMessage", tube_l, tube_e, argv[2], argv[3], NULL);
            perror("ERREUR: recouvrement impossible");
            exit(EXIT_FAILURE);
            break;

        default:            // Processus père
            break;
    }

    close(tubes[0]);    // Fermeture du descripteur de lecture du tube

    // Lecture du message et ecriture dans le tube
    char lettre;
    for(int i = 0; i < tailleMessage; i++){
        read(descr, &lettre, sizeof(char));
        write(tubes[1], &lettre, sizeof(char));
    }

    close(tubes[1]);    // Fermeture du descripteur d'ecriture du tube

    // Attente de la fin du processus fils
    int pStatus;
    wait(&pStatus);
    if(WEXITSTATUS(pStatus)){
        printf("Fichier %s déchiffré avec pour décalage %s\n", argv[1], argv[3]);
    }

    close(descr); // Fermeture du fichier

    return 0;
}