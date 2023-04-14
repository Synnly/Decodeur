#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

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
    char lettre, buffer;
    char mot[128] = "\0";       // Table qui stocke le mot qu'on lit
    char* motAChercher = "TEST"; // Mot à chercher
    bool aTrouveMot = false;    // Booléen qui indique si on a trouvé le mot à chercher


    close(tube_e);  // fermeture du tube d'écriture

    // lecture des caracteres
    while(read(tube_l, &lettre, sizeof(char)) > 0){

        lettre = dechiffrerChar(lettre, 2);     // On déchiffre le caractère
        printf("%c", lettre);                   // On affiche le caractère déchiffré
        buffer = (lettre == ' ' || lettre == '.' || lettre == ',' ? '\0' : lettre);  // On met le caractère \0 dans le buffer si ce n'est pas une lettre
        strcat(mot, &buffer);   // On ajoute le caractère au mot

        // On verifie si on a touvé le mot à chercher et on vide le mot
        if(buffer == '\0'){

            if(strcmp(mot, motAChercher) == 0){
                aTrouveMot = true;
            }

            mot[0] = '\0';
        }
    }
    // On affiche si on a trouvé le mot à chercher
    printf("\nLe mot %s a été trouvé : %s\n", motAChercher, (aTrouveMot ? "OUI" : "NON"));

    close(tube_l);  // Fermeture du tube de lecture
    return 0;
}