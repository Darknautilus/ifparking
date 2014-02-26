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
//------------------------------------------------------ Include personnel
#include "BarriereSortie.h"
#include "Outils.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define VOITURE_ENTREE_R 1 // les descritpeurs des deux canaux anonymes utilisés
#define VOITURE_ENTREE_W 2
#define VOITURE_SORTIE_R 3
#define VOITURE_SORTIE_W 4
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int *zone_nbPlace;
static int *zone_placesParking;

static struct sembuf reserver = {0,-1,0};
static struct sembuf liberer = {0,1,0};

static int nbVoiturier=0;
static int *psem_nbPlace;
//------------------------------------------------------ Fonctions privées
static void FinT (int noSignal)
{
	if(noSignal == SIGUSR2)
	{

	}
}

static void FinVoiturier(int noSignal)
{
	int numPlace;
	if(noSignal = SIGCHLD )
	{
		wait(&numPlace); // pour récupérer le retour du processus Voiturier
		string etat= "ETAT_P"+numPlace; 
		Effacer(etat); 
		//AfficherSortie manque les infos 
		nbVoiturier--;
		zone_nbPlace++;
		semop((*psem_nbPlace),&liberer,1); //Pour indiquer qu'il y a de nouveau une place libre
	}
}

static void Initialisation(int sem_nbPlace, int mp_placesParking, int mp_nbPlace, int *canal)
// Mode d'emploi :
//	- masque les signaux SIGINT,SIGUSR2, SIGCHLD
//	- arme les signaux SIGUSR2 et SIGCHLD respectivement sur les handler FinT et FinVoiturier
//	- ferme le canal de lecture et d'écriture destiné à la tache BarriereEntree
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

	//pas sur que ça marche... SOLUTION ACTUELLE: récupérer en paramètre le tableau de descripteur initialisé par Mere.cpp
	close(canal[VOITURE_ENTREE_R]);
	close(canal[VOITURE_ENTREE_W]);
	close(canal[VOITURE_SORTIE_W]);
	
	int flag_options = 0;
	zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,flag_options); //attachement au segment de mémoire
	zone_placesParking = (int*) shmat(mp_placesParking,NULL,flag_options);

	*psem_nbPlace =sem_nbPlace;
} //----- fin de Initialisation()

static void moteur()
{
	for(;;)
	{
		
	}
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereSortie ( int sem_nbPlace, int sem_ecran, int mp_placesParking, int mp_nbPlace, int *canal )
// Algorithme :
//
{
	Initialisation(sem_nbPlace, mp_placesParking, mp_nbPlace, canal);
	moteur();

} //----- fin de Nom

