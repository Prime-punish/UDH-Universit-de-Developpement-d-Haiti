#ifndef ISOLATIONFACULTEUTILS_H
#define ISOLATIONFACULTEUTILS_H

#include <QString>
#include <vector>
#include <string>
#include <map>
#include "professeur.h"
#include "etudiant.h"

namespace IsolationFaculteUtils {

    // Normalise le nom de la faculté pour comparaison robuste (sans accents, minuscules, alias)
    QString normaliserFaculte(const QString &fac);

    // Vérifie de manière stricte si une entité de candidateFac est accessible par un coordonnateur de assignedFac
    bool verifierAccesFaculte(const QString &candidateFac, const QString &assignedFac);

    // Fonctions centralisées de filtrage par faculté
    std::vector<Professeur> filtrerProfsParFaculte(const std::vector<Professeur> &profs, const QString &assignedFac);
    std::vector<CompteEtudiant> filtrerComptesParFaculte(const std::vector<CompteEtudiant> &comptes, const QString &assignedFac);
    ProgrammeEtudes filtrerProgrammeParFaculte(const std::map<std::string, ProgrammeEtudes> &programmes, const QString &assignedFac);

}

#endif // ISOLATIONFACULTEUTILS_H
