#include "professeur.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>

static const std::string FICHIER_PROFESSEURS = "professeurs.txt";
static const std::string FICHIER_COMPTES_ETUDIANTS = "comptes.txt";
static const char SEPARATEUR = '|';

static std::string hacherMotDePasseProf(const std::string &motDePasse) {
    unsigned long hash = 5381;
    for (char c : motDePasse) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

static std::string genererIdProf(const std::string &nom, const std::string &prenom) {
    std::string id = "PROF-";
    char initialeNom = (!nom.empty()) ? toupper(nom[0]) : 'X';
    char initialePrenom = (!prenom.empty()) ? toupper(prenom[0]) : 'X';
    id += initialeNom;
    id += initialePrenom;
    id += "-";
    const std::string chiffres = "0123456789";
    for (int i = 0; i < 5; i++) {
        id += chiffres[rand() % chiffres.size()];
    }
    return id;
}

static std::string nettoyerChampProf(std::string champ) {
    for (char &c : champ) {
        if (c == SEPARATEUR) c = ';';
    }
    return champ;
}

static std::vector<std::string> decouperProf(const std::string &texte, char sep) {
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

Professeur::Professeur(std::string nom, std::string prenom, std::string email, std::string telephone,
                       std::string motDePasse, std::string faculte, std::vector<std::string> matieres) {
    this->id = genererIdProf(nom, prenom);
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->telephone = telephone;
    this->motDePasseHash = hacherMotDePasseProf(motDePasse);
    this->faculte = faculte;
    this->matieres = matieres;
    this->valide = false;
    this->estPaye = false;
    this->datePaiement = "";
    this->montantSalaire = 0.0;
    this->statutCompte = "Actif";
}

Professeur::Professeur() : valide(false), estPaye(false), datePaiement(""), montantSalaire(0.0), statutCompte("Actif") {}

bool Professeur::verifierMotDePasse(const std::string &motDePasse) const {
    return hacherMotDePasseProf(motDePasse) == motDePasseHash;
}

void Professeur::modifierMotDePasse(const std::string &nouveauMotDePasse) {
    motDePasseHash = hacherMotDePasseProf(nouveauMotDePasse);
}

void Professeur::modifierNom(const std::string &nouveauNom) {
    char ancienneInitiale = (!nom.empty()) ? toupper(nom[0]) : 'X';
    char nouvelleInitiale = (!nouveauNom.empty()) ? toupper(nouveauNom[0]) : 'X';
    nom = nouveauNom;
    if (ancienneInitiale != nouvelleInitiale) {
        id = genererIdProf(nom, prenom);
    }
}

void Professeur::modifierPrenom(const std::string &nouveauPrenom) {
    char ancienneInitiale = (!prenom.empty()) ? toupper(prenom[0]) : 'X';
    char nouvelleInitiale = (!nouveauPrenom.empty()) ? toupper(nouveauPrenom[0]) : 'X';
    prenom = nouveauPrenom;
    if (ancienneInitiale != nouvelleInitiale) {
        id = genererIdProf(nom, prenom);
    }
}

void Professeur::attribuerNote(const std::string &idEtudiant, const std::string &matiere, double valeur, const std::string &statut) {
    for (Note &n : notes) {
        if (n.idEtudiant == idEtudiant && n.matiere == matiere) {
            n.valeur = valeur;
            n.statut = statut;
            return;
        }
    }
    Note n(idEtudiant, matiere, valeur, statut);
    notes.push_back(n);
}

void Professeur::ajouterPhotoExamen(const std::string &matiere, const std::string &cheminFichier, const std::string &dateAjout) {
    ExamenPhoto p;
    p.matiere = matiere;
    p.cheminFichier = cheminFichier;
    p.dateAjout = dateAjout;
    photosExamens.push_back(p);
}

std::string Professeur::toLigne() const {
    std::ostringstream oss;
    oss << nettoyerChampProf(id) << SEPARATEUR
        << nettoyerChampProf(nom) << SEPARATEUR
        << nettoyerChampProf(prenom) << SEPARATEUR
        << nettoyerChampProf(email) << SEPARATEUR
        << nettoyerChampProf(telephone) << SEPARATEUR
        << nettoyerChampProf(motDePasseHash) << SEPARATEUR
        << nettoyerChampProf(faculte) << SEPARATEUR
        << (valide ? 1 : 0) << SEPARATEUR;

    for (size_t i = 0; i < matieres.size(); i++) {
        oss << nettoyerChampProf(matieres[i]);
        if (i + 1 < matieres.size()) oss << ";";
    }
    oss << SEPARATEUR;

    for (size_t i = 0; i < notes.size(); i++) {
        oss << notes[i].toChamp();
        if (i + 1 < notes.size()) oss << "^";
    }
    oss << SEPARATEUR;

    for (size_t i = 0; i < photosExamens.size(); i++) {
        oss << photosExamens[i].toChamp();
        if (i + 1 < photosExamens.size()) oss << "@";
    }
    oss << SEPARATEUR;

    // 12th, 13th, 14th & 15th fields
    oss << (estPaye ? 1 : 0) << SEPARATEUR
        << nettoyerChampProf(datePaiement) << SEPARATEUR
        << montantSalaire << SEPARATEUR
        << nettoyerChampProf(statutCompte.empty() ? "Actif" : statutCompte);

    return oss.str();
}

void sauvegarderProfesseurs(const std::vector<Professeur> &profs) {
    std::ofstream fichier(FICHIER_PROFESSEURS, std::ios::out | std::ios::trunc);
    if (!fichier.is_open()) return;
    for (const auto &p : profs) {
        fichier << p.toLigne() << "\n";
    }
    fichier.close();
}

std::vector<Professeur> chargerProfesseurs() {
    std::vector<Professeur> profs;
    std::ifstream fichier(FICHIER_PROFESSEURS);
    if (!fichier.is_open()) return profs;

    std::string ligneTexte;
    while (getline(fichier, ligneTexte)) {
        if (ligneTexte.empty()) continue;
        std::vector<std::string> champs = decouperProf(ligneTexte, SEPARATEUR);
        if (champs.size() < 8) continue;

        Professeur p;
        p.setId(champs[0]);
        p.setNom(champs[1]);
        p.setPrenom(champs[2]);
        p.setEmail(champs[3]);
        p.setTelephone(champs[4]);
        p.setMotDePasseHash(champs[5]);
        p.setFaculte(champs[6]);
        p.setValide(champs[7] == "1");

        if (champs.size() > 8 && !champs[8].empty()) {
            std::vector<std::string> matieres;
            std::stringstream ss(champs[8]);
            std::string m;
            while (getline(ss, m, ';')) {
                if (!m.empty()) matieres.push_back(m);
            }
            p.setMatieres(matieres);
        }

        if (champs.size() > 9 && !champs[9].empty()) {
            std::vector<Note> notes;
            std::stringstream ssNotes(champs[9]);
            std::string entreeNote;
            while (getline(ssNotes, entreeNote, '^')) {
                std::vector<std::string> partiesNote = decouperProf(entreeNote, '~');
                if (partiesNote.size() >= 3) {
                    Note n;
                    n.idEtudiant = partiesNote[0];
                    n.matiere = partiesNote[1];
                    try {
                        n.valeur = std::stod(partiesNote[2]);
                    } catch (...) {
                        n.valeur = 0;
                    }
                    if (partiesNote.size() >= 4 && !partiesNote[3].empty()) {
                        n.statut = partiesNote[3];
                    } else {
                        n.statut = "Reçue";
                    }
                    notes.push_back(n);
                }
            }
            p.setNotes(notes);
        }

        if (champs.size() > 10 && !champs[10].empty()) {
            std::vector<ExamenPhoto> photos;
            std::stringstream ssPhotos(champs[10]);
            std::string entreePhoto;
            while (getline(ssPhotos, entreePhoto, '@')) {
                std::vector<std::string> partiesPhoto = decouperProf(entreePhoto, '~');
                if (partiesPhoto.size() == 3) {
                    ExamenPhoto ph;
                    ph.matiere = partiesPhoto[0];
                    ph.cheminFichier = partiesPhoto[1];
                    ph.dateAjout = partiesPhoto[2];
                    photos.push_back(ph);
                }
            }
            p.setPhotosExamens(photos);
        }

        if (champs.size() > 11) {
            p.setEstPaye(champs[11] == "1");
        }
        if (champs.size() > 12) {
            p.setDatePaiement(champs[12]);
        }
        if (champs.size() > 13 && !champs[13].empty()) {
            try {
                p.setMontantSalaire(std::stod(champs[13]));
            } catch (...) {
                p.setMontantSalaire(0.0);
            }
        }
        if (champs.size() > 14 && !champs[14].empty()) {
            p.setStatutCompte(champs[14]);
        } else {
            p.setStatutCompte("Actif");
        }

        profs.push_back(p);
    }
    fichier.close();
    return profs;
}

void enregistrerProfesseur(std::vector<Professeur> &profs, const Professeur &prof) {
    for (auto &p : profs) {
        if (p.getId() == prof.getId() || (!p.getEmail().empty() && p.getEmail() == prof.getEmail())) {
            p = prof;
            sauvegarderProfesseurs(profs);
            return;
        }
    }
    profs.push_back(prof);
    sauvegarderProfesseurs(profs);
}

bool rechercherEtudiant(const std::string &idEtudiant, std::string &nomComplet) {
    std::ifstream fichier(FICHIER_COMPTES_ETUDIANTS);
    if (!fichier.is_open()) return false;
    std::string ligneTexte;
    while (getline(fichier, ligneTexte)) {
        if (ligneTexte.empty()) continue;
        std::vector<std::string> champs = decouperProf(ligneTexte, SEPARATEUR);
        if (champs.size() < 3) continue;
        if (champs[0] == idEtudiant) {
            nomComplet = champs[1] + " " + champs[2];
            fichier.close();
            return true;
        }
    }
    fichier.close();
    return false;
}
