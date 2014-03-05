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
#include <unistd.h>

//------------------------------------------------------ Include personnel
#include "GererClavier.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int barriere1[2];
static int barriere2[2];
static int barriere3[2];

//------------------------------------------------------ Fonctions privées
static void FinT(int signal)
{
	exit(0);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GererClavier(int pBarriere1[], int pBarriere2[], int pBarriere3[])
{
	struct sigaction masqueFin;
	masqueFin.sa_handler = FinT;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);

	for(int i=0;i<2;i++)
	{
		barriere1[i] = pBarriere1[i];
		close(barriere1[0]);
		barriere2[i] = pBarriere2[i];
		close(barriere2[0]);
		barriere3[i] = pBarriere3[i];
		close(barriere3[0]);
	}

	for(;;)
		Menu();
} //----- fin de GererClavier

void Commande(char code, unsigned int valeur)
{
	int desc;
	Voiture voit;
	switch(code)
	{
		case 'Q':
			FinT(0);
			break;
		case 'P':
			voit.type = PROF;
			if(valeur == 1)
			{
				desc = barriere1[1];
				voit.barriere = PROF_BLAISE_PASCAL;
			}
			else
			{
				desc = barriere3[1];
				voit.barriere = ENTREE_GASTON_BERGER;
			}
			write(desc,&voit,sizeof(Voiture));
			break;
		case 'A':
			voit.type = AUTRE;
			if(valeur == 1)
			{
				desc = barriere2[1];
				voit.barriere = AUTRE_BLAISE_PASCAL;
			}
			else
			{
				desc = barriere3[1];
				voit.barriere = ENTREE_GASTON_BERGER;
			}
			write(desc,&voit,sizeof(Voiture));
			break;
		case 'S':
			printf("Sortie place %d",valeur);
			break;
		default:
			break;
	}
} //----- fin de Commande
