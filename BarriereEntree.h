/*************************************************************************
                           BarriereEntree  -  description
                             -------------------
    début                : BarriereEntree
    e-mail               : BarriereEntree
*************************************************************************/

//---------- Interface de la tâche <BarriereEntree> (fichier BarriereEntree.h) -------
#if ! defined ( BarriereEntree_H )
#define BarriereEntree_H

//------------------------------------------------------------------------
// Rôle de la tâche <BarriereEntree>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "GererClavier.h"

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereEntree(int canal[],int mp_numVoiture, int sem_numVoiture, int sem_ecran, int sem_placeLibre, int mp_nbPlace, int mp_placesParking);
//Mode d'emploi :

//Contrat :


#endif // BarriereEntree_H

