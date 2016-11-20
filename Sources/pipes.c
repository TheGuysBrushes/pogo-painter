#include "../Headers/pipes.h"

#define BUFFER_SIZE 256

// ################################################
// ####	Fonctions d'echanges d'input		####
// ################################################

// transmission d'un input (du pere vers le fils)
void transmettre_input(int input, int desc_write){
	write (desc_write, &input, sizeof(int));
}

// transmission d'un input avec verrou mutex (du pere vers le fils)
// l'input est remis à zéros ensuite pour ne pas redéplacer le personnage si le joueur n'a pas fait de nouvel input
/// non utilisé
void transmettre_input_lock(Input* input, int desc_write){
	// verrouillage d'input pendant l'envoi et la RAZ
	pthread_mutex_lock(&input->verrou);
	write (desc_write, &input->val, sizeof(int));
	input->val= 0;
	// deverrouillage d'input
	pthread_mutex_unlock(&input->verrou);
}

// demande d'un input à l'utilisateur puis envoi
void demande_input(int desc_write){
	printw("Touches de deplacement ? ; ou BACKSPACE pour quitter\n\t");
	int input= getch();
	transmettre_input(input, desc_write);
}

// reception d'un input
int recevoir_input(int desc_read){
	int input;
	read(desc_read, &input, sizeof(int));
	return input;
}

// ################################################
// ====	fonctions d'echanges du temps		====
// ################################################

// permet d'envoyer le temps restant au pere
void transmettre_temps(time_t temps, int desc_write){
	// conversion du temps en secondes
	int buf= ((int) temps);
	write (desc_write, &buf, sizeof(float));
}

// permet de recevoir le temps restant du fils
void recevoir_temps(int* temps, int desc_read){
	read(desc_read, temps, sizeof(int));
	printw("Temps recu : %d de taille : %d\n", *temps, sizeof(*temps));	
}

// ################################################
// ====	fonctions d'echanges messages		====
// ################################################

// permet de transmettre un message
void transmettre_message(char* message , int desc_write){	
	write (desc_write, &message, sizeof(message));
}

// reception d'un message
char* recevoir_message(char* message, int desc_read){
	// l'ancienne adresse est d'abord désallouée
	if (message!=NULL){
		free(message);
		message= NULL;
	}
	// puis on lit la nouvelle et on la retourne
	read(desc_read, &message, sizeof(message));
	printw("msg recu : -%s- de taille : %d\n", message, sizeof(message));
	return message;
}


// ################################################
// ####	Fonctions d'envoi d'un plateau		####
// ################################################

void transmettre_tab_cases(int taille, int** tab_cases, int desc_write){
	int i;
	// transmission ligne par ligne, car le tableau est alloué ligne par ligne
	for ( i=0; i< taille; ++i){
		write(desc_write, tab_cases[i], taille*sizeof(int));
	}
}

// 
void transmettre_tab_joueurs(unsigned int nb_joueurs, Joueur* tab_joueurs, int desc_write){
	int i;
	for ( i=0; i< nb_joueurs; ++i){
		write(desc_write, &tab_joueurs[i].emplacement.ligne , sizeof(int));
		write(desc_write, &tab_joueurs[i].emplacement.colonne , sizeof(int));
		write(desc_write, &tab_joueurs[i].nbPoints , sizeof(int));
	}
}

void transmettre_plateau(Plateau* plateau, Chrono* chrono_partie, int desc_write){	
	// envoi du temps restant
	pthread_mutex_lock(&chrono_partie->verrou);
	transmettre_temps(chrono_partie->temps_restant, desc_write);
	pthread_mutex_unlock(&chrono_partie->verrou);
	// envoi des couleurs des cases
	transmettre_tab_cases(plateau->taille, plateau->tab_cases, desc_write);
	// envoi des joueurs
	transmettre_tab_joueurs(plateau->nb_joueurs, plateau->tab_joueurs, desc_write);
}


// ################################################
// ####	Fonctions de réception d'un plateau	####
// ################################################

// reception d'un tableau de cases
void recevoir_tab_cases(int taille, int** tab_cases, int desc_read){
	int i;
	// reception ligne par ligne, car le tableau est alloué ligne par ligne
	for ( i=0; i< taille; ++i){
		read(desc_read, tab_cases[i], taille*sizeof(int));
	}
}

