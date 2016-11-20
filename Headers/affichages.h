#ifndef AFFICHAGES_H
#define AFFICHAGES_H

#include <ncurses.h>
#include <unistd.h>

#include "types.h"

void affiche_ligne_contour(int largeur);

void affiche_ligne_separation(int largeur);

void affiche_joueur(Joueur J, unsigned int nb_joueurs_reels, int num_joueur);

void affiche_case(int couleur_case);

void affiche_ligne_contenu(int largeur, int *ligne_cases, int ligne, unsigned int nb_joueurs, unsigned int nb_joueurs_reels, Joueur *tab_joueurs);

void affiche_points(Plateau* p, int num_tour, int temps_restant);

void affiche(Plateau* plateau,  int num_tour, int temps_restant);

void compteur_start();

#endif