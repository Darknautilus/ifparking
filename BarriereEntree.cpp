/*************************************************************************
                           BarriereEntree  -  description
                             -------------------
    début                : BarriereEntree
    e-mail               : BarriereEntree
*************************************************************************/

//---------- Réalisation de la tâche <BarriereEntree> (fichier BarriereEntree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <map>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <Outils.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/errno.h>

//------------------------------------------------------ Include personnel
#include "BarriereEntree.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static bool exited = false;
static std::map<pid_t,Voiture> voituriers;
static Voiture voitCourante;
static int lastNumVoiture = 0;

//------------------------------------------------------ Fonctions privées
static void FinVoiturier(int signal)
{
	if(!exited)
	{
		int status;
		pid_t voiturier = wait(&status);
		if(WIFEXITED(status))
		{
			int numPlace = WEXITSTATUS(status);
			Voiture voit = voituriers[voiturier];
			AfficherPlace(numPlace,voit.type,voit.num,time(NULL));
			voituriers.erase(voiturier);
		}
	}
}

static void FinT(int signal)
{
	exited = true;
	for(std::map<pid_t,Voiture>::iterator it = voituriers.begin(); it != voituriers.end(); ++it)
	{
		kill(it->first,SIGUSR2);
	}
	voituriers.clear();
	exit(0);
}

static int getNumVoiture()
{
	return lastNumVoiture = (lastNumVoiture % 999) + 1;
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

	struct sigaction masqueFinVoit;
	masqueFinVoit.sa_handler = FinVoiturier;
	sigemptyset(&masqueFinVoit.sa_mask);
	masqueFinVoit.sa_flags = 0;
	sigaction(SIGCHLD,&masqueFinVoit,NULL);

	close(canal[1]);

	Voiture voiture;
	int readRet;
	do
	{
		while((readRet = read(canal[0],&voiture,sizeof(Voiture))) > 0)
		{
			voiture.num = getNumVoiture();

			DessinerVoitureBarriere(voiture.barriere,voiture.type);
			// vérifications
			pid_t voiturier = GarerVoiture(voiture.barriere);
			voituriers.insert(make_pair(voiturier,voiture));
		}
	}
	while(readRet == -1 && errno == EINTR);

} //----- fin de BarriereEntree

