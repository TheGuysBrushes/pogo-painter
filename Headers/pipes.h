#ifndef PIPES_H
#define PIPES_H

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <pthread.h>

#include "types.h"
#include "affichages.h"

void transmettre_input(int input, int desc_write);

void transmettre_input_lock(Input* input, int desc_write);

void demande_input(int desc_write);

int recevoir_input(int desc_read);

//################################################

void transmettre_temps(time_t temps, int desc_write);

void recevoir_temps(int* temps, int desc_read);

//################################################

void transmettre_message(char* message, int desc_write);

char* recevoir_message(char* message, int desc_read);

//################################################

void transmettre_tab_cases(int taille, int** tab_cases, int desc_write);

void transmettre_tab_joueurs(unsigned int nb_joueurs, Joueur* tab_joueurs, int desc_write);

void transmettre_plateau(Plateau* plateau, Chrono* chrono_partie, int desc_write);

//################################################

void recevoir_tab_cases(int taille, int** tab_cases, int desc_read);

void recevoir_tab_joueurs(unsigned int nb_joueurs, Joueur* tab_joueurs, int desc_read);

void recevoir_plateau(Plateau* plateau, int* temps_restant, int desc_read);

//################################################

void transmettre_gagnant(int gagnant, int desc_write);

int recevoir_gagnant(int desc_read);

//################################################

int interpretation_input(Plateau* plateau, int* temps_restant, int pipe_versFils, int pipe_versPere);

int reception_interpretation(int desc_read);

#endif