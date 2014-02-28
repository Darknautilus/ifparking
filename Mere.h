/// ======================
// INSA Lyon, Département Informatique
// TP 3IF : Programmation concurrente
// Binome : B3229
// ======================

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

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
// type Nom (liste de paramètres );
// Mode d'emploi :
//
// Contrat :
//

#endif // MERE_H

