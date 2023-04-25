#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

int main (int argc, char** argv){

    pid_t pid;
    DIR* dossier = opendir(argv[1]);

    if(dossier == NULL){
        perror("Erreur lors de l'ouverture du dossier");
        exit(-1);
    }

    struct dirent* entree;      // Entrée du dossier
    int pidFils[1024];          // Tableau des PID des fils
    int nbFils = 0;             // Nombre de fils créés
    char chemin[1024] = "";     // Chemin du fichier

    // Parcours du dossier
    while((entree = readdir(dossier))!= NULL){

        // Si le fichier est un fichier régulier
        if(entree->d_type == DT_REG){

            // Création du chemin du fichier
            strcpy(chemin, argv[1]);
            strcat(chemin, "/");
            strcat(chemin, entree->d_name);

            fflush(stdout);
            switch (pid = fork()){
                    case -1:
                        perror("Recouvrement impossible");
                        exit(-1);

                    case 0:     // Processus fils
                        execl("./dechiffreMessage", "dechiffreMessage", chemin, argv[2], NULL);
                        perror("Recouvrement impossible");
                        exit(-1);
                        break;

                    default:    // Processus père
                        pidFils[nbFils++] = pid;    // Ajout du PID du fils au tableau
                        chemin[0] = '\0';           // Réinitialisation du chemin
                        break;
                }
        }
    }

    int pStatus;
    bool aDechiffre = false;

    for(int i = 0; i < nbFils; i++){
        waitpid(pidFils[i], &pStatus, 0);
        if(WEXITSTATUS(pStatus) == 1){
            aDechiffre = true;
        }
    }
    fflush(stdout);
    printf(aDechiffre ? "Un message a ete dechiffre avec succes\n" : "Aucun message n'a pas pu etre dechiffre\n");

}