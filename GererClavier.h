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

//------------------------------------------------------------------ Types
typedef struct
{
	TypeUsager type;
	TypeBarriere barriere;
	int num;
} Voiture;

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GererClavier(int pBarriere1[], int pBarriere2[], int pBarriere3[]);
// Mode d'emploi :
// Contrat :

void Commande(char code, unsigned int valeur);

#endif // GererClavier_H

