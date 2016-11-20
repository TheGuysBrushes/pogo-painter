#include "../Headers/affichages.h"

//#########################################
//===	fonctions d'affichage		====
//#########################################

// ecrit la premiere/derniere ligne du plateau, avec le numéros de colonnes
void affiche_ligne_contour(int largeur){
	int j;
	printw("| ");
	for(j=0; j<largeur; j++)
		if (j < 10)
			printw("|=%d=", j);
		else 	printw("|%d=", j);
	printw("||\n");
}

// ecrit un separation entre les lignes du plateau
void affiche_ligne_separation(int largeur){
	int j;
	printw("| ");
	for(j=0; j<largeur; j++)
		printw("|---");
	printw("||\n");
}

// afficher un joueur à l'ecran, dans sa couelur, avec son numéro, à l'emplacement du curseur
void affiche_joueur(Joueur J, unsigned int nb_joueurs_reels, int num_joueur){
	printw("| ");
	attron(COLOR_PAIR(J.couleur));
	// si le joueur est un joueur réel, on affiche son numéro, sinon on affiche une étoile
	if (num_joueur<nb_joueurs_reels)
		printw("%d", num_joueur+1);
	else 	printw("*");
	attroff(COLOR_PAIR(J.couleur));
	printw(" ");
}

// affiche une case vide, dans sa couleur
void affiche_case(int couleur_case){
	if (couleur_case){
		printw("| ");
		attron(COLOR_PAIR(couleur_case));
		//mvprintw(ligne*2+1, colonne*4+3, "X");
		printw(" ");
		attroff(COLOR_PAIR(couleur_case));
		printw(" ");
	}
	else printw("|   ", couleur_case);// 3 espaces
}

// ecirt une ligne de cases avec son numéro; utilise les fonctions affiche_case et affiche_joueur
void affiche_ligne_contenu(int largeur, int *ligne_cases, int ligne, unsigned int nb_joueurs, unsigned int nb_joueurs_reels, Joueur *tab_joueurs){
	int j;	//colonne courante
	int num_j_present;	//booleen indiquant si un joueur est present sur  la case courante
	if (ligne < 10)
		printw("|%d", ligne);
	else 	printw("%d", ligne);
	// verifie si il y un joueur sur la case, l'affiche si oui
	for(j=0; j<largeur; j++){
		// on verifie si il y a un joueur sur la case
		if (( num_j_present= joueur_present(ligne, j, nb_joueurs, tab_joueurs) ))
			affiche_joueur(tab_joueurs[num_j_present-1], nb_joueurs_reels, num_j_present-1);	
		else affiche_case(ligne_cases[j]);
	}
	printw("||\n");
}

// TODO : faire condition : si 2*taille> 3*plateau->nb_joueurs; alors afficher les points sur 2 colonnes pour éviter les chevauchements
// affiche le nombre de points de chaque joueur
// -> on déplace le curseur avant pour les afficher à côté du plateau, ensuite il faut le replacer
void affiche_points(Plateau* p, int num_tour, int temps_restant){
	int i;
	// on initialise le nombre de cases coloriées à la taille² du plateau
	int nb_cases_vides= p->taille*p->taille;
	// on compte le nombre de cases de chaque joueur
	for (i=0; i< p->nb_joueurs; i++){
		// le nombre de points du joueur est écrit dans sa couleur
		attron(COLOR_PAIR(i+1));
		mvprintw(i*3, p->taille*4+6, "Joueur %d : %2d",i+1, p->tab_joueurs[i].nbPoints );
		// utilise remet les couleurs par défaut
		attroff(COLOR_PAIR(i+1));
		// on soustrait le nombre de points du joueur courant au nombre de cases coloriées
		nb_cases_vides -= p->tab_joueurs[i].nbPoints;
	}
	// affiche du nombre de cases non coloriées, du numero de tour et du temps restant en dessous des points
	mvprintw(i*3-1, p->taille*4+6, "Tour : %d ; Cases vides : %d", num_tour, nb_cases_vides );
	mvprintw(i*3, p->taille*4+6, "Temps restant : %d\n", temps_restant);
	refresh();
}

