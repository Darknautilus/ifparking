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
#include <Menu.h>

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void GererClavier();
// Mode d'emploi :
// Contrat :

void Commande(char code, unsigned int valeur);

#endif // GererClavier_H

