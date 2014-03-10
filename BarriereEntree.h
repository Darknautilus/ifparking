/*************************************************************************
                           BarriereEntree
                             -------------------
    e-mail               : aurelien.bertron@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <BarriereEntree> (fichier BarriereEntree.h) -------
#if ! defined ( BarriereEntree_H )
#define BarriereEntree_H

//------------------------------------------------------------------------
// Rôle de la tâche <BarriereEntree>
//		Gère les actions à effectuer par chaque barrière
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "GererClavier.h"

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
/*
	Lance la gestion de la barrière
	Contrat : toutes les ressources critiques et de communication doivent être intialisées.
*/
void BarriereEntree(int canal[], ress_critique numVoiture, ress_critique requetes, int sem_ecran, int sem_placeLibre, int mp_nbPlace, int mp_placesParking);


#endif // BarriereEntree_H