// reception des joueurs
void recevoir_tab_joueurs(unsigned int nb_joueurs, Joueur* tab_joueurs, int desc_read){
	int i;
	for ( i=0; i< nb_joueurs; ++i){
		// on recoit d'abord la ligne et colonne du joueur pour avoir sa position
		read(desc_read, &tab_joueurs[i].emplacement.ligne , sizeof(int));
		read(desc_read, &tab_joueurs[i].emplacement.colonne , sizeof(int));
		// on recoit ensuite le nombre de points de ce joueur
		read(desc_read, &tab_joueurs[i].nbPoints , sizeof(int));
	}
}

// reception du plateau contenu le tableau de cases 
void recevoir_plateau(Plateau* old, int* temps_restant, int desc_read){
	printw("Attente du temps restant\n");
	refresh();
	// reception du temps restant
	recevoir_temps(temps_restant, desc_read);
	
	// modification du tableau de cases et celui de joueurs en recevant ceux du fils
	printw("Attente tab_cases\n");
	refresh();
	recevoir_tab_cases(old->taille, old->tab_cases, desc_read);
	
	printw("Attente tab_joueurs\n");
	refresh();
	recevoir_tab_joueurs(old->nb_joueurs, old->tab_joueurs, desc_read);
}

// ################################################
// ####	Fonctions d'echanges du gagnant	####
// ################################################

// permet de transmettre le numéro du gagnant
void transmettre_gagnant(int gagnant, int desc_write){
	FILE *f=fopen("test.txt","a");
	fprintf(f, "Ecriture de : %d\n", gagnant);
	int taille_lue= write(desc_write, &gagnant, sizeof(int));
	fprintf(f, "Taille ecrite : %d\n", taille_lue);
	fclose(f);
}

// permet de recevoir le numéro du gagnant
int recevoir_gagnant(int desc_read){
	int buf= 0;
	int taille_recu= read(desc_read, &buf, sizeof(int));
	printw("taille lue : %d", taille_recu);
	refresh();
	return buf;
}

// ################################################
// ####	Fonctions d'echanges d'interpretation	####
// ################################################

// interpretation d'un input recu et renvoi du message correspondant
int interpretation_input(Plateau* plateau, int* temps_restant,  int pipe_versFils, int pipe_versPere){
	int input;
	int num_j= 0;	//joueur 1
	Cmd validite= BON; // on suppose que la commande qui sera recu est correcte, on modifie la valeur de "validite" sinon
	
	// on recoit la commande rentré par l'utilisateur et on l'interprete
	input= recevoir_input(pipe_versFils);
	
	if (*temps_restant<=0){
		transmettre_message("FIN", pipe_versPere);
	}
	else{
		if (input==KEY_UP){
			deplace_joueur(plateau, num_j, TOP);
		}
		else
		if (input==KEY_RIGHT){
			deplace_joueur(plateau, num_j, RIGHT);
		}
		else
		if (input==KEY_DOWN){
			deplace_joueur(plateau, num_j, BOT);
		}
		else
		if (input==KEY_LEFT){
			deplace_joueur(plateau, num_j, LEFT);
		}
		else
		// on envoi un message au pere qui dépend du résultat d'interpretation
		if (input==KEY_BACKSPACE){
			transmettre_message("STOP", pipe_versPere);
			validite= ARRET;
		}
		else {
			transmettre_message("INVALIDE", pipe_versPere);
			validite= INVALIDE;
		}
	}
	if (validite==BON){
		transmettre_message("OK", pipe_versPere);
	}
	return validite;
}

// reception du message de l'interpretation faite par le fils
int reception_interpretation(int desc_read){
	Cmd cmd= BON;
	// on recoit l'interpretation
	char* interpretation= NULL;
	interpretation= recevoir_message(interpretation, desc_read);
	if (!strcmp(interpretation, "OK")){
		printw("commande correcte\n");
		refresh();
	}
	else
	if (!strcmp(interpretation, "INVALIDE")){
		cmd= INVALIDE;
		refresh();
		printw("la commande saisie est invalidite\n");
	}
	else {
		// lors d'un cas d'arret, on signale l'arret du programme et on avertit l'utilisateur de la raison
		cmd= ARRET;
		if (!strcmp(interpretation, "FIN")){
			printw("Fin de la partie\n");
			refresh();
		}
		else
		if (!strcmp(interpretation, "STOP")){
			printw("Vous avez interrompu le programme (FIN)\n");
			refresh();
		}
		else {
			printw("Probleme d'interpretation\n");
			refresh();
		}
	}
	return cmd;
}
	
