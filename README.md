TP 5 : Miniprojet Systèmes
==========================

Compilation
-----------

Pour compiler le programme vous avez deux méthodes à votre disposition

### Methode 1 : À la main

#### Ordre de compilation du programme à la racine du dossier :

    gcc dechiffreMessageAvecDecalage.c -o dechiffreMessageAvecDecalage
    gcc dechiffreMessage.c -o dechiffreMessage
    gcc main.c -o main

### Methode 2 : En utilisant le makefile

#### Commande à lancer à la racine du dossier :

    make
ou

    make main

Utilisation du programme
------------------------

### Commande

$ ./main <chemin> <mot>

### Paramètres
**chemin** : le chemin du dossier où se situe le(s) message(s) à déchiffrer de la forme: dossier/.../dossier

**mot** le mot à chercher en lettres CAPITALES

Exemple d'utilisation et d'affichage
------------------------------------

    $ make  
    $ ./main TestEtMessagesCodes/MessagesCodes FERNANDESDOSSANTOS  
    FELICITATIONS VOUS AVEZ TROUVE LE CODE, VOTRE NOM EST CERTAINEMENT FERNANDESDOSSANTOS ET VOUS AVEZ DECRYPTE CE MESSAGE BRAVO  
    Fichier TestEtMessagesCodes/MessagesCodes/10.cod déchiffré avec pour décalage 16  
    Un message a ete dechiffre avec succes

Autres
------

Le makefile contient aussi deux autres commandes utiles:

    $ make clean //permet de supprimer les fichiers executables créés  
    $ make test  //permet de rapidement lancer les fichiers de test

Fernandes dos Santos Emanuel | Numéro étudiant : 32105861 | Groupe 3