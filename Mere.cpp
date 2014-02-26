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
//------------------------------------------------------ Include personnel
#include "Mere.h"
#include "Heure.h"
#include "Outils.h"
#include "Menu.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
#define DROITS 0666 //lecture, écriture pour tout le monde
#define IPC_PATHNAME

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main ( )
// Algorithme :
//	Fonction exécuté au lancement de l'application
{
	//liste tâche filles
	pid_t noGererClavier;
	pid_t noBarriereSortie;
	pid_t noBarriereEntree;
	pid_t noHeure;

	//masque de blocage de signaux
	sigset_t masque;
	sigset_t anciens;

	//sigemptyset(&masque); // création d'un masque vide (liste de signaux masqué)
	//sigaddset(&masque,SIGINT); // ajout du signal SIGINT à mon masque
	//sigprocmask(SIG_SETMASK,&masque,&anciens); // mise en place du masque grâce à cette fonction !

	
	key_t clefParking; // voir si c'est necessaire plutot qu'une simple clef privée..
	
	//creation des ipc
	int mp_nbPlace =shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | DROITS);
	int mp_placesParking = shmget(IPC_PRIVATE,8*sizeof(int ),IPC_CREAT | DROITS);
	int mp_requetes = shmget (IPC_PRIVATE, sizeof(char *), IPC_CREAT | DROITS);

	int sem_placeLibre = semget (IPC_PRIVATE,1, IPC_CREAT | DROITS); //sémaphore pour gérer une nouvelle place disponible laissé après une sortie de voiture
	int sem_ecran = semget(IPC_PRIVATE,1,IPC_CREAT|DROITS); //sémaphore pour gérer les accès concurent sur la ressource critique écran.

	//creation canal de communication (anonyme)
	int canal[4];
	pipe(canal);



	//phase Initialisation
	//InitialiserApplication(XTERM);
	noHeure = ActiverHeure();

	//initalisation des mémoires partagées
	int flag_options = 0;
	int *zone_nbPlace = (int*) shmat(mp_nbPlace,NULL,flag_options); //attachement au segment de mémoire
	*zone_nbPlace = 8;
	int *zone_placesParking = (int*) shmat(mp_placesParking,NULL,flag_options);
	for(int i =0; i<8;i++)
		zone_placesParking[i]=0;		// on utilisera 0 pour dire que la place est vacante et 1 pour dire qu'elle est occupée
	//TODO initialisation mémoire partagée mp_requetes
	
	//initalisation des sémaphores
	semctl(sem_placeLibre,0,SETVAL,8);
	semctl(sem_ecran,0,SETVAL,1);
	
	if((noGererClavier =fork()) == 0)
	{
		//appel gererClavier()
		for(;;);
	}
	else if((noBarriereSortie = fork()) == 0)
	{
		//appel BariereSortie(mp_nbPlace,mp_placesParking,sem_placeLibre,sem_ecran)
		for(;;);
	}
	else if((noBarriereEntree = fork()) == 0)
	{
		// appel BarriereEntree( ... )
		for(;;);
	}
	else
	{
		for(int i =0;i<4;i++)
			close (canal[i]);
		//fin phase initialisation
	
		//phase moteur
		waitpid(noGererClavier,NULL,0);
		printf("%d est mort alors je pète tout !",noGererClavier);	
		//phase de destruction
		kill(noBarriereEntree,SIGKILL);
		waitpid(noBarriereEntree,NULL,0);
		kill(noBarriereSortie,SIGKILL);
		waitpid(noBarriereSortie,NULL,0);
		//kill(noHeure,SIGKILL);
		//waitpid(noHeure,0,0);

		//destruction des ipc
		shmctl(mp_nbPlace,IPC_RMID,0);
		shmctl(mp_placesParking,IPC_RMID,0);
		shmctl(mp_requetes,IPC_RMID,0);
		semctl(sem_placeLibre,IPC_RMID,0);
		semctl(sem_ecran,IPC_RMID,0);

	exit(0);
	}	


} 

