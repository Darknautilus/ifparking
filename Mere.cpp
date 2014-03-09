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
#include <sys/errno.h>
#include <string>
//------------------------------------------------------ Include personnel
#include "Mere.h"
#include "Heure.h"
#include "Outils.h"
#include "Menu.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define NB_MAX_IPC 8
#define IPC_KEYS_FILE "ipcKeys"
#define DROITS 0666 //lecture, écriture pour tout le monde
#define IPC_PATHNAME
#define NB_PLACES 8
#define TYPE_TERMINAL XTERM
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main (int argc, const char **argv)
// Algorithme :
//	- Processus principal lancé au début de l'application.
{
	// on masque le signal SIGINT
	struct sigaction masqueFin;
	masqueFin.sa_handler = SIG_IGN;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);

	FILE *ipcFile = fopen(IPC_KEYS_FILE,"w");
	if(ipcFile == NULL)
	{
		exit(EXIT_FAILURE);
	}

	// génération des clés IPC
	key_t ipcKeys[NB_MAX_IPC];
	int i;
	for(i=0;i<NB_MAX_IPC;i++)
	{
		ipcKeys[i] = ftok(IPC_KEYS_FILE,i);
	}

	//liste tâche filles
	pid_t noGererClavier;
	pid_t noBarriereSortie;
	pid_t noBarriereEntree1;
	pid_t noBarriereEntree2;
	pid_t noBarriereEntree3;
	pid_t noHeure;
	
	//creation des ipc
	int mp_nbPlace =shmget(ipcKeys[0],sizeof(int),IPC_CREAT | DROITS);
	int mp_placesParking = shmget(ipcKeys[1],8*sizeof(Voiture),IPC_CREAT | DROITS); // Pour connaitre quelle voiture occupe quelle place

	int sem_placeLibre = semget (ipcKeys[2],1, IPC_CREAT | DROITS); //sémaphore pour gérer une nouvelle place disponible laissé après une sortie de voiture
	int sem_ecran = semget(ipcKeys[3],1,IPC_CREAT|DROITS); //sémaphore pour gérer les accès concurent sur la ressource critique écran.

	ress_critique requetes;
	requetes.mem = shmget (ipcKeys[4], 3*sizeof(Requete), IPC_CREAT | DROITS);
	requetes.sem = semget(ipcKeys[5],3,IPC_CREAT|DROITS);

	ress_critique numVoiture;
	numVoiture.mem = shmget(ipcKeys[6],sizeof(int),IPC_CREAT | DROITS); // le numéro d'immatriculation des voitures
	numVoiture.sem = semget(ipcKeys[7],1,IPC_CREAT | DROITS);

	//creation canal de communication des barrieres
	int barriere1[2];
	int barriere2[2];
	int barriere3[2];
	int barriereSortie[2];
	pipe(barriere1);
	pipe(barriere2);
	pipe(barriere3);
	pipe(barriereSortie);

	//phase Initialisation
	InitialiserApplication(TYPE_TERMINAL);
	noHeure = ActiverHeure();

	//initalisation des mémoires partagées
	int flag_options = 0;
	
	int *zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,flag_options); //attachement au segment de mémoire
	*zone_nbPlace = 8;

	Voiture *zone_placesParking = (Voiture*) shmat(mp_placesParking,NULL,flag_options);

	Requete *requeteInit = (Requete*)shmat(requetes.mem,NULL,flag_options);
	for(i=0;i<3;i++)
	{
		requeteInit[i].type = AUCUN;
		requeteInit[i].barriere = AUCUNE;
		requeteInit[i].arrivee = 0;
	}
	shmdt(requeteInit);
	
	int *numVoitureInit = (int*)shmat(numVoiture.mem,NULL,flag_options);
	*numVoitureInit = 0;
	shmdt(numVoitureInit);
	// fin initialisation mémoire partagées
	

	//initalisation des sémaphores
	semctl(sem_placeLibre,0,SETVAL,NB_PLACES);
	semctl(sem_ecran,0,SETVAL,1);
	semctl(numVoiture.sem,0,SETVAL,1);
	unsigned short int semReqInitVals[3] = {1,1,1};
	semctl(requetes.sem,3,SETALL,semReqInitVals);
	
	//creation des processus fils..
	if((noGererClavier =fork()) == 0)
	{
		GererClavier(barriere1,barriere2,barriere3,barriereSortie);
		//exit(0);
	}
	else if((noBarriereSortie = fork()) == 0)
	{
		BarriereSortie(barriereSortie,sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);	
	}
	else if((noBarriereEntree1 = fork()) == 0)
	{
		BarriereEntree(barriere1, numVoiture, requetes, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
	}
	else if((noBarriereEntree2 = fork()) == 0)
	{
		BarriereEntree(barriere2, numVoiture, requetes, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
	}
	else if((noBarriereEntree3 = fork()) == 0)
	{
		BarriereEntree(barriere3, numVoiture, requetes, sem_ecran,sem_placeLibre,mp_nbPlace,mp_placesParking);
	}
	else
	{
		for(int i =0;i<2;i++)
		{
			close (barriere1[i]);
			close (barriere2[i]);
			close (barriere3[i]);
			close (barriereSortie[i]);
		}
		//fin phase initialisation
	
		//phase moteur
		pid_t child;
		do
		{	
			child = waitpid(noGererClavier,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		
		//phase de destruction
		kill(noBarriereEntree3,SIGUSR2);
		do
		{
			child = waitpid(noBarriereEntree3,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		kill(noBarriereEntree2,SIGUSR2);
		do
		{
			child = waitpid(noBarriereEntree2,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		kill(noBarriereEntree1,SIGUSR2);
		do
		{
			child = waitpid(noBarriereEntree1,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		kill(noBarriereSortie,SIGUSR2);
		do
		{
			child = waitpid(noBarriereSortie,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		kill(noHeure,SIGUSR2);
		do
		{
			child = waitpid(noHeure,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		
		TerminerApplication(true);
		
		//destruction des ipc
		shmctl(mp_nbPlace,IPC_RMID,0);
		shmctl(mp_placesParking,IPC_RMID,0);

		semctl(sem_placeLibre,0,IPC_RMID,0);
		semctl(sem_ecran,0,IPC_RMID,0);

		semctl(numVoiture.sem,0,IPC_RMID,0);
		shmctl(numVoiture.mem,IPC_RMID,0);

		semctl(requetes.sem,0,IPC_RMID,0);
		shmctl(requetes.mem,IPC_RMID,0);

		fclose(ipcFile);
		unlink(IPC_KEYS_FILE);

		exit(0);
	}	
} 

