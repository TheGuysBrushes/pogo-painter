# pogo-painter
Une réplique du mini-jeu Pogo Painter, du jeu Crash Bash. Programmé en C, avec des forks, threads, pipes et la bibliothèques ncurses


## Implémentation :
	Pour afficher des couleurs et pouvoir placer le curseur d'écriture, on utilise la bibliothèque ncurses.

Les paramètres du programme sont les arguments de la ligne de commande d'exécution. 
Le détail des arguments est avant la fonction main, dans le fichier projet.c.
Si il n'y a pas d'arguments, des valeurs par défaut sont attribuées.
Le makefile est dans le dossier Sources.

Les types de données utilisées et leurs fonctions sont de les fichiers types.h et types.c

Toutes les fonctions d'initalisiation des données sont dans le fichier initialisations.c
Les fonctions pour afficher le plateau sont dans affichages.c
Les fonctions d'echanges par tubes et de threads sont dans les fichiers pipes.c et threads.c .

	Les couleurs des cases du plateau sont stockées sous forme d'un tableau d'entiers à deux dimensions.
Les joueurs sont représentés par une Position sur le plateau, une un entier représentant la couleur lui appartenant et sont nombre de points, qui est le nombre de cases du plateau de sa couleur.
La taille de plateau est la hauteur et la largeur du plateau.
Le nombre de joueurs réels et les fonctions qui prennent une hauteur et une largeur,  permettent une évolution du code simplifiée. Comme on veut un plateau carré, les fonctions reçoivent une hauteur et une largeur égales.

	On déplace le joueur  gràce au touches directionnels et on arrète le programme grace à Backspace.
Il peut y avoir de 1 à 5 joueurs réels ou non. Pour qu'il y ait plus de joueurs réels qui jouent, il faudrait attribuer des touches de déplacements pour chaque joueur réel. Si il y a plus d'un joueur réel, seul le joueur 1 peut jouer. Seul les numéros des joueurs réels sont affichés
Si une commande est incorrecte, il faut redemander une à l'utilisateur 

	On utilise deux tubes pour communiquer dans les deux sens.
Le déplacement des joueurs IA et du chronomètre sont effectués dans un thead car s'exécutant en parallèle des autres opérations qui dépendent de l'interaction de l'utilisateur.
Dans le déplacement d'IA, on utilise getimeofday() pour prendre en compte le temps d'exécution et que les joueurs se déplacent toutes les 500 ms exactement
Les joueurs choisissent une case aléatoirement, si elle n'est pas disponible, il essaie les autres cases.
Le nombre de points du joueur est le nombre de cases de sa couleur, dont la case où il se trouve. 

## Autres :
	Dans le code, les commentaires après “///” indiquent les moyens de déboggage, à retirer avant la compilation finale destinée à être distribuée.
On aurait put utiliser des fonction "accesseurs" pour les joueurs ( get_ligne, get_colonne) pour alléger le code.
On avait prévu d'utiliser le thread attente_input en mettant seulement à jour le dernier input et en n'envoyant une commande au fils que tous les 0.5 secondes, pour que le joueur ne puisse pas jouer plus souvent que l'IA. Mais cela n'était pas assez proche du sujet.
Les choix d'utiliser une seule touche pour déplacer le joueur a été fait car on trouvait cela plus pratique à utiliser et plus proche du jeu vidéo original.

## Règles :
On peut choisir n'importe quelle taille de plateau meme si cela pose probleme lors l'affichage ou des incohérences (plusieurs joueurs sur la même case, c'est a l'utilisateur de choisir des valeurs ni trop grandes ni trop petites
De plus la taille de la fenêtre n'est pas modulable selon la taille du plateau 
Comme indiqué dans le sujet, l'affichage n'est mis à jour que lorsque l'utilisateur entre une commande de déplacement. Donc le déplacement des personnages contrôlés par l'ordinateur n'est mis à jour qu'après que le joueur se soit déplacé.

## Auteurs et Contributeurs

[<img alt="Florian David" src="https://avatars0.githubusercontent.com/u/11854849" width="140">](https://flodavid.github.io) |  [<img alt="Rodolphe Charlot" src="https://avatars2.githubusercontent.com/u/11831152" width="140">](https://github.com/caminin) |
-------------------------------------|----------------------------------------|----------------------|
[@flodavid](https://flodavid.github.io) | [@caminin](https://github.com/caminin) |


Projet développé par [Florian David](https://github.com/flodavid) et [Rodolphe Charlot](https://github.com/caminin).

