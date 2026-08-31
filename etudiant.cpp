#include "etudiant.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>

using namespace std;

static const string FICHIER_COMPTES = "comptes.txt";
static const char SEPARATEUR = '|';

// ============================================================
//  Programme des matières par faculté
// ============================================================
map<string, ProgrammeEtudes> construireProgrammes() {
    map<string, ProgrammeEtudes> programmes;

    ProgrammeEtudes gi;
    gi.faculte = "Genie informatique";
    gi.annees = {
        {1, {"Algorithmique et programmation", "Mathematiques generales", "Architecture des ordinateurs",
             "Introduction aux systemes d'exploitation", "Anglais technique", "Communication ecrite"}},
        {2, {"Programmation orientee objet (C++)", "Structures de donnees", "Algebre lineaire",
             "Bases de donnees I", "Reseaux informatiques I", "Electronique numerique"}},
        {3, {"Programmation Java", "Bases de donnees II", "Systemes d'exploitation avances",
             "Reseaux informatiques II", "Genie logiciel", "Probabilites et statistiques"}},
        {4, {"Developpement Web", "Developpement mobile", "Intelligence artificielle",
             "Securite informatique", "Gestion de projets informatiques", "Systemes distribues"}},
        {5, {"Cloud computing", "Architecture logicielle avancee", "Data science et Big Data",
             "Stage professionnel", "Projet de fin d'etudes", "Ethique et droit du numerique"}}
    };
    programmes["Genie informatique"] = gi;

    ProgrammeEtudes gc;
    gc.faculte = "Genie civil";
    gc.annees = {
        {1, {"Mathematiques generales", "Physique generale", "Dessin technique",
             "Chimie appliquee", "Informatique de base", "Communication ecrite"}},
        {2, {"Resistance des materiaux I", "Topographie", "Mecanique des sols I",
             "Mathematiques appliquees", "Materiaux de construction", "Anglais technique"}},
        {3, {"Resistance des materiaux II", "Beton arme I", "Mecanique des sols II",
             "Hydraulique generale", "Construction metallique", "Geotechnique"}},
        {4, {"Beton arme II", "Structures avancees", "Voirie et reseaux divers (VRD)",
             "Gestion de chantier", "Hydrologie", "Etudes d'impact environnemental"}},
        {5, {"Ouvrages d'art", "Batiments a etages", "Estimation et devis",
             "Reglementation et normes du batiment", "Stage professionnel", "Projet de fin d'etudes"}}
    };
    programmes["Genie civil"] = gc;

    ProgrammeEtudes sec;
    sec.faculte = "Science economique et comptabilite";
    sec.annees = {
        {1, {"Introduction a l'economie", "Comptabilite generale I", "Mathematiques financieres",
             "Statistiques descriptives", "Droit des affaires I", "Communication ecrite"}},
        {2, {"Microeconomie", "Comptabilite generale II", "Macroeconomie",
             "Statistiques inferentielles", "Droit des affaires II", "Anglais des affaires"}},
        {3, {"Comptabilite analytique", "Finance d'entreprise", "Fiscalite",
             "Economie internationale", "Analyse financiere", "Marketing"}},
        {4, {"Audit et controle de gestion", "Comptabilite des societes", "Gestion budgetaire",
             "Economie du developpement", "Stage professionnel", "Projet de fin d'etudes"}}
    };
    programmes["Science economique et comptabilite"] = sec;

    ProgrammeEtudes sa;
    sa.faculte = "Science administration";
    sa.annees = {
        {1, {"Introduction au management", "Comptabilite generale", "Mathematiques appliquees a la gestion",
             "Sociologie des organisations", "Droit civil", "Communication ecrite"}},
        {2, {"Gestion des ressources humaines I", "Marketing I", "Microeconomie",
             "Statistiques appliquees", "Droit du travail", "Anglais des affaires"}},
        {3, {"Gestion des ressources humaines II", "Marketing II", "Gestion financiere",
             "Management strategique I", "Systemes d'information de gestion", "Communication organisationnelle"}},
        {4, {"Management strategique II", "Gestion de projets", "Entrepreneuriat",
             "Ethique et responsabilite sociale", "Stage professionnel", "Projet de fin d'etudes"}}
    };
    programmes["Science administration"] = sa;

    ProgrammeEtudes sj;
    sj.faculte = "Science juridique";
    sj.annees = {
        {1, {"Introduction au droit", "Droit constitutionnel", "Histoire des institutions",
             "Sociologie generale", "Methodologie juridique", "Communication ecrite"}},
        {2, {"Droit civil (personnes et biens)", "Droit administratif I", "Droit penal general",
             "Droit international public I", "Institutions judiciaires", "Anglais juridique"}},
        {3, {"Droit des obligations", "Droit administratif II", "Droit penal special",
             "Droit international public II", "Droit du travail", "Procedure civile"}},
        {4, {"Droit des affaires", "Droit fiscal", "Droit des contrats speciaux",
             "Procedure penale", "Stage professionnel", "Projet de fin d'etudes"}}
    };
    programmes["Science juridique"] = sj;

    ProgrammeEtudes se;
    se.faculte = "Science education";
    se.annees = {
        {1, {"Introduction aux sciences de l'education", "Psychologie generale", "Sociologie de l'education",
             "Communication ecrite", "Philosophie de l'education", "Methodologie du travail universitaire"}},
        {2, {"Psychologie de l'enfant et de l'adolescent", "Didactique generale", "Pedagogie generale",
             "Histoire de l'education", "Statistiques appliquees a l'education", "Anglais pedagogique"}},
        {3, {"Didactique des disciplines", "Evaluation des apprentissages", "Gestion de classe",
             "Psychologie de l'apprentissage", "Technologies educatives", "Politique educative"}},
        {4, {"Ingenierie de la formation", "Education inclusive", "Recherche en education",
             "Stage pratique en milieu scolaire", "Projet de fin d'etudes", "Ethique professionnelle"}}
    };
    programmes["Science education"] = se;

    return programmes;
}

