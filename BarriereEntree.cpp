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

static int sem_ecran;

static ress_critique lastNumVoiture;
static ress_critique requetes;

static Voiture *placesParking;

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

			placesParking[numPlace] = voit;

			semop(sem_ecran,&reserver,1);
			AfficherPlace(numPlace,voit.type,voit.num,voit.arrivee);
			semop(sem_ecran,&liberer,1);

			voituriers.erase(voiturier);
		}
	}
}

static void FinT(int signal)
{
	exited = true;
	pid_t voiturier;
	for(std::map<pid_t,Voiture>::iterator it = voituriers.begin(); it != voituriers.end(); ++it)
	{
		kill(it->first,SIGUSR2);
		do
		{
			voiturier = waitpid(it->first,NULL,0);
		}
		while(voiturier == -1 && errno == EINTR);
	}
	voituriers.clear();
	shmdt(placesParking);
	exit(0);
}

// op : true, setter
// 			false, getter
static Requete *reqctl(unsigned short int indice, Requete *req, bool op)
{
	if(indice < 0 || indice > REQ_GASTON)
	{
		return NULL;
	}
	else
	{
		struct sembuf reserver = {indice,-1,0};
		struct sembuf liberer = {indice,1,0};
		semop(requetes.sem,&reserver,1);
		// MAJ de la mémoire partagée
		Requete *tabReq = (Requete*)shmat(requetes.mem,NULL,0);
		if(op)
		{
			tabReq[indice].type = req->type;
			tabReq[indice].barriere = req->barriere;
			tabReq[indice].arrivee = req->arrivee;
		}
		else
		{
			req->type = tabReq[indice].type;
			req->barriere = tabReq[indice].barriere;
			req->arrivee = tabReq[indice].arrivee;
		}
		shmdt(tabReq);
		semop(requetes.sem,&liberer,1);
		return req;
	}
}

static int getNumVoiture()
{
	int num;
	semop(lastNumVoiture.sem,&reserver,1);
	int *numVoiture = (int*)shmat(lastNumVoiture.mem,NULL,0);
	num = *numVoiture = ((*numVoiture % 999) + 1);
	shmdt(numVoiture);
	semop(lastNumVoiture.sem,&liberer,1);
	return num;
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereEntree(int canal[], ress_critique numVoiture, ress_critique pRequetes, int sem_ecran, int sem_placeLibre, int mp_nbPlace, int mp_placesParking)
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

	lastNumVoiture = numVoiture;
	requetes = pRequetes;

	placesParking = (Voiture*) shmat(mp_placesParking,NULL,0);

	close(canal[1]);
	
	Voiture voiture;
	int readRet;
	do
	{
		while((readRet = read(canal[0],&voiture,sizeof(Voiture))) > 0)
		{
			voiture.num = getNumVoiture();
			voiture.arrivee = time(NULL);  // pour enregistrer l'heure d'arrivée

			unsigned short int indReq = TypeBarriereToReqId(voiture.barriere);

			int nbPlaces = semctl(sem_placeLibre,0,GETVAL,0);
			if(!nbPlaces)
			{
				// Mise en place de la requête
				Requete req = {voiture.type,voiture.barriere,voiture.arrivee};
				reqctl(indReq,&req,true);

				// Affichage de la requête à l'écran
				semop(sem_ecran,&reserver,1);
				AfficherRequete(voiture.barriere,voiture.type,voiture.arrivee);
				semop(sem_ecran,&liberer,1);
			}
			
			DessinerVoitureBarriere(voiture.barriere,voiture.type);
			semop(sem_placeLibre,&reserver,1); // attente d'une place

			if(!nbPlaces)
			{
				// Suppression de la requête
				Requete emptyReq = {AUCUN,AUCUNE,0};
				reqctl(indReq,&emptyReq,true);

				semop(sem_ecran,&reserver,1);
				Effacer((TypeZone)(((int)REQUETE_R1)+indReq));
				semop(sem_ecran,&liberer,1);
				// Fin suppression requête
			}

			pid_t voiturier = GarerVoiture(voiture.barriere);
			voituriers.insert(make_pair(voiturier,voiture));
			sleep(1);
		}
	}
	while(readRet == -1 && errno == EINTR);

} //----- fin de BarriereEntree

