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
#include <string>
#include <map>
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
static int *zone_nbPlace;
static int *zone_placesParking;

static struct sembuf reserver = {0,-1,0};
static struct sembuf liberer = {0,1,0};

static int nbVoiturier=0;
static int *psem_placeLibre;
static int *psem_ecran;
static int (*pcanal)[];
static std::map<pid_t,Voiture> voituriers;

//------------------------------------------------------ Fonctions privées
static void FinT (int noSignal)
{
	if(noSignal == SIGUSR2)
	{
		for(std::map<pid_t,Voiture>::iterator it = voituriers.begin(); it != voituriers.end(); ++it)
		{
			kill(it->first,SIGUSR2);
			waitpid(it->first,NULL,0);
		}
		voituriers.clear();
		exit(0); // detachement automatique de touts les segments de mémoire attachés par le processus
	}
}

static void FinVoiturier(int noSignal)
{
	int numPlace;
	if(noSignal == SIGCHLD )
	{
		pid_t voiturier = wait(&numPlace); // pour récupérer le numéro de place de la voiture ainsi que le voiturier qui s'en est occupé
		Voiture voit = voituriers[voiturier];  // pour récupérer les infos concernant la voiture gérée par le voiturier
		voit.depart = time(0);	// défini l'heure de départ 

		enum TypeZone etat;
		//etat	=	ETAT_P2;
		semop((*psem_ecran),&reserver,1); // Réservation de la ressource critique écran
		//Effacer(etat); 
		AfficherSortie(voit.type,voit.num,voit.arrivee,voit.depart);  
		semop((*psem_ecran),&liberer,1); // Liberation de la ressource critique écran

		voituriers.erase(voiturier);
		nbVoiturier--;
		zone_nbPlace++;
		
		semop((*psem_placeLibre),&liberer,1); //Pour indiquer qu'il y a de nouveau une place libre
	}
}

static void Initialisation(int canal[], int sem_ecran,int sem_placeLibre,/* int mp_placesParking*/ int mp_nbPlace)
// Mode d'emploi :
//	- masque les signaux SIGINT,SIGUSR2, SIGCHLD
//	- arme les signaux SIGUSR2 et SIGCHLD respectivement sur les handler FinT et FinVoiturier
//	- ferme le canal d'écriture du canal utilisé 
//
// Contrat : aucun
{
	//masque de blocage de signaux
	sigset_t masque;
	sigset_t anciens;

	sigemptyset(&masque); // création d'un masque vide (liste de signaux masqué)
	sigaddset(&masque,SIGINT); // ajout du signal SIGINT à mon masque
	sigaddset(&masque,SIGUSR2);
	sigaddset(&masque,SIGCHLD);
	sigprocmask(SIG_SETMASK,&masque,&anciens); // mise en place du masque grâce à cette fonction !
	
	struct sigaction actionFinT;
	actionFinT.sa_handler = FinT;  // associe la procedure FinT
	sigemptyset(&actionFinT.sa_mask);
	actionFinT.sa_flags = 0;
	sigaction(SIGUSR2, &actionFinT, NULL); // armement du signal SIGUSR2 sur l'action (le handler) FinT

	struct sigaction actionFinVoiturier;
	actionFinVoiturier.sa_handler = FinVoiturier;
	sigemptyset(&actionFinVoiturier.sa_mask);
	actionFinVoiturier.sa_flags=0; //TODO penser à mettre ou pas l'option special NO_CLDSTOP
	sigaction(SIGCHLD, &actionFinVoiturier,0); 

	//fermeture du canal en écriture
	close(canal[TO_WRITE]);
	
	int flag_options = 0;
	zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,flag_options); //attachement au segment de mémoire
	//zone_placesParking = (int*) shmat(mp_placesParking,NULL,flag_options);

	*psem_placeLibre =sem_placeLibre;
	*psem_ecran=sem_ecran;
	//*pcanal=canal;
} //----- fin de Initialisation()

static void Moteur()
// Mode d'emploi :
//	- lecture en boucle du canal (de lecture) sortie où GererClavier est écrivain
//	- récupère les informations de la voiture à faire sortir
//	- appel de la tâche Voiturier pour sortir la voiture
//
// Contrat : aucun
{
	for(;;)
	{
		Voiture voit;
		while(read((*pcanal)[TO_READ],&voit,sizeof(Voiture)))
		{
			while(nbVoiturier<8)
			{
				pid_t voiturier = SortirVoiture(voit.numPlace); // appel un voiturier pour sortir la voiture garée à numPlace
				voituriers.insert(make_pair(voiturier,voit));	// insère dans la structure de données des voituriers en cours
				nbVoiturier++;
			}
		}
	}
} //----- fin de Moteur()

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereSortie (int canal[],int sem_ecran,int sem_placeLibre,int mp_nbPlace)
// Algorithme :
//
{
	Initialisation(canal,sem_ecran,sem_placeLibre, mp_nbPlace);
	Moteur();

} //----- fin de BarriereSortie

