/// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

//---------- Réalisation de la tâche <BarriereSortie> (fichier BarriereSortie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <string>
#include <map>
#include <iostream>

//------------------------------------------------------ Include personnel
#include "BarriereSortie.h"
#include "Outils.h"
#include "GererClavier.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define TO_WRITE 1 //  descripteur de lecture et écriture
#define TO_READ 0
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static bool exited = false;
static int *zone_nbPlace;
static Voiture *zone_placesParking;

static struct sembuf reserver = {0,-1,0};
static struct sembuf liberer = {0,1,0};

static int nbVoiturier=0;
static int *psem_placeLibre;
static int *psem_ecran;
static std::map<pid_t,Voiture> voituriers;

//------------------------------------------------------ Fonctions privées
static TypeZone DefineEtat(int num)
{
	enum TypeZone etat;
	switch(num)
	{
		case 1:
			etat =ETAT_P1;
			break;
		case 2:
			etat =ETAT_P2;
			break;
		case 3:
			etat =ETAT_P3;
			break;
		case 4:
			etat =ETAT_P4;
			break;
		case 5:
			etat =ETAT_P5;
			break;
		case 6: 
			etat =ETAT_P6;
			break;
		case 7:
			etat =ETAT_P7;
			break;
		case 8:
			etat =ETAT_P8;
			break;
		default:
			break;
	}
	return etat;
}

static void FinT (int noSignal)
// Mode emploi: destruction de la tâche BarriereSortie
// 	- envoie un signal de fin SIGUSR2 à tous les processus fils voituriers en cours
// 	- termine le processus BarriereSortie
{
	exited = true;
	pid_t voiturier;
	if(noSignal == SIGUSR2)
	{
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
		exit(0); // detachement automatique de touts les segments de mémoire attachés par le processus
	}
}

static void FinVoiturier(int noSignal)
// Mode emploi : handler de fin d'une tâche Voiturier
// 		- récupère l'id du processus mort ainsi que l'objet voiture associé à cet id dans la map voituriers
// 		- ajoute l'heure de départ à cet voiture 
// 		- efface de la zone Etat associé les références de la voiture
// 		- affiche les informations de sortie du véhicule
// 		- incrémente la sémaphore sem_placeLibre pour indiquer qu'une place s'est libérée
// 		- TODO efface la bonne requete en prenant compte du type voiture et ordre d'arrivée
// 		 
{
	if(!exited)
	{
		int numPlace;
		int status;
		if(noSignal == SIGCHLD )
		{
			pid_t voiturier = wait(&status); // 
			Voiture voit = voituriers[voiturier];  // pour récupérer les infos concernant la voiture gérée par le voiturier
			voit.depart = time(0);	// défini l'heure de départ 

			semop((*psem_ecran),&reserver,1); // Réservation de la ressource critique écran
			Effacer(DefineEtat(voit.numPlace)); 
			AfficherSortie(voit.type,voit.num,voit.arrivee,voit.depart);  
			semop((*psem_ecran),&liberer,1); // Liberation de la ressource critique écran

			voituriers.erase(voiturier); 
			zone_nbPlace++;
			
			semop((*psem_placeLibre),&liberer,1); //Pour indiquer qu'il y a de nouveau une place libre
		}
	}
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereSortie (int canal[],int sem_ecran,int sem_placeLibre,int mp_nbPlace, int mp_placesParking)
// Algorithme :
//		processus fils BarriereSortie
{
	//masque de blocage du signal SIGINT
	struct sigaction masqueFin;
	masqueFin.sa_handler = SIG_IGN;	// handler par défaut pour ignorer un signal
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);	
	
	struct sigaction actionFinT;
	actionFinT.sa_handler = FinT;  // associe la procedure FinT
	sigemptyset(&actionFinT.sa_mask);
	actionFinT.sa_flags = 0;
	sigaction(SIGUSR2, &actionFinT, NULL); // armement du signal SIGUSR2 sur l'action (le handler) FinT

	struct sigaction actionFinVoiturier;
	actionFinVoiturier.sa_handler = FinVoiturier;
	sigemptyset(&actionFinVoiturier.sa_mask);
	actionFinVoiturier.sa_flags=0; 
	sigaction(SIGCHLD, &actionFinVoiturier,NULL); 

	//fermeture du canal en écriture
	close(canal[TO_WRITE]);
	
	//attachement aux segments de mémoire
	zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,0); 
	zone_placesParking = (Voiture*) shmat(mp_placesParking,NULL,SHM_RDONLY); // lecture unique sur la mémoire partagée


	psem_placeLibre =&sem_placeLibre;
	psem_ecran=&sem_ecran;

	
	// --- PHASE MOTEUR ---
	int numPlace;
	int readRet;
	do
	{
		while((readRet = read(canal[TO_READ],&numPlace,sizeof(int))) > 0)
		{
			pid_t voiturier = SortirVoiture(numPlace);	// appel un voiturier pour sortir la voiture garée à numPlace
			
			voituriers.insert(make_pair(voiturier,zone_placesParking[numPlace]));	// insère dans la structure de données des voituriers en cours
		
		}
	}
	while(readRet == -1);
} //----- fin de BarriereSortie

