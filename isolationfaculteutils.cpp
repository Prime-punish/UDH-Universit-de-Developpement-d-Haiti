#include "isolationfaculteutils.h"
#include <QDebug>

namespace IsolationFaculteUtils {

QString normaliserFaculte(const QString &fac) {
    QString s = fac.trimmed().toLower();
    
    // Remplacement des accents
    s.replace("é", "e").replace("è", "e").replace("ê", "e").replace("ë", "e");
    s.replace("à", "a").replace("â", "a").replace("ä", "a");
    s.replace("î", "i").replace("ï", "i");
    s.replace("ô", "o").replace("ö", "o");
    s.replace("ù", "u").replace("û", "u").replace("ü", "u");
    s.replace("ç", "c");

    // Mapping des alias courants
    if (s.contains("informatique")) return "genie informatique";
    if (s.contains("civil")) return "genie civil";
    if (s.contains("econom") || s.contains("compta")) return "science economique et comptabilite";
    if (s.contains("admin")) return "science administration";
    if (s.contains("jurid") || s.contains("droit")) return "science juridique";
    if (s.contains("educ")) return "science education";

    return s;
}

bool verifierAccesFaculte(const QString &candidateFac, const QString &assignedFac) {
    if (assignedFac.trimmed().isEmpty()) {
        return false;
    }
    QString candNorm = normaliserFaculte(candidateFac);
    QString assNorm = normaliserFaculte(assignedFac);
    return !candNorm.isEmpty() && (candNorm == assNorm);
}

std::vector<Professeur> filtrerProfsParFaculte(const std::vector<Professeur> &profs, const QString &assignedFac) {
    std::vector<Professeur> resultat;
    for (const auto &p : profs) {
        if (verifierAccesFaculte(QString::fromStdString(p.getFaculte()), assignedFac)) {
            resultat.push_back(p);
        }
    }
    return resultat;
}

std::vector<CompteEtudiant> filtrerComptesParFaculte(const std::vector<CompteEtudiant> &comptes, const QString &assignedFac) {
    std::vector<CompteEtudiant> resultat;
    for (const auto &c : comptes) {
        if (verifierAccesFaculte(QString::fromStdString(c.getFaculte()), assignedFac)) {
            resultat.push_back(c);
        }
    }
    return resultat;
}

ProgrammeEtudes filtrerProgrammeParFaculte(const std::map<std::string, ProgrammeEtudes> &programmes, const QString &assignedFac) {
    QString assNorm = normaliserFaculte(assignedFac);
    for (const auto &pair : programmes) {
        if (normaliserFaculte(QString::fromStdString(pair.first)) == assNorm ||
            normaliserFaculte(QString::fromStdString(pair.second.faculte)) == assNorm) {
            return pair.second;
        }
    }
    // Si introuvable, retourne un programme vide
    ProgrammeEtudes vide;
    vide.faculte = assignedFac.toStdString();
    return vide;
}

} // namespace IsolationFaculteUtils