string genererId(const string &nom, const string &prenom) {
    string id = "ETU-";
    char inN = (!nom.empty()) ? toupper(nom[0]) : 'X';
    char inP = (!prenom.empty()) ? toupper(prenom[0]) : 'X';
    id += inN;
    id += inP;
    id += "-";
    const string digits = "0123456789";
    for (int i = 0; i < 5; i++) {
        id += digits[rand() % digits.size()];
    }
    return id;
}

string hacherMotDePasse(const string &motDePasse) {
    unsigned long hash = 5381;
    for (char c : motDePasse) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    ostringstream oss;
    oss << hex << hash;
    return oss.str();
}

string nettoyerChamp(string champ) {
    for (char &c : champ) {
        if (c == SEPARATEUR) c = ';';
    }
    return champ;
}

// ============================================================
//  Classe Inscription
// ============================================================

Inscription::Inscription() : aPaye(false) {}

// ============================================================
//  Classe CompteEtudiant
// ============================================================

CompteEtudiant::CompteEtudiant(string nom, string prenom, string email, string telephone, string motDePasse) {
    this->id = genererId(nom, prenom);
    this->nom = nom;
    this->prenom = prenom;
    this->email = email;
    this->telephone = telephone;
    this->motDePasseHash = hacherMotDePasse(motDePasse);
    this->actif = false;         
    this->inscriptionFaite = false;
    this->enPause = false;
    this->statutInscription = "En attente";
    this->dateSoumission = "";
    this->motifRejet = "";
    this->documentsSpec = "";
}

CompteEtudiant::CompteEtudiant() 
    : actif(false), inscriptionFaite(false), enPause(false), statutInscription("En attente"), dateSoumission(""), motifRejet(""), documentsSpec("") {}

