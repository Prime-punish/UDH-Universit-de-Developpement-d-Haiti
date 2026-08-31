#include "administrateur.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>

static const std::string FICHIER_ADMINS = "administrateurs.txt";
static const char SEPARATEUR = '|';

static std::string hacherMotDePasseAdmin(const std::string &motDePasse) {
    unsigned long hash = 5381;
    for (char c : motDePasse) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

static std::string genererIdAdmin(const std::string &nom, const std::string &prenom, const std::string &poste) {
    std::string code = "ADM-";
    if (poste.find("Recteur") != std::string::npos) code += "RC-";
    else if (poste.find("Président") != std::string::npos || poste.find("President") != std::string::npos) code += "PR-";
    else if (poste.find("Secrétaire") != std::string::npos || poste.find("Secretaire") != std::string::npos) code += "SC-";
    else if (poste.find("Coordonnateur") != std::string::npos) code += "CD-";
    else code += "AD-";

    char inN = (!nom.empty()) ? toupper(nom[0]) : 'X';
    char inP = (!prenom.empty()) ? toupper(prenom[0]) : 'X';
    code += inN;
    code += inP;
    code += "-";
    const std::string digits = "0123456789";
    for (int i = 0; i < 4; i++) {
        code += digits[rand() % digits.size()];
    }
    return code;
}

static std::string nettoyerChampAdmin(std::string champ) {
    for (char &c : champ) {
        if (c == SEPARATEUR) c = ';';
    }
    return champ;
}

static std::vector<std::string> decouperAdmin(const std::string &texte, char sep) {
    std::vector<std::string> champs;
    std::stringstream ss(texte);
    std::string champ;
    while (getline(ss, champ, sep)) {
        champs.push_back(champ);
    }
    if (!texte.empty() && texte.back() == sep) {
        champs.push_back("");
    }
    return champs;
}

Administrateur::Administrateur(std::string nom, std::string prenom, std::string telephone, std::string email,
                               std::string poste, std::string faculte, std::string nomProche, std::string prenomProche,
                               std::string telProche, std::string motDePasse)
{
    this->id = genererIdAdmin(nom, prenom, poste);
    this->nom = nom;
    this->prenom = prenom;
    this->telephone = telephone;
    this->email = email;
    this->poste = poste;
    this->faculte = faculte;
    this->nomProche = nomProche;
    this->prenomProche = prenomProche;
    this->telProche = telProche;
    this->motDePasseHash = hacherMotDePasseAdmin(motDePasse);
    this->estPaye = false;
    this->datePaiement = "";
    this->montantSalaire = 0.0;
    this->statutCompte = "Actif";
}

Administrateur::Administrateur() : estPaye(false), datePaiement(""), montantSalaire(0.0), statutCompte("Actif") {}

bool Administrateur::verifierMotDePasse(const std::string &pwd) const {
    return hacherMotDePasseAdmin(pwd) == motDePasseHash;
}

void Administrateur::modifierMotDePasse(const std::string &pwd) {
    motDePasseHash = hacherMotDePasseAdmin(pwd);
}

std::string Administrateur::toLigne() const {
    std::ostringstream oss;
    oss << nettoyerChampAdmin(id) << SEPARATEUR
        << nettoyerChampAdmin(nom) << SEPARATEUR
        << nettoyerChampAdmin(prenom) << SEPARATEUR
        << nettoyerChampAdmin(telephone) << SEPARATEUR
        << nettoyerChampAdmin(email) << SEPARATEUR
        << nettoyerChampAdmin(poste) << SEPARATEUR
        << nettoyerChampAdmin(faculte) << SEPARATEUR
        << nettoyerChampAdmin(nomProche) << SEPARATEUR
        << nettoyerChampAdmin(prenomProche) << SEPARATEUR
        << nettoyerChampAdmin(telProche) << SEPARATEUR
        << nettoyerChampAdmin(motDePasseHash) << SEPARATEUR
        << (estPaye ? 1 : 0) << SEPARATEUR
        << nettoyerChampAdmin(datePaiement) << SEPARATEUR
        << montantSalaire << SEPARATEUR
        << nettoyerChampAdmin(statutCompte.empty() ? "Actif" : statutCompte);
    return oss.str();
}

void sauvegarderAdministrateurs(const std::vector<Administrateur> &admins) {
    std::ofstream fichier(FICHIER_ADMINS, std::ios::out | std::ios::trunc);
    if (!fichier.is_open()) return;
    for (const auto &a : admins) {
        fichier << a.toLigne() << "\n";
    }
    fichier.close();
}

std::vector<Administrateur> chargerAdministrateurs() {
    std::vector<Administrateur> admins;
    std::ifstream fichier(FICHIER_ADMINS);
    if (!fichier.is_open()) return admins;

    std::string ligne;
    while (getline(fichier, ligne)) {
        if (ligne.empty()) continue;
        std::vector<std::string> champs = decouperAdmin(ligne, SEPARATEUR);
        if (champs.size() < 11) continue;

        Administrateur a;
        a.setId(champs[0]);
        a.setNom(champs[1]);
        a.setPrenom(champs[2]);
        a.setTelephone(champs[3]);
        a.setEmail(champs[4]);
        a.setPoste(champs[5]);
        a.setFaculte(champs[6]);
        a.setNomProche(champs[7]);
        a.setPrenomProche(champs[8]);
        a.setTelProche(champs[9]);
        a.setMotDePasseHash(champs[10]);

        if (champs.size() > 11) {
            a.setEstPaye(champs[11] == "1");
        }
        if (champs.size() > 12) {
            a.setDatePaiement(champs[12]);
        }
        if (champs.size() > 13 && !champs[13].empty()) {
            try {
                a.setMontantSalaire(std::stod(champs[13]));
            } catch (...) {
                a.setMontantSalaire(0.0);
            }
        }
        if (champs.size() > 14 && !champs[14].empty()) {
            a.setStatutCompte(champs[14]);
        } else {
            a.setStatutCompte("Actif");
        }

        admins.push_back(a);
    }
    fichier.close();
    return admins;
}

void enregistrerAdministrateur(std::vector<Administrateur> &admins, const Administrateur &admin) {
    for (auto &a : admins) {
        if (a.getId() == admin.getId() || (!a.getEmail().empty() && a.getEmail() == admin.getEmail())) {
            a = admin;
            sauvegarderAdministrateurs(admins);
            return;
        }
    }
    admins.push_back(admin);
    sauvegarderAdministrateurs(admins);
}

bool estPostePris(const std::vector<Administrateur> &admins, const std::string &poste, const std::string &faculte) {
    if (poste == "Recteur" || poste == "Président" || poste == "Secrétaire de l'administration") {
        for (const auto &a : admins) {
            if (a.getPoste() == poste) return true;
        }
    } else if (poste == "Coordonnateur de faculté" && !faculte.empty()) {
        for (const auto &a : admins) {
            if (a.getPoste() == poste && a.getFaculte() == faculte) return true;
        }
    }
    return false;
}