// affichage du nouveau plateau
void affiche(Plateau* p, int num_tour, int temps_restant){
	// l'ecran est effacé
	clear();
	// on place le curseur au debut de l'écran
	move(0, 0);
	// premiere ligne du plateau
	affiche_ligne_contour(p->taille);
	
	// affiche le contenu du plateau avec les séparations entre
	int i;
	for(i=0; i<p->taille-1; i++){
		affiche_ligne_contenu(p->taille, p->tab_cases[i], i, p->nb_joueurs, p->nb_joueurs_reels, p->tab_joueurs);
		affiche_ligne_separation(p->taille);
	}
	// affiche la dernière ligne de contenu puis 
	affiche_ligne_contenu(p->taille, p->tab_cases[i], i, p->nb_joueurs, p->nb_joueurs_reels, p->tab_joueurs);
	affiche_ligne_contour(p->taille);

	// RAZ des nombre de points de chaque joueur
	for (i=0; i< p->nb_joueurs; i++)
		p->tab_joueurs[i].nbPoints=0;

	// calcul dont nombre de point de chaque joueur selon le nombre de case de sa couleur
	for (i=0; i<p->taille; i++){
		int j;
		for (j=0; j<p->taille; j++){
			//on verifie que la case est coloriee
			if (p->tab_cases[i][j])
				//on ajoute un point au joueur
				p->tab_joueurs[p->tab_cases[i][j]-1].nbPoints++;
		}
	}
	// affichage des nouveaux point puis replacement du curseur
	affiche_points(p, num_tour, temps_restant);
	move(p->taille*2+3, 0);
}

// compteur avant le demarrage de la partie
// la fonction est largement simplifiable (caractères stockés dans un fichier)
void compteur_start(){
	int taille_numX= 5;
	int taille_numY= 4;
	int taille_startX= 32;
	int taille_startY= 5;
	int x, y;
	getmaxyx(stdscr, y, x);
	int pos_debX= x/2-taille_numX;
	int pos_debY= y/2-taille_numY;
	clear();
	mvprintw( pos_debY+1, pos_debX,  "  33333  \n");
	mvprintw( pos_debY+2, pos_debX,  "  3   33 \n");
	mvprintw( pos_debY+3, pos_debX,  "      33 \n");
	mvprintw( pos_debY+4, pos_debX,  "      33 \n");
	mvprintw( pos_debY+5, pos_debX,  "    3333 \n");
	mvprintw( pos_debY+6, pos_debX,  "      33 \n");
	mvprintw( pos_debY+7, pos_debX,  "      33 \n");
	mvprintw( pos_debY+8, pos_debX,  "   33333 \n");
	mvprintw( pos_debY+9, pos_debX,  "  33333 \n");
	refresh();
	sleep(1);
	clear();
	mvprintw( pos_debY+1, pos_debX,  "  22222  \n");
	mvprintw( pos_debY+2, pos_debX,  "  2   22 \n");
	mvprintw( pos_debY+3, pos_debX,  "      22 \n");
	mvprintw( pos_debY+4, pos_debX,  "      22 \n");
	mvprintw( pos_debY+5, pos_debX,  "   2222  \n");
	mvprintw( pos_debY+6, pos_debX,  "  2222   \n");
	mvprintw( pos_debY+7, pos_debX,  "  2      \n");
	mvprintw( pos_debY+8, pos_debX,  "  22222  \n");
	mvprintw( pos_debY+9, pos_debX,  "  222222 \n");
	refresh();
	sleep(1);
	clear();
	mvprintw( pos_debY+1, pos_debX,  "   11  \n");
	mvprintw( pos_debY+2, pos_debX,  "  111  \n");
	mvprintw( pos_debY+3, pos_debX,  "   11  \n"); 
	mvprintw( pos_debY+4, pos_debX,  "   11  \n");
	mvprintw( pos_debY+5, pos_debX,  "   11  \n");
	mvprintw( pos_debY+6, pos_debX,  "   11  \n");
	mvprintw( pos_debY+7, pos_debX,  "   11  \n");
	mvprintw( pos_debY+8, pos_debX,  "  1111 \n");
	mvprintw( pos_debY+9, pos_debX,  "  1111\n");
	refresh();
	sleep(1);
	clear();
	pos_debX= x/2-taille_startX;
	pos_debY= y/2-taille_startY;
	mvprintw(pos_debY, pos_debX,  "      rtstartst rtstartstart sta     rtstartst   startstart   \n");
	mvprintw(pos_debY+1, pos_debX,  "     rtstartst rtstartstart star    rtstartstar startstarts   \n");
	mvprintw( pos_debY+2, pos_debX,  "      sta           tst   ts arts    st    tart      tar      \n");
	mvprintw( pos_debY+3, pos_debX,  "        rt          sta   st  tst    ta     rts      art      \n");
	mvprintw( pos_debY+4, pos_debX,  "         st         tar   ta   ta    ar     ts       rts      \n");
	mvprintw( pos_debY+5, pos_debX,  "           rt       art  tarts art   rt    tst       tst      \n");
	mvprintw( pos_debY+6, pos_debX,  "           tst      rts  artsta ts   ts   tst        sta      \n");
	mvprintw( pos_debY+7, pos_debX,  "     tstartsta      tst  rt     st   st   sta        tar      \n");
	mvprintw( pos_debY+8, pos_debX,  "    tstartstar      sta rt      tar  ta    arts      art      \n");
	mvprintw( pos_debY+9, pos_debX,  "                    ta                               rt       \n");
	refresh();
	sleep(1);
}
