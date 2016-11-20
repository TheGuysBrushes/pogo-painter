#include "../Headers/types.h"

//########################################
//===	fonctions sur les types		===
//########################################

//retourne un booleen indiquant si les 2 positions sont identiques
int cmp_position(Position pos1, Position pos2){
	return ( pos1.ligne==pos2.ligne && pos1.colonne==pos2.colonne );
}


//########################################
//===	fonctions sur les joueurs	===
//########################################

//retourne le numero du joueur present, 0 le cas echéant
int joueur_present(int ligne, int colonne, unsigned int nb_joueurs, Joueur* tab_joueurs){
	int k;
	//cree une position à comparer ac celle joueur
	Position pos_test;
	pos_test.ligne= ligne;
	pos_test.colonne= colonne;
	
	// on verifie si il y a un joueur sur la case
	for ( k= nb_joueurs; k>0; --k){
			if ( cmp_position(pos_test, tab_joueurs[k-1].emplacement) )
				return k;
	}
	return 0;
}

int deplace_joueur(Plateau* p, int num_j, Dir direction){
	int* ligne_J=	&(p->tab_joueurs[num_j].emplacement.ligne);
	int* colonne_J=	&(p->tab_joueurs[num_j].emplacement.colonne);
	// on cree les coordonnées où le joueur désire aller
	int new_ligne=	(*ligne_J);
	int new_colonne= (*colonne_J);
	//change la pos du joueur si possible
	if ( direction==TOP && new_ligne!=0 ){
		new_ligne--;
	}
	else if (direction==RIGHT && new_colonne+1<p->taille){
		new_colonne++;
	}
	else if (direction==BOT && new_ligne+1<p->taille){
		new_ligne++;
	}
	else if (direction==LEFT && new_colonne!=0){
		new_colonne--;
	}
	else return 0; //direction invalide
	// on verifie qu'il n'y a pas deja un joueur sur les nouvelles coordonnées
	if ( !joueur_present(new_ligne, new_colonne, p->nb_joueurs,p->tab_joueurs) ){
		// si il n'y a personne, on fait effectivement le deplacement
		*ligne_J=	new_ligne;
		*colonne_J=	new_colonne;
		
		// change la couleur de la case où va le joueur (les cases sont donc coloriées à l'arrivée)
		p->tab_cases[*ligne_J][*colonne_J]= p->tab_joueurs[num_j].couleur;
		return 1; 
	}
	return 0; // le joueur n'a pas pu changer de direction
}

void deplace_IA(Plateau* plateau, int num_j){
	int dir_random= rand()%4;
	int num_tentative= 1;
	// on essai les 4 directions de deplacement
	while ( !deplace_joueur(plateau, num_j, dir_random) && num_tentative<=4){
		dir_random=(dir_random+1)%4;
		num_tentative++;
	}
}