void CompteEtudiant::setId(const string &v) { id = v; }
void CompteEtudiant::setNom(const string &v) { nom = v; }
void CompteEtudiant::setPrenom(const string &v) { prenom = v; }
void CompteEtudiant::setEmail(const string &v) { email = v; }
void CompteEtudiant::setTelephone(const string &v) { telephone = v; }
void CompteEtudiant::setMotDePasseHash(const string &v) { motDePasseHash = v; }
void CompteEtudiant::setActif(bool v) { actif = v; }
void CompteEtudiant::setInscriptionFaite(bool v) { inscriptionFaite = v; }
void CompteEtudiant::setEnPause(bool v) { enPause = v; }
void CompteEtudiant::setInscription(const Inscription &insc) { inscription = insc; }
void CompteEtudiant::setStatutInscription(const string &v) { statutInscription = v; }
void CompteEtudiant::setDateSoumission(const string &v) { dateSoumission = v; }
void CompteEtudiant::setMotifRejet(const string &v) { motifRejet = v; }
void CompteEtudiant::setDocumentsSpec(const string &v) { documentsSpec = v; }

string CompteEtudiant::getId() const { return id; }
string CompteEtudiant::getNom() const { return nom; }
string CompteEtudiant::getPrenom() const { return prenom; }
string CompteEtudiant::getEmail() const { return email; }
string CompteEtudiant::getTelephone() const { return telephone; }
bool CompteEtudiant::estActif() const { return actif; }
bool CompteEtudiant::estInscrit() const { return inscriptionFaite; }
bool CompteEtudiant::estEnPause() const { return enPause; }
string CompteEtudiant::getFaculte() const { return inscription.faculte; }
const Inscription& CompteEtudiant::getInscription() const { return inscription; }
string CompteEtudiant::getStatutInscription() const { return statutInscription; }
string CompteEtudiant::getDateSoumission() const { return dateSoumission; }
string CompteEtudiant::getMotifRejet() const { return motifRejet; }
string CompteEtudiant::getDocumentsSpec() const { return documentsSpec; }

bool CompteEtudiant::verifierMotDePasse(const string &motDePasse) const {
    return hacherMotDePasse(motDePasse) == motDePasseHash;
}

void CompteEtudiant::modifierNom(const string &nouveauNom) {
    char ancienneInitiale = (!nom.empty()) ? toupper(nom[0]) : 'X';
    char nouvelleInitiale = (!nouveauNom.empty()) ? toupper(nouveauNom[0]) : 'X';
    nom = nouveauNom;
    if (ancienneInitiale != nouvelleInitiale) {
        id = genererId(nom, prenom);
    }
}

void CompteEtudiant::modifierPrenom(const string &nouveauPrenom) {
    char ancienneInitiale = (!prenom.empty()) ? toupper(prenom[0]) : 'X';
    char nouvelleInitiale = (!nouveauPrenom.empty()) ? toupper(nouveauPrenom[0]) : 'X';
    prenom = nouveauPrenom;
    if (ancienneInitiale != nouvelleInitiale) {
        id = genererId(nom, prenom);
    }
}

void CompteEtudiant::modifierTelephone(const string &nouveauTelephone) { telephone = nouveauTelephone; }
void CompteEtudiant::modifierEmail(const string &nouvelEmail) { email = nouvelEmail; }

void CompteEtudiant::modifierMotDePasse(const string &nouveauMotDePasse) {
    motDePasseHash = hacherMotDePasse(nouveauMotDePasse);
}

void CompteEtudiant::modifierProche(const string &nomProche, const string &prenomProche, const string &telProche) {
    inscription.nomProche = nomProche;
    inscription.prenomProche = prenomProche;
    inscription.telProche = telProche;
}

void CompteEtudiant::basculerPause() { enPause = !enPause; }

string CompteEtudiant::toLigne() const {
    ostringstream oss;
    oss << nettoyerChamp(id) << SEPARATEUR
        << nettoyerChamp(nom) << SEPARATEUR
        << nettoyerChamp(prenom) << SEPARATEUR
        << nettoyerChamp(email) << SEPARATEUR
        << nettoyerChamp(telephone) << SEPARATEUR
        << nettoyerChamp(motDePasseHash) << SEPARATEUR
        << (actif ? 1 : 0) << SEPARATEUR
        << (inscriptionFaite ? 1 : 0) << SEPARATEUR
        << (enPause ? 1 : 0) << SEPARATEUR
        << nettoyerChamp(inscription.nomProche) << SEPARATEUR
        << nettoyerChamp(inscription.prenomProche) << SEPARATEUR
        << nettoyerChamp(inscription.telProche) << SEPARATEUR
        << nettoyerChamp(inscription.faculte) << SEPARATEUR
        << nettoyerChamp(inscription.pieceIdentitePath) << SEPARATEUR
        << nettoyerChamp(inscription.photoPath) << SEPARATEUR
        << (inscription.aPaye ? 1 : 0) << SEPARATEUR
        << nettoyerChamp(inscription.preuvePaiementPath) << SEPARATEUR
        << nettoyerChamp(statutInscription) << SEPARATEUR
        << nettoyerChamp(dateSoumission) << SEPARATEUR
        << nettoyerChamp(motifRejet) << SEPARATEUR
        << nettoyerChamp(documentsSpec);
    return oss.str();
}

