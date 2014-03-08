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
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
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
static Voiture *zone_placesParking;

static struct sembuf reserver = {0,-1,0};
static struct sembuf liberer = {0,1,0};


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
			voit.numPlace = numPlace;
			zone_placesParking[numPlace] = voit;
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
		waitpid(it->first,NULL,0);
		
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

	zone_placesParking = (Voiture*) shmat(mp_placesParking,NULL,0);
	
	Voiture voiture;
	int readRet;
	do
	{
		while((readRet = read(canal[0],&voiture,sizeof(Voiture))) > 0)
		{
			voiture.num = getNumVoiture();
			voiture.arrivee = time(0);  // pour enregistrer l'heure d'arrivée
			DessinerVoitureBarriere(voiture.barriere,voiture.type);
			// vérifications
			semop(sem_placeLibre,&reserver,1); //demande s'il y a de la place sinon faut depose requete
	
			pid_t voiturier = GarerVoiture(voiture.barriere);
			voituriers.insert(make_pair(voiturier,voiture));
		}
	}
	while(readRet == -1 && errno == EINTR);

} //----- fin de BarriereEntree

