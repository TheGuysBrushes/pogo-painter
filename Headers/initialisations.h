#ifndef INITIALISATIONS_H
#define INITIALISATIONS_H

#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include "types.h"

int** initialisation_tab_cases(int taille_vtcl, int taille_hztl);	

Joueur* initialisation_joueurs(int taille_vtcl, int taille_hztl, unsigned int nb_joueur);

Plateau* initialisation_plateau(int argc, char *argv[]);

Chrono* initialisation_chrono(unsigned int duree);

Input* initialisation_input();

void initialisation_ecran();

#endif