#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

/**
 * @brief Dechiffre un caractère selon le décalage
 * @param c Le caractère à déchiffrer
 * @param decalage Le décalage
 * @return char Le caractère déchiffré
 */
char dechiffrerChar(char c, int decalage){
    int decalageReel = decalage%27;
    char lettres[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int estUneLettre = 0;

    // On verifie que le caractère est une lettre ou un espace
    for(int i = 0; i < 27; i++){
        if(c == lettres[i]){
            estUneLettre = 1;
        }
    }

    /* 
    On enleve 65 pour avoir un indice compris entre 0 et 26 puis on enleve le decalage pour avoir l'indice de la lettre dechiffree puis
    on ajoute 27 pour avoir l'indice de la lettre déchiffrée puis on prend le modulo 27 pour avoir un indice compris entre 0 et 26
    */
    return (estUneLettre ? lettres[(c-65-decalageReel+27)%27] : c);
}

int main (int argc, char** argv){

    int tube_l = atoi(argv[1]); // descripteur du tube de lecture
    int tube_e = atoi(argv[2]); // descripteur du tube d'écriture
    char lettre;

    close(tube_e);  // fermeture du tube d'écriture

    // lecture des caracteres
    while(read(tube_l, &lettre, sizeof(char)) > 0){
        printf("%c", dechiffrerChar(lettre, 2));
    }
    printf("\n");

    close(tube_l);  // fermeture du tube de lecture
    return 0;
}