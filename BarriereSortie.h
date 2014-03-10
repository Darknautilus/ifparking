/// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

// Par Thomas Losbar
// thomas.losbar@insa-lyon.fr

//---------- Interface de la tâche <BarriereSortie> (fichier BarriereSortie.h) -------
#if ! defined ( BARRIERESORTIE_H )
#define BARRIERESORTIE_H

//------------------------------------------------------------------------
// Rôle de la tâche <BarriereSortie>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void BarriereSortie(int canal[], int sem_ecran, int sem_placeLibre, int mp_nbPlace,int mp_placesParking);

#endif // BARRIERESORTIE_H

