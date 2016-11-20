#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>

struct s_Position{
	int ligne;
	int colonne;
};
typedef struct s_Position Position;


struct s_Joueur{
	int couleur;
	Position emplacement;
	int nbPoints;
};
typedef struct s_Joueur Joueur;


struct s_Plateau{
	int taille;			// taille du plateau (largeur=hauteur pour le projet, possible de mettre 2 valeurs distinctes)
	int **tab_cases;		// tableau d'entier contenant les couleurs des cases
	unsigned int nb_joueurs;	// nombre de joueurs
	unsigned int nb_joueurs_reels; // nombre de joueurs reels
	Joueur *tab_joueurs;		// tableau contenant tous les joueurs de la partie
};
typedef struct s_Plateau Plateau;


struct s_Chrono{
	int duree;			// temps total du jeu en secondes
	time_t dateDebut;		// a quel moment on a démarré le jeu
	int temps_restant;		// temps restant du jeu en secondes
	pthread_mutex_t verrou;		// verrou du chrono
};
typedef struct s_Chrono Chrono;

struct s_Input{
	int* val;			// val de la touche de l'input
	pthread_mutex_t verrou;		// verrou de l'input
};
typedef struct s_Input Input;

typedef enum s_Dir{TOP, RIGHT, BOT, LEFT} Dir;

typedef enum s_Cmd{BON, INVALIDE, ARRET} Cmd;

//################################################

int cmp_position(Position pos1, Position pos2);

//################################################

//retourne le numero du joueur present, 0 le cas echéant
int joueur_present(int ligne, int colonne, unsigned int nb_joueurs, Joueur* tab_joueurs);

int deplace_joueur(Plateau* p, int num_j, Dir direction);

void deplace_IA(Plateau* plateau, int num_j);

//################################################

void copie_plateau(Plateau* ancien, Plateau* nouveau);



#endif