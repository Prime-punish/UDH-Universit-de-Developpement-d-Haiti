#ifndef CONSOLE_ETUDIANT_H
#define CONSOLE_ETUDIANT_H

#include "models.h"

void menuPrincipalEtudiant();
void inscriptionEtudiant();
void connexionEtudiant();
void dashboardEtudiant(CompteEtudiant &compte);

// Sous-sections du dashboard
void afficherProfilEtudiant(CompteEtudiant &compte);
void finaliserDossierInscription(CompteEtudiant &compte);
void afficherProgrammeAcademique(const CompteEtudiant &compte);
void afficherReleveNotes(const CompteEtudiant &compte);
void afficherExamensPartages(const CompteEtudiant &compte);
void gererVersementsEtudiant(const CompteEtudiant &compte);
void messagerieEtudiant(const CompteEtudiant &compte);
void afficherAnnoncesEtudiant(const CompteEtudiant &compte);
void afficherCalendrierEtudiant();

#endif // CONSOLE_ETUDIANT_H
