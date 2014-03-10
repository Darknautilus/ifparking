// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

// Par Thomas Losbar
// thomas.losbar@insa-lyon.fr

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
	// Masquage de SIGINT
	struct sigaction masqueFin;
	masqueFin.sa_handler = SIG_IGN;
	sigemptyset(&masqueFin.sa_mask);
	masqueFin.sa_flags = 0;
	sigaction(SIGINT,&masqueFin,NULL);
	// Fin Masquage SIGINT

	// Génération des clés IPC
	FILE *ipcFile = fopen(IPC_KEYS_FILE,"w");
	if(ipcFile == NULL)
	{
		exit(EXIT_FAILURE);
	}
	key_t ipcKeys[NB_MAX_IPC];
	int i;
	for(i=0;i<NB_MAX_IPC;i++)
	{
		ipcKeys[i] = ftok(IPC_KEYS_FILE,i);
	}
	// Fin génération clés

	
	
	// Creation des IPC
	int mp_nbPlace = shmget(ipcKeys[0],sizeof(int),IPC_CREAT | DROITS);
	int mp_placesParking = shmget(ipcKeys[1],8*sizeof(Voiture),IPC_CREAT | DROITS); // Pour connaitre quelle voiture occupe quelle place

	int sem_placeLibre = semget (ipcKeys[2],1, IPC_CREAT | DROITS); // sémaphore pour gérer une nouvelle place disponible laissé après une sortie de voiture
	int sem_ecran = semget(ipcKeys[3],1,IPC_CREAT|DROITS); // sémaphore pour gérer les accès concurent sur la ressource critique écran.

	ress_critique requetes; // Gère les requêtes déposées par les voitures
	requetes.mem = shmget (ipcKeys[4], 3*sizeof(Requete), IPC_CREAT | DROITS);
	requetes.sem = semget(ipcKeys[5],3,IPC_CREAT|DROITS);

	ress_critique numVoiture; // Gère l'attribution des numéros d'immatriculation
	numVoiture.mem = shmget(ipcKeys[6],sizeof(int),IPC_CREAT | DROITS);
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
	// Fin création IPC

	// Initalisation des mémoires partagées
	int *zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,0);
	*zone_nbPlace = NB_PLACES;
	shmdt(zone_nbPlace);

	Requete *requeteInit = (Requete*)shmat(requetes.mem,NULL,0);
	for(i=0;i<3;i++)
	{
		requeteInit[i].type = AUCUN;
		requeteInit[i].barriere = AUCUNE;
		requeteInit[i].arrivee = 0;
	}
	shmdt(requeteInit);
	
	int *numVoitureInit = (int*)shmat(numVoiture.mem,NULL,0);
	*numVoitureInit = 0;
	shmdt(numVoitureInit);
	// Fin initialisation mémoire partagées
	

	// Initalisation des sémaphores
	semctl(sem_placeLibre,0,SETVAL,NB_PLACES);
	semctl(sem_ecran,0,SETVAL,1);
	semctl(numVoiture.sem,0,SETVAL,1);
	unsigned short int semReqInitVals[3] = {1,1,1};
	semctl(requetes.sem,3,SETALL,semReqInitVals);
	// Fin initialisation des sémaphores
	
	// Initialisation de l'affichage
	InitialiserApplication(TYPE_TERMINAL);
	noHeure = ActiverHeure();
	
	// Liste tâches filles
	pid_t noGererClavier;
	pid_t noBarriereSortie;
	pid_t noBarriereEntree1;
	pid_t noBarriereEntree2;
	pid_t noBarriereEntree3;
	pid_t noHeure;
	
	// Creation des processus fils
	if((noGererClavier = fork()) == 0)
	{
		GererClavier(barriere1,barriere2,barriere3,barriereSortie);
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
		// On n'utilise pas les canaux ici
		for(int i =0;i<2;i++)
		{
			close (barriere1[i]);
			close (barriere2[i]);
			close (barriere3[i]);
			close (barriereSortie[i]);
		}
		// Fin phase initialisation
	
		// Phase moteur
		pid_t child;
		do
		{	
			child = waitpid(noGererClavier,NULL,0);
		}
		while(child == -1 && errno == EINTR);
		// Fin phase moteur
		
		// Phase de destruction
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
		
		// Fermeture application
		TerminerApplication(true);
		
		// Destruction des ipc
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

