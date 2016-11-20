#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>
#include <pthread.h>
// #include <sys/types.h>

#include "../Headers/affichages.h"
#include "../Headers/types.h"
#include "../Headers/threads.h"
#include "../Headers/initialisations.h"
#include "../Headers/pipes.h"

// gestion des signaux CTRL+C et de segmentation fault pour le pere
void gestion_signal_pere(int signum){
	// gestion de CTRl+C
	if (signum==SIGINT){
		printw("END");
		// attente d'entrée de l'utilisateur
		getch();
		// fin de la fenetre ncurses
		endwin();
		printf("\n\tVous avez interrompu le programme (CTRL + C)\n");
		kill(getpid(), SIGTERM);
	}
		// gestion de segmentation fault
	else	if (signum==SIGSEGV){
			printw("ERROR");
			// attente d'entrée de l'utilisateur
			getch();
			// fin de la fenetre ncurses
			endwin();
			printf("\n\tLe programme a tenté d'accéder à une zone non autorisée (core dumped)\n");
			kill(getpid(), SIGTERM);
	}
}

// gestion des signaux CTRL+C et de segmentation fault pour le fils
void gestion_signal_fils(int signum){
	// gestion de CTRl+C
	if (signum==SIGINT){
		printf("\n\tLe fils est arrêté (CTRL + C)\n");
		kill(getpid(), SIGTERM);
	}
		// gestion de segmentation fault
	else	if (signum==SIGSEGV){
			printf("\n\tLe fils a tenté d'accéder à une zone non autorisée (core dumped)\n");
			kill(getpid(), SIGTERM);
	}
}

//########################################
//===	fonctions de deroulement fils	===
//########################################

// attente de la réception d'une commande de la part du pere, puis interpretation
void attente_commande(Plateau* plateau, Chrono* chrono_partie, int pipe_versFils_read, int pipe_versPere_write){
	Cmd res_interpretation;
	// tant que la commande est invalide :
	do {
		// on attend input est recu du pere et interprété
		res_interpretation= interpretation_input(plateau, &chrono_partie->temps_restant, pipe_versFils_read, pipe_versPere_write);
	}while (res_interpretation==INVALIDE);
	
	// si la commande est une direction
	if (res_interpretation==BON){
		// on transmet le plateau au pere
		transmettre_plateau(plateau, chrono_partie, pipe_versPere_write);
	}
	// fin si la commande correspond à arrêt/erreur
	else {
		// en mettant le temps restant à 0, on termine la partie
		chrono_partie->temps_restant= 0;
	}
}

// debut des operations effectuées par le processus fils lourd
//	=> reception d'un input, interpretation de cet input puis modification et envoi du plateau si l'input est valide
void operations_fils(Plateau* plateau, unsigned int duree_partie, int pipe_versFils_read, int pipe_versPere_write){
	int num_tour= 0; /// utile seulement pour le debuggage, pour connaitre l'avancement dans le fichier de debuggage
	
	// attend la fin du decompte du fils
	sleep(4);
	
	// creation du chrono et lancement de celui-ci
	Chrono chrono_partie= *initialisation_chrono(duree_partie);
	pthread_t thread_chrono;
	pthread_create(&thread_chrono, NULL, defilement_chrono, (void*) &chrono_partie);
	// creation du thread de deplacements des IA
	pthread_t thread_deplacement;
	pthread_create(&thread_deplacement, NULL, deplacements_joueurs_IA, (void*) plateau);
	
	//affichage dans test.txt
	FILE *f=fopen("test.txt","a");
	fprintf(f, "Partie démarrée, temps : %d\n", chrono_partie.temps_restant);
	
	// on effecute les operations tant que la partie n'est pas finie et qu'on ne recoit pas d'ordre d'arrêt
	while (chrono_partie.temps_restant> 0){
		++num_tour;	/// on incremente le numero de tour courant, utilisé pour le debuggage
		//affichage dans test.txt
		fprintf(f, "tps restant : %d", chrono_partie.temps_restant);
		fprintf(f, "\nTour : %d\n", num_tour);
		attente_commande(plateau, &chrono_partie, pipe_versFils_read, pipe_versPere_write);
	}
	pthread_cancel(thread_deplacement);
	// ferme le fichier de debuggage
	fprintf(f, "Partie terminée\n");
	fclose(f);
}

//########################################
//===	fonctions de deroulement pere	===
//########################################

// fonction calculant quel joueur a le plus de points
int num_gagnant(unsigned int nb_joueurs, Joueur *tab_joueurs){
	int i;
	int max_points= tab_joueurs[0].nbPoints;
	int J_max= 0;
	// on parcourt tous les joueurs et on compare leur nombre de points
	for (i=1; i< nb_joueurs; i++){
		if (tab_joueurs[i].nbPoints>max_points){
			max_points= tab_joueurs[i].nbPoints;
			J_max= i;
		}
	}
	// le joueur avec le plus de points est retourné
	return J_max;
}

