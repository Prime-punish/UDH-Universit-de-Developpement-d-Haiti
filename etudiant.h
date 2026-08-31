#ifndef COMPTE_ETUDIANT_H
#define COMPTE_ETUDIANT_H

#include <string>
#include <vector>
#include <map>

// ============================================================
//  Structure et base de données des cours par faculté
// ============================================================
struct Annee {
    int numero;
    std::vector<std::string> matieres;
};

struct ProgrammeEtudes {
    std::string faculte;
    std::vector<Annee> annees;
};

std::map<std::string, ProgrammeEtudes> construireProgrammes();

// ============================================================
//  Fonctions utilitaires
// ============================================================
std::string genererId(const std::string &nom, const std::string &prenom);
std::string hacherMotDePasse(const std::string &motDePasse);
std::string nettoyerChamp(std::string champ);

// ============================================================
//  Classe Inscription (contient les infos complémentaires)
// ============================================================
class Inscription {
public:
    std::string nomProche;
    std::string prenomProche;
    std::string telProche;
    std::string faculte;
    std::string pieceIdentitePath;
    std::string photoPath;
    std::string preuvePaiementPath;
    bool aPaye;

    Inscription();
};

// ============================================================
//  Classe CompteEtudiant
// ============================================================
class CompteEtudiant {
private:
    std::string id;
    std::string nom;
    std::string prenom;
    std::string email;
    std::string telephone;
    std::string motDePasseHash;
    bool actif;
    bool inscriptionFaite;
    bool enPause;
    Inscription inscription;

    // Nouveaux champs pour la validation par le Secrétariat
    std::string statutInscription; // "En attente", "Approuvé", "Rejeté"
    std::string dateSoumission;     // ex: "26/08/2026"
    std::string motifRejet;         // Sauvegardé en cas de rejet
    std::string documentsSpec;      // Fichiers complémentaires séparés par ~ et @

public:
    CompteEtudiant(std::string nom, std::string prenom, std::string email, std::string telephone, std::string motDePasse);
    CompteEtudiant();

    void setId(const std::string &v);
    void setNom(const std::string &v);
    void setPrenom(const std::string &v);
    void setEmail(const std::string &v);
    void setTelephone(const std::string &v);
    void setMotDePasseHash(const std::string &v);
    void setActif(bool v);
    void setInscriptionFaite(bool v);
    void setEnPause(bool v);
    void setInscription(const Inscription &insc);
    void setStatutInscription(const std::string &v);
    void setDateSoumission(const std::string &v);
    void setMotifRejet(const std::string &v);
    void setDocumentsSpec(const std::string &v);

    std::string getId() const;
    std::string getNom() const;
    std::string getPrenom() const;
    std::string getEmail() const;
    std::string getTelephone() const;
    bool estActif() const;
    bool estInscrit() const;
    bool estEnPause() const;
    std::string getFaculte() const;
    const Inscription& getInscription() const;
    std::string getStatutInscription() const;
    std::string getDateSoumission() const;
    std::string getMotifRejet() const;
    std::string getDocumentsSpec() const;

    bool verifierMotDePasse(const std::string &motDePasse) const;

    void modifierNom(const std::string &nouveauNom);
    void modifierPrenom(const std::string &nouveauPrenom);
    void modifierTelephone(const std::string &nouveauTelephone);
    void modifierEmail(const std::string &nouvelEmail);
    void modifierMotDePasse(const std::string &nouveauMotDePasse);
    void modifierProche(const std::string &nomProche, const std::string &prenomProche, const std::string &telProche);
    void basculerPause();

    std::string toLigne() const;
};

// ============================================================
//  Persistance des comptes
// ============================================================
void sauvegarderComptes(const std::vector<CompteEtudiant> &comptes);
std::vector<CompteEtudiant> chargerComptes();
void enregistrerCompte(std::vector<CompteEtudiant> &comptes, const CompteEtudiant &compte);

#endif // COMPTE_ETUDIANT_H
