/*************************************************************************
                           GererClavier
                             -------------------
    e-mail               : aurelien.bertron@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <GererClavier> (fichier GererClavier.h) -------
#if ! defined ( GererClavier_H )
#define GererClavier_H

//------------------------------------------------------------------------
// Rôle de la tâche <GererClavier>
//		Elle est chargée de gérer les entrées clavier et de faire réagir l'application en conséquences.
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <Outils.h>
#include <Menu.h>

//------------------------------------------------------------- Constantes
#define REQ_BLAISE_PROF 0
#define REQ_BLAISE_AUTRE 1
#define REQ_GASTON 2

//------------------------------------------------------------------ Types

/*
	Définit une ressource critique en mémoire partagée.
	Permet d'encapsuler à la fois le descripteur de mémoire partagée et le sémaphore associé.
*/
typedef struct
{
	int mem;
	int sem;
} ress_critique;

/*
	Contient les différentes informations concernant une voiture.
*/
typedef struct
{
	TypeUsager type;
	TypeBarriere barriere;
	int num; // numero d'immatriculation
	int numPlace;
	time_t arrivee;
	time_t depart;
} Voiture;

/*
	Les informations d'une requête déposée par une voiture.
*/
typedef struct
{
	TypeUsager type;
	TypeBarriere barriere;
	time_t arrivee;
} Requete;

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

/*
	Permet d'obtenir l'id du sémaphore élémentaire associé à une requête.
*/
unsigned short int TypeBarriereToReqId(TypeBarriere barriere);

/*
	Lance la gestion du clavier
	Paramètres : les canaux de communication anonymes pour communiquer avec les barrières.
	Contrat : les canaux doivent être initialisés avec pipe()
*/
void GererClavier(int pBarriere1[], int pBarriere2[], int pBarriere3[], int pBarriereSortie[]);

/*
	Permet de traiter les entrées clavier.
	Contrat : Il n'y a pas de sens à appeler cette fonction. La fonction GererClavier le fait automatiquement.
*/
void Commande(char code, unsigned int valeur);

#endif // GererClavier_H

