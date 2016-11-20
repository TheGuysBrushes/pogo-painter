#include <pthread.h>
#include <unistd.h>

#include "types.h"

// inutilisé
struct s_Partie{
	Plateau* plateau;
	Chrono chrono;
};
typedef struct s_Partie Partie;

void* deplacements_joueurs_IA(void* v_Plateau);//v_Partie);

void* defilement_chrono(void* v_chrono);