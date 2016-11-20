#include <sys/time.h>

#include "../Headers/threads.h"

void* defilement_chrono(void* v_chrono){
	// "cast" de l'argument
	Chrono* chrono=(Chrono*) v_chrono;
	// on effectue des MAJ du temps restant jusqu'à ce qu'il n'y en ait plus => fin du jeu
	while(chrono->temps_restant > 0.0){
		time_t temps;
		time(&temps);
		// verrouillage des valeurs du chrono
		pthread_mutex_lock(&chrono->verrou);
		// maj du temps restant avant la fin du jeu ( duree- (temps ecoulé) )
		chrono->temps_restant= chrono->duree -difftime(temps, chrono->dateDebut);
		// déverrouillage des valeurs du chrono
		pthread_mutex_unlock(&chrono->verrou);
		// on attend avant de remettre à jour pour ne pas faire trop d'opérations
		// le temps restant est donc précis à 0,25 seconde pret
		usleep(250000);
	}
	pthread_exit(NULL);
}

//deplace tous les joueurs IA, toutes les 0.5 secondes
void* deplacements_joueurs_IA(void* v_Plateau){
	Plateau* plateau= (Plateau*) v_Plateau;
	struct timeval tval_before, tval_after, tval_result;
	while (1){
		// on recupre à quel moment on à commencé le déplacement des joueurs
		gettimeofday(&tval_before, NULL);

		// parcours des joueurs non réels
		int j;
		for ( j= plateau->nb_joueurs_reels; j< plateau->nb_joueurs ; j++){
			// deplace le joueur non réel courant, de manière aléatoire sur une des cases disponibles
			deplace_IA(plateau, j);
		}
		// on calcule le temps écoulé pendant les déplacements avec le temps de début des déplacements et celui de fin
		gettimeofday(&tval_after, NULL);
		timersub(&tval_after, &tval_before, &tval_result);
		// on attend jusqu'a ce que le temps pour deplacer le joueurs + le temps de pause soit égale à 0.5 s
		usleep(500000-(long int)tval_result.tv_usec);
	}
	pthread_exit(NULL);
}


/// thread d'entrees de la part du joueur réel
/// non inutilisé
#include <ncurses.h>
void* attente_input(void* v_input){
	Input* input= (Input *) v_input;
	
	while(1){
		printw("Touches de deplacement ? ; BACKSPACE pour quitter\n\t");
		int tmp= getch();
		pthread_mutex_lock(&input->verrou);
		*input->val= tmp;
		pthread_mutex_unlock(&input->verrou);
	}
	pthread_exit(NULL);
}// mettre un pthread_cancel pour arrêter ?