// le pere lance un tour de jeu
int tour(Plateau* plateau, int* temps_restant, int pipe_versFils_write, int pipe_versPere_read){
	int cmd_invalide;
	// entiers de sauvegarde de position du curseur
	int x= 0, y= 0;
	
	//recupération de la position courante du curseur, pour pouvoir y revenir et effacer ce qui a été ecrit en dessous
	getyx(stdscr, y, x);
	
	// tant que la commande de l'utilisateur est invalide
	do{
// 		transmettre_input_lock(input, pipe_versFils_write);

		// attente d'input de l'utilisateur; puis envoie au fils
		demande_input(pipe_versFils_write);
		/// exemple d'affichage de suivi de l'avancement de l'exécution, facultatifs :
		printw("j'ai transmis, attente reponse : ");
		refresh();
		// verification que la commande est valide
		cmd_invalide= reception_interpretation(pipe_versPere_read);
		if (cmd_invalide==INVALIDE){
			move(y,x);
			clrtobot();
		}
	}while (cmd_invalide==INVALIDE);//strcmp(interpretation, "OK") && run);
	// on recoit le plateau modifié par le fils
	// si on la boucle s'est arrêtée suite à une demande d'arrêt, on renvoit 0 (faux) pour arrêter l'exécution
	if (cmd_invalide==ARRET){
		printw("Dernier tour\n");
		refresh();
		return 0;
	}
	
	// si la commande est valide, on recoit un plateau de la part du fils
	if (cmd_invalide!=ARRET){
		recevoir_plateau(plateau, temps_restant, pipe_versPere_read);
	}
	printw("Fin du tour\n");
	refresh();
	return 1;
}

// début du jeu pour le père
void startGame(Plateau* plateau, unsigned int duree_partie, int pipe_versFils_write, int pipe_versPere_read){
	int num_tour= 0; int run= 1;
	// conversion du temps en secondes en time_t (micro-secondes)
	
	initialisation_ecran();
	// Départ de la partie, représente le début du mini-jeu sur la console. Avant ce point, le joueur est dans les menus.
	compteur_start();
	
	// l'affichage du jeu est démarré
	int temps_restant= duree_partie;
	while(run){
		affiche(plateau, num_tour, temps_restant);
		run= tour(plateau, &temps_restant, pipe_versFils_write, pipe_versPere_read);
		++num_tour;
		// Ces commandes sont utilisées pour le débuggage, elles indiquent l'avancement du programme
		// Les affichages sont quand meme fais, comme il n'y a pas de pause, les clear() les effacent
		// Pour gagner légérement en performance. Il faudrait les supprimmer mais on a préféré les laisser pour l'évaluation
		///printw("run=%d\n", run);
		///refresh();
		///getch();
	}
	printw("j'arrete, PRESS [ANY KEY]\n");
	refresh();
	getch();
}

void endGame(pid_t pid_fils, int winner, int nb_points, int  pipe_versPere_read){
// 	int winner= recevoir_gagnant(pipe_versPere_read);
	clear();
	printw("\n\n\n\n\n==========\t\t============");
	printw("\n\n====\tJoueur %d a gagne !!!\t====", winner+1);	
	printw("\n\n==Il a marqué %2d points, bien joué==", nb_points);//plateau->tab_joueurs[winner].nbPoints);
	printw("\n\n==========\t\t============");
	refresh();
	getch();
	printw("\n Vous allez quitter la partie");
	refresh();
	getch();
	endwin();
	kill(pid_fils,SIGTERM);
}

// argv[1] <- taille_plateau
// argv[2] <- duree_partie (secondes)
// argv[3] <- nb_joueurs
// argv[4] <- nb_joueurs_reels
int main(int argc, char *argv[]){
	srand(time(NULL));

	if (argc > 1) {
		if ( (!strcmp(argv[1], "help")) || (!strcmp(argv[1], "--help")) || (!strcmp(argv[1], "-help")) || (!strcmp(argv[1], "-h")) ){
			printf("Utilisation :\n");
			printf("./projet [taille plateau(>0)] [duree partie(secondes)] [nombre joueurs (max 5)][nombre joueurs humains]\n");
			return 0;
		}
	}

	
	unsigned int duree_partie;
	if (argc>2)
		duree_partie= atoi(argv[2]);
	else duree_partie= 30;
	
	Plateau* plateau;
	plateau= initialisation_plateau(argc, argv);	
	
	// Creation des tube
	int pipe_versPere[2];
	int pipe_versFils[2];
	if (pipe(pipe_versPere)!= 0 || pipe(pipe_versFils)!= 0) {
	      fprintf(stderr, "Erreur dans pipe\n");
	      exit(1);
	}
	
	// Creation du processus fils lourd
	pid_t pid= 0;
	pid=fork();

	//Actions du fils
	if (pid== (pid_t) 0){
		close(pipe_versFils[1]);
		close(pipe_versPere[0]);
		//affichage dans test.txt
		FILE *f=fopen("test.txt","w");
		fprintf(f, "Debut:\n");
		fclose(f);
		signal(SIGINT, gestion_signal_fils);
		signal(SIGSEGV, gestion_signal_fils);
		operations_fils(plateau, duree_partie, pipe_versFils[0], pipe_versPere[1]);
	}
	//Actions du pere
	else {
		close(pipe_versFils[0]);
		close(pipe_versPere[1]);
		signal(SIGINT, gestion_signal_pere);
		signal(SIGSEGV, gestion_signal_pere);
		//===	Attentes commandes et affichages
		startGame(plateau, duree_partie, pipe_versFils[1], pipe_versPere[0]);
		int winner= num_gagnant(plateau->nb_joueurs, plateau->tab_joueurs);
		//===	Fin du programme
		endGame(pid, winner, plateau->tab_joueurs[winner].nbPoints, pipe_versPere[0]);
	}
	
	// Libération des ressources allouée
	int i;
	free(plateau->tab_joueurs);
	for ( i=0; i<plateau->taille; i++){
		free(plateau->tab_cases[i]);
	}
	free(plateau);
	return 0;
}
