/// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

// Par Thomas Losbar
// thomas.losbar@insa-lyon.fr

//---------- Interface de la tâche <Mere> (fichier Mere.h) -------
#if ! defined ( MERE_H )
#define MERE_H

//------------------------------------------------------------------------
// Rôle de la tâche <Mere>
// 	En phase d'initalisation: est chargée de lancer l'interface homme/machine de l'application Parking, créé les IPC nécessaires 
// 	et les différentes taches fille (BarriereSortie, BarriereEntree, GererClavier ) 
// 	En phase de destrucion de l'application: envoie les signaux de demande de fin aux différentes taches filles créé précédement
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "GererClavier.h"
#include "BarriereEntree.h"
#include "BarriereSortie.h"

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

#endif // MERE_H