// ============================================================
//  Persistance des comptes (fichier texte "comptes.txt")
// ============================================================

vector<string> decouper(const string &ligneTexte, char sep) {
    vector<string> champs;
    stringstream ss(ligneTexte);
    string champ;
    while (getline(ss, champ, sep)) {
        champs.push_back(champ);
    }
    if (!ligneTexte.empty() && ligneTexte.back() == sep) {
        champs.push_back("");
    }
    return champs;
}

void sauvegarderComptes(const vector<CompteEtudiant> &comptes) {
    ofstream fichier(FICHIER_COMPTES, ios::out | ios::trunc);
    if (!fichier.is_open()) return;
    for (const auto &c : comptes) {
        fichier << c.toLigne() << "\n";
    }
    fichier.close();
}

vector<CompteEtudiant> chargerComptes() {
    vector<CompteEtudiant> comptes;
    ifstream fichier(FICHIER_COMPTES);
    if (!fichier.is_open()) return comptes;

    string ligneTexte;
    while (getline(fichier, ligneTexte)) {
        if (ligneTexte.empty()) continue;
        try {
            vector<string> champs = decouper(ligneTexte, SEPARATEUR);
            if (champs.size() < 6) continue;

            CompteEtudiant c;
            c.setId(champs[0]);
            c.setNom(champs[1]);
            c.setPrenom(champs[2]);
            c.setEmail(champs[3]);
            c.setTelephone(champs[4]);
            c.setMotDePasseHash(champs[5]);
            c.setActif(champs.size() > 6 ? (champs[6] == "1") : true);
            c.setInscriptionFaite(champs.size() > 7 ? (champs[7] == "1") : false);
            c.setEnPause(champs.size() > 8 ? (champs[8] == "1") : false);

            Inscription insc;
            if (champs.size() > 9) insc.nomProche = champs[9];
            if (champs.size() > 10) insc.prenomProche = champs[10];
            if (champs.size() > 11) insc.telProche = champs[11];
            if (champs.size() > 12) insc.faculte = champs[12];
            if (champs.size() > 13) insc.pieceIdentitePath = champs[13];
            if (champs.size() > 14) insc.photoPath = champs[14];
            if (champs.size() > 15) insc.aPaye = (champs[15] == "1");
            if (champs.size() > 16) insc.preuvePaiementPath = champs[16];
            c.setInscription(insc);

            if (champs.size() > 17 && !champs[17].empty()) {
                c.setStatutInscription(champs[17]);
            } else {
                c.setStatutInscription(c.estActif() ? "Approuvé" : "En attente");
            }

            if (champs.size() > 18) {
                c.setDateSoumission(champs[18]);
            }

            if (champs.size() > 19) {
                c.setMotifRejet(champs[19]);
            }

            if (champs.size() > 20) {
                c.setDocumentsSpec(champs[20]);
            }

            comptes.push_back(c);
        } catch (...) {
            // Ignore malformed line gracefully
        }
    }
    fichier.close();
    return comptes;
}

void enregistrerCompte(vector<CompteEtudiant> &comptes, const CompteEtudiant &compte) {
    for (auto &c : comptes) {
        if (c.getId() == compte.getId() || (!c.getEmail().empty() && c.getEmail() == compte.getEmail())) {
            c = compte;
            sauvegarderComptes(comptes);
            return;
        }
    }
    comptes.push_back(compte);
    sauvegarderComptes(comptes);
}