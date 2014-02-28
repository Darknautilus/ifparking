/*************************************************************************
                           GererClavier  -  description
                             -------------------
    début                : GererClavier
    e-mail               : GererClavier
*************************************************************************/

//---------- Réalisation de la tâche <GererClavier> (fichier GererClavier.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

//------------------------------------------------------ Include personnel
#include "GererClavier.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static void FinT(int signal)
{
	exit(0);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GererClavier(void)
{
	struct sigaction masqueFin;
	masqueFin.sa_handler = FinT;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);

	for(;;)
		Menu();
} //----- fin de GererClavier

void Commande(char code, unsigned int valeur)
{
	switch(code)
	{
		case 'Q':
			FinT(0);
			break;
		case 'P':
			printf("Prof porte %d",valeur);
			break;
		case 'A':
			printf("Autre porte %d",valeur);
			break;
		case 'S':
			printf("Sortie place %d",valeur);
			break;
		default:
			break;
	}
} //----- fin de Commande
