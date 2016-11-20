#include "../Headers/initialisations.h"

//#########################################
//===	fonctions d'initialisation	====
//#########################################

// initialisation du tableau de cases à partir d'une hauteur et d'une largeur
// j'utilise une taille commune pour le projet, mais il serait facile d'utiliser un plateau rectangulaire si besoin
int** initialisation_tab_cases(int taille_vtcl, int taille_hztl){
	int **tab_cases;
	tab_cases= (int**)malloc(sizeof(int)*taille_vtcl);
	int i= 0;
	for ( ; i<taille_vtcl; i++){
		int j= 0;
		tab_cases[i]= (int*)malloc(sizeof(int)*taille_hztl);
		for ( ; j<taille_hztl; j++){
			// on initialise toutes les cases à 0 (pas de couleur);
			tab_cases[i][j]= 0;
		}
	}
	return tab_cases;
}

// initialisation du tableau de joueurs à partir de hauteur et d'une largeur pour les placer dans les coins voir au centre
Joueur* initialisation_joueurs(int taille_vtcl, int taille_hztl, unsigned int nb_joueurs){
	Joueur* tab_joueurs;
	tab_joueurs=(Joueur*)malloc(nb_joueurs*sizeof(Joueur));
	int i;
	// les couleurs sont assignées aux joueurs
	for (i=0; i< nb_joueurs; i++){
		tab_joueurs[i].couleur= i+1;
		tab_joueurs[i].nbPoints= 1;
	}
	// on place le nombre de joueurs indiqué sur le plateau (5 positions prédéfinies)
	switch (nb_joueurs){
		// place le 5e joueur au centre
		case 5 : tab_joueurs[4].emplacement.ligne=	taille_vtcl/2;
			tab_joueurs[4].emplacement.colonne=	taille_hztl/2;
		// place le 4e joueur en bas à droite
		case 4 : tab_joueurs[3].emplacement.ligne=	taille_vtcl-1;
			tab_joueurs[3].emplacement.colonne=	taille_hztl-1;
		// place le 3e joueur en haut à gauche
		case 3 : tab_joueurs[2].emplacement.ligne=	0;
			tab_joueurs[2].emplacement.colonne=	taille_hztl-1;
		// place le 2e joueur en bas à gauche
		case 2 : tab_joueurs[1].emplacement.ligne=	taille_vtcl-1;
			tab_joueurs[1].emplacement.colonne=	0;
		// place le 1e joueur en haut à gauche
		case 1 : tab_joueurs[0].emplacement.ligne=	0;
			tab_joueurs[0].emplacement.colonne=	0;
	}
	// les cases occupées sont coloriées lors de l'initialiation du plateau
	return tab_joueurs;
}

Plateau* initialisation_plateau(int argc, char *argv[]){
	// allocation d"un nouveau plateau
	Plateau* new_plateau=	(Plateau*)malloc(sizeof(Plateau));
	new_plateau->taille= 8;		// taille par defaut du plateau	
	
	// utilisation des arguments lors de l'appel du programme pour initialiser les éléments utiles
	// si il y a 3 arguments au moins, on définit la taille du plateau; ici hauteur=largeur=taille
	if (argc>1)
		new_plateau->taille= atoi(argv[1]);
	// si il y a 3 arguments au moins, on définit le nombre de joueurs total
	if (argc> 3) {
		// si on ne demande pas plus de 5 joueurs
		if (atoi(argv[3])< 6 && atoi(argv[3])> 0)
			new_plateau->nb_joueurs= atoi(argv[3]);
		else {
			// necessite la relance du programme avec un argument different
			printf("Veuillez entrer un nombre de joueurs compris entre 1 et 5 svp");
			exit(1);
		}
	}
	else new_plateau->nb_joueurs= 4;
	
	// si il y a 4 arguments au moins, on définit le nombre de joueurs réels
	if (argc>4){
		new_plateau->nb_joueurs_reels= atoi(argv[4]);
		// si on demande plus de joueurs reels que de joueurs au total
		if ( atoi(argv[3]) < new_plateau->nb_joueurs_reels ){
			// retour d'erreur et arrêt du programme
			fprintf(stderr, "\tRelancez en entrant mois de joueurs reels que de joueurs\n");
			exit(1);
		}
// 		printf("%d Joueurs\n", nb_joueurs_reels);
	}
	else new_plateau->nb_joueurs_reels= 1;

	// initialiation du tableau de cases et du tableau de joueurs du plateau
	new_plateau->tab_cases=		initialisation_tab_cases(new_plateau->taille, new_plateau->taille);
	new_plateau->tab_joueurs=	initialisation_joueurs(new_plateau->taille, new_plateau->taille, new_plateau->nb_joueurs);
	
	// coloriage des cases où les joueurs sont placés au debut de la partie
	int i;
	for (i= 0; i< new_plateau->nb_joueurs; ++i){
		Joueur tmp= new_plateau->tab_joueurs[i];
		new_plateau->tab_cases[tmp.emplacement.ligne][tmp.emplacement.colonne]= tmp.couleur;
	}
	return new_plateau;
}

// initalisation du chrono à partir d'une duree entière, en secondes
Chrono* initialisation_chrono(unsigned int duree){
	Chrono* new_chrono= (Chrono*)malloc(sizeof(Chrono));
	
	// assignation de la duree totale, elle est ensuite constante 
	new_chrono->duree= duree;
	// le temps restant est décrementé dans un thread grâce au verrou
	new_chrono->temps_restant= duree;
	time(&new_chrono->dateDebut);
	// initialisation du verrou
	pthread_mutex_init(&new_chrono->verrou, NULL);
	return new_chrono;
}

// Inutilisé, destiné modifié la commande entrée dans un thread, pour n'envoyer au fils que le dernier input
Input* initialisation_input(){
	Input* input= (Input*) malloc(sizeof(Input));;
	input->val= (int*) malloc(sizeof(int));
	*input->val= 0;
	pthread_mutex_init(&input->verrou, NULL);
	return input;
}

// démarre le mode ncurses pour l"affichage
void initialisation_ecran(){
	// lance le mode curses et le mode keypad pour les touches clavier
	initscr();
	keypad(stdscr, 1);
	
	// initialise le mode couleur de la fenetre si disponible
	if(!has_colors())
	{	endwin();
		printw("Your terminal does not support color\n");
		exit(1);
	}
	start_color();
	
	//initalise les paires couleurs, utilisées pour afficher les cases coloriées en couleur
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	init_pair(3, COLOR_BLACK, COLOR_BLUE);
	init_pair(4, COLOR_BLACK, COLOR_GREEN);
	init_pair(5, COLOR_BLACK, COLOR_CYAN);
}
