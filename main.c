#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

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

int main (int argc, char** argv){

    int descr = open(argv[1], O_RDONLY);  // Ouverture du fichier en lecture seule

    // Vérification de l'ouverture du fichier
    if (descr == -1){
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Lecture des deux premières lettres du fichier
    char lettreC, lettreR;
    read(descr, &lettreC, sizeof(char));
    read(descr, &lettreR, sizeof(char));
    
    // Verification du chiffrage du fichier
    if(lettreC == 'C' && lettreR == 'R'){
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


    close(descr); // Fermeture du fichier

}