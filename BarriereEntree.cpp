/*************************************************************************
                           BarriereEntree  -  description
                             -------------------
    début                : BarriereEntree
    e-mail               : BarriereEntree
*************************************************************************/

//---------- Réalisation de la tâche <BarriereEntree> (fichier BarriereEntree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <Outils.h>

//------------------------------------------------------ Include personnel
#include "BarriereEntree.h"

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
void BarriereEntree(int canal[],int sem_ecran, int sem_placeLibre, int mp_nbPlace, int mp_placesParking)
{
	struct sigaction masqueFin;
	masqueFin.sa_handler = SIG_IGN;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);

	struct sigaction masqueSigusr2;
	masqueSigusr2.sa_handler = FinT;
	sigemptyset(&masqueSigusr2.sa_mask);
	masqueSigusr2.sa_flags = 0;
	sigaction(SIGUSR2,&masqueSigusr2,NULL);

	close(canal[1]);

	for(;;)
	{
		Voiture voiture;
		read(canal[0],&voiture,sizeof(Voiture));
		
		char message[255];
		char integer[2];

		strcpy(message,"Une voiture est arrivée à la barrière ");
		sprintf(integer,"%d",voiture.barriere);
		strcat(message,integer);
		strcat(message," !");
		Afficher(MESSAGE,message);
	}

} //----- fin de BarriereEntree

