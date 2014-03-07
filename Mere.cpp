// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

//---------- Réalisation de la tâche <Mere> (fichier Mere.cpp) ---

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
#include "Mere.h"
#include "Heure.h"
#include "Outils.h"
#include "Menu.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define DROITS 0666 //lecture, écriture pour tout le monde
#define IPC_PATHNAME
#define NB_PLACES 8
#define TYPE_TERMINAL XTERM
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main ( )
// Algorithme :
//	- Processus principal lancé au début de l'application.
{
	// on masque le signal SIGINT
	struct sigaction masqueFin;
	masqueFin.sa_handler = SIG_IGN;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);	

	//liste tâche filles
	pid_t noGererClavier;
	pid_t noBarriereSortie;
	pid_t noBarriereEntree1;
	pid_t noBarriereEntree2;
	pid_t noBarriereEntree3;
	pid_t noHeure;
	
	key_t clefParking; // voir si c'est necessaire plutot qu'une simple clef privée..
	
	//creation des ipc
	int mp_nbPlace =shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | DROITS);
	int mp_placesParking = shmget(IPC_PRIVATE,8*sizeof(Voiture),IPC_CREAT | DROITS); // Pour connaitre quelle voiture occupe quelle place
	int mp_requetes = shmget (IPC_PRIVATE, 3*sizeof(string), IPC_CREAT | DROITS);

	int sem_placeLibre = semget (IPC_PRIVATE,1, IPC_CREAT | DROITS); //sémaphore pour gérer une nouvelle place disponible laissé après une sortie de voiture
	int sem_ecran = semget(IPC_PRIVATE,1,IPC_CREAT|DROITS); //sémaphore pour gérer les accès concurent sur la ressource critique écran.

	//creation canal de communication des barrieres
	int barriere1[2];
	int barriere2[2];
	int barriere3[2];
	int barriere4[2];
	pipe(barriere1);
	pipe(barriere2);
	pipe(barriere3);
	pipe(barriere4);

	//phase Initialisation
	InitialiserApplication(TYPE_TERMINAL);
	noHeure = ActiverHeure();

	//initalisation des mémoires partagées
	int flag_options = 0;
	
	int *zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,flag_options); //attachement au segment de mémoire
	*zone_nbPlace = 8;

	Voiture *zone_placesParking = (Voiture*) shmat(mp_placesParking,NULL,flag_options);
	
	string * zone_requetes = (string *) shmat(mp_requetes,NULL,flag_options);
	// fin initialisation mémoire partagées
	

	//initalisation des sémaphores
	semctl(sem_placeLibre,0,SETVAL,NB_PLACES);
	semctl(sem_ecran,0,SETVAL,1);
	
	//creation des processus fils..
	if((noGererClavier =fork()) == 0)
	{
		GererClavier(barriere1,barriere2,barriere3,barriere4);
		//exit(0);
	}
	else if((noBarriereSortie = fork()) == 0)
	{
		BarriereSortie(barriere4,sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);	
	}
	else if((noBarriereEntree1 = fork()) == 0)
	{
		BarriereEntree(barriere1, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
		//exit(0);
	}
	else if((noBarriereEntree2 = fork()) == 0)
	{
		BarriereEntree(barriere2, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
		//exit(0);
	}
	else if((noBarriereEntree3 = fork()) == 0)
	{
		BarriereEntree(barriere3, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
		//exit(0);
	}
	else
	{
		for(int i =0;i<2;i++)
		{
			close (barriere1[i]);
			close (barriere2[i]);
			close (barriere3[i]);
			close (barriere4[i]);
		}
		//fin phase initialisation
	
		//phase moteur
		waitpid(noGererClavier,NULL,0);
		
		//phase de destruction
		kill(noBarriereEntree3,SIGUSR2);
		waitpid(noBarriereEntree3,NULL,0);
		kill(noBarriereEntree2,SIGUSR2);
		waitpid(noBarriereEntree2,NULL,0);
		kill(noBarriereEntree1,SIGUSR2);
		waitpid(noBarriereEntree1,NULL,0);
		kill(noBarriereSortie,SIGUSR2);
		waitpid(noBarriereSortie,NULL,0);
		kill(noHeure,SIGUSR2);
		waitpid(noHeure,NULL,0);
		
		bool efface = true;
		TerminerApplication(efface);
		
		//destruction des ipc
		shmctl(mp_nbPlace,IPC_RMID,0);
		shmctl(mp_placesParking,IPC_RMID,0);
		shmctl(mp_requetes,IPC_RMID,0);
		semctl(sem_placeLibre,IPC_RMID,0);
		semctl(sem_ecran,IPC_RMID,0);

		exit(0);
	}	
} 

