/*************************************************************************
                           GererClavier  -  description
                             -------------------
    début                : GererClavier
    e-mail               : GererClavier
*************************************************************************/

//---------- Interface de la tâche <GererClavier> (fichier GererClavier.h) -------
#if ! defined ( GererClavier_H )
#define GererClavier_H

//------------------------------------------------------------------------
// Rôle de la tâche <GererClavier>
//
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
typedef struct
{
	int mem;
	int sem;
} ress_critique; // Ressource critique

typedef struct
{
	TypeUsager type;
	TypeBarriere barriere;
	int num;
	int numPlace; // qu'on utilise que pour la commande de sortie
	time_t arrivee;
	time_t depart;
} Voiture;

typedef struct
{
	TypeUsager type;
	TypeBarriere barriere;
	time_t arrivee;
} Requete;

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
unsigned short int TypeBarriereToReqId(TypeBarriere barriere);

void GererClavier(int pBarriere1[], int pBarriere2[], int pBarriere3[], int pBarriere4[]);
// Mode d'emploi :
// Contrat :

void Commande(char code, unsigned int valeur);

#endif // GererClavier_H

