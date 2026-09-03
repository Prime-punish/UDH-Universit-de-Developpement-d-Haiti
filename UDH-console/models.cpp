#include "models.h"
#include <ctime>
#include <iomanip>

using namespace std;

static const char SEPARATEUR = '|';
static const string FICHIER_COMPTES = "comptes.txt";
static const string FICHIER_PROFS = "professeurs.txt";
static const string FICHIER_ADMINS = "administrateurs.txt";
static const string FICHIER_VERSEMENTS = "versements.txt";
static const string FICHIER_COURRIERS = "courriers.txt";
static const string FICHIER_ANNONCES = "annonces.txt";
static const string FICHIER_EVENTS = "evenements.txt";

// ============================================================
//  Utilitaires
// ============================================================
string nettoyerChamp(string champ) {
    for (char &c : champ) {
        if (c == SEPARATEUR || c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return champ;
}

string genererId(const string &nom, const string &prenom) {
    string id = "ETU-";
    id += (nom.empty() ? 'X' : (char)toupper(nom[0]));
    id += (prenom.empty() ? 'X' : (char)toupper(prenom[0]));
    id += "-";
    int r = 10000 + (rand() % 90000);
    id += to_string(r);
    return id;
}

string hacherMotDePasse(const string &motDePasse) {
    unsigned long hash = 5381;
    for (char c : motDePasse) {
        hash = ((hash << 5) + hash) + (unsigned char)c;
    }
    ostringstream oss;
    oss << hex << hash;
    return oss.str();
}

string obtenirDateHeureActuelle() {
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", now);
    return string(buf);
}

static vector<string> decouperLigne(const string &ligne, char sep = SEPARATEUR) {
    vector<string> champs;
    string champ;
    istringstream flux(ligne);
    while (getline(flux, champ, sep)) {
        champs.push_back(champ);
    }
    return champs;
}

// ============================================================
//  Programme d'études
// ============================================================
map<string, ProgrammeEtudes> construireProgrammes() {
    map<string, ProgrammeEtudes> p;

    ProgrammeEtudes gi;
    gi.faculte = "Genie informatique";
    gi.annees = {
        {1, {"Algorithmique & Programmation C++", "Mathematiques generales", "Architecture des ordinateurs", "Systemes d'exploitation", "Anglais technique"}},
        {2, {"Structures de donnees & POO", "Bases de donnees relationnelles", "Algebre lineaire", "Reseaux informatiques I", "Electronique numerique"}},
        {3, {"Programmation Java / Python", "Genie logiciel & UML", "Bases de donnees avancees", "Reseaux II & Securite", "Probabilites et statistiques"}},
        {4, {"Developpement Web & Mobile", "Intelligence Artificielle", "Systemes distribues", "Cybersecurite", "Gestion de projets agiles"}},
        {5, {"Cloud computing & DevOps", "Data Science & Big Data", "Architecture logicielle", "Stage professionnel", "Projet de fin d'etudes (PFE)"}}
    };
    p["Genie informatique"] = gi;

    ProgrammeEtudes gc;
    gc.faculte = "Genie civil";
    gc.annees = {
        {1, {"Mathematiques generales", "Physique mecanique", "Dessin technique & DAO", "Chimie appliquee", "Communication scientifique"}},
        {2, {"Resistance des materiaux I", "Topographie", "Mecanique des sols I", "Materiaux de construction", "Anglais technique"}},
        {3, {"Resistance des materiaux II", "Beton arme I", "Mecanique des sols II", "Hydraulique generale", "Geotechnique"}},
        {4, {"Beton arme II", "Structures metalliques", "Voirie et reseaux divers (VRD)", "Gestion de chantier", "Hydrologie"}},
        {5, {"Ouvrages d'art & Ponts", "Batiments parasismiques", "Devis et estimation", "Stage professionnel", "Projet de fin d'etudes (PFE)"}}
    };
    p["Genie civil"] = gc;

    ProgrammeEtudes sec;
    sec.faculte = "Science economique et comptabilite";
    sec.annees = {
        {1, {"Introduction a l'economie", "Comptabilite generale I", "Mathematiques financieres", "Statistiques descriptives", "Droit des affaires I"}},
        {2, {"Microeconomie", "Macroeconomie", "Comptabilite generale II", "Statistiques inferentielles", "Anglais des affaires"}},
        {3, {"Comptabilite analytique", "Finance d'entreprise", "Fiscalite appliquee", "Economie internationale", "Analyse financiere"}},
        {4, {"Audit et controle de gestion", "Comptabilite des societes", "Gestion budgetaire", "Economie du developpement", "Stage & PFE"}}
    };
    p["Science economique et comptabilite"] = sec;

    ProgrammeEtudes sa;
    sa.faculte = "Science administration";
    sa.annees = {
        {1, {"Introduction au management", "Comptabilite generale", "Mathematiques appliquees", "Sociologie des organisations", "Droit civil"}},
        {2, {"Gestion des ressources humaines I", "Marketing fondamental", "Microeconomie", "Statistiques", "Droit du travail"}},
        {3, {"Management strategique I", "Gestion financiere", "Marketing digital", "Systemes d'information", "Communication d'entreprise"}},
        {4, {"Management strategique II", "Gestion de projets", "Entrepreneuriat", "Ethique & Gouvernance", "Stage & PFE"}}
    };
    p["Science administration"] = sa;

    ProgrammeEtudes sj;
    sj.faculte = "Science juridique";
    sj.annees = {
        {1, {"Introduction au droit", "Droit constitutionnel", "Histoire du droit", "Sociologie juridique", "Methodologie"}},
        {2, {"Droit civil (personnes & biens)", "Droit administratif I", "Droit penal general", "Droit international public", "Anglais juridique"}},
        {3, {"Droit des obligations", "Droit administratif II", "Droit penal special", "Droit du travail", "Procedure civile"}},
        {4, {"Droit des affaires", "Droit fiscal", "Contrats speciaux", "Procedure penale", "Stage professionnel & PFE"}}
    };
    p["Science juridique"] = sj;

    ProgrammeEtudes se;
    se.faculte = "Science education";
    se.annees = {
        {1, {"Introduction aux sciences de l'education", "Psychologie generale", "Sociologie de l'education", "Philosophie de l'education"}},
        {2, {"Psychologie du developpement", "Didactique generale", "Pedagogie appliquee", "Statistiques appliquees"}},
        {3, {"Evaluation des apprentissages", "Technologie educative", "Didactique speciale", "Administration scolaire"}},
        {4, {"Planification de l'education", "Education comparee", "Recherche pedagogique", "Stage en milieu scolaire & PFE"}}
    };
    p["Science education"] = se;

    return p;
}

// ============================================================
//  Inscription & Compte Étudiant
// ============================================================
Inscription::Inscription() : aPaye(false) {}

CompteEtudiant::CompteEtudiant()
    : actif(false), inscriptionFaite(false), enPause(false), statutInscription("En attente") {}

CompteEtudiant::CompteEtudiant(string n, string p, string e, string t, string mdp)
    : nom(n), prenom(p), email(e), telephone(t),
      motDePasseHash(hacherMotDePasse(mdp)), actif(false), inscriptionFaite(false), enPause(false),
      statutInscription("En attente") {
    id = genererId(nom, prenom);
}

bool CompteEtudiant::verifierMotDePasse(const string &motDePasse) const {
    return hacherMotDePasse(motDePasse) == motDePasseHash;
}

void CompteEtudiant::modifierMotDePasse(const string &nouveau) {
    motDePasseHash = hacherMotDePasse(nouveau);
}

string CompteEtudiant::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << nettoyerChamp(nom) << SEPARATEUR
        << nettoyerChamp(prenom) << SEPARATEUR
        << nettoyerChamp(email) << SEPARATEUR
        << nettoyerChamp(telephone) << SEPARATEUR
        << motDePasseHash << SEPARATEUR
        << (actif ? "1" : "0") << SEPARATEUR
        << (inscriptionFaite ? "1" : "0") << SEPARATEUR
        << (enPause ? "1" : "0") << SEPARATEUR
        << nettoyerChamp(inscription.nomProche) << SEPARATEUR
        << nettoyerChamp(inscription.prenomProche) << SEPARATEUR
        << nettoyerChamp(inscription.telProche) << SEPARATEUR
        << nettoyerChamp(inscription.faculte) << SEPARATEUR
        << nettoyerChamp(inscription.pieceIdentitePath) << SEPARATEUR
        << nettoyerChamp(inscription.photoPath) << SEPARATEUR
        << (inscription.aPaye ? "1" : "0") << SEPARATEUR
        << nettoyerChamp(inscription.preuvePaiementPath) << SEPARATEUR
        << (statutInscription.empty() ? "En attente" : statutInscription) << SEPARATEUR
        << (dateSoumission.empty() ? "N/A" : dateSoumission) << SEPARATEUR
        << nettoyerChamp(motifRejet) << SEPARATEUR
        << nettoyerChamp(documentsSpec);
    return oss.str();
}

vector<CompteEtudiant> chargerComptes() {
    vector<CompteEtudiant> comptes;
    ifstream fichier(FICHIER_COMPTES);
    if (!fichier.is_open()) return comptes;

    string ligne;
    while (getline(fichier, ligne)) {
        if (ligne.empty()) continue;
        vector<string> champs = decouperLigne(ligne);
        if (champs.size() < 6) continue;

        CompteEtudiant c;
        c.setId(champs[0]);
        c.setNom(champs[1]);
        c.setPrenom(champs[2]);
        c.setEmail(champs[3]);
        c.setTelephone(champs[4]);
        c.setMotDePasseHash(champs[5]);
        c.setActif(champs.size() > 6 && champs[6] == "1");
        c.setInscriptionFaite(champs.size() > 7 && champs[7] == "1");
        c.setEnPause(champs.size() > 8 && champs[8] == "1");

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

        if (champs.size() > 17) c.setStatutInscription(champs[17]);
        if (champs.size() > 18) c.setDateSoumission(champs[18]);
        if (champs.size() > 19) c.setMotifRejet(champs[19]);
        if (champs.size() > 20) c.setDocumentsSpec(champs[20]);

        comptes.push_back(c);
    }
    return comptes;
}

void sauvegarderComptes(const vector<CompteEtudiant> &comptes) {
    ofstream fichier(FICHIER_COMPTES, ios::trunc);
    if (!fichier.is_open()) return;
    for (const auto &c : comptes) {
        fichier << c.toLigne() << "\n";
    }
}

void enregistrerCompte(vector<CompteEtudiant> &comptes, const CompteEtudiant &compte) {
    bool trouve = false;
    for (auto &c : comptes) {
        if (c.getId() == compte.getId()) {
            c = compte;
            trouve = true;
            break;
        }
    }
    if (!trouve) {
        comptes.push_back(compte);
    }
    sauvegarderComptes(comptes);
}

// ============================================================
//  Professeur
// ============================================================
Professeur::Professeur()
    : valide(false), estPaye(false), montantSalaire(0.0), statutCompte("Actif") {}

Professeur::Professeur(string n, string p, string e, string t, string mdp, string fac, vector<string> mat)
    : nom(n), prenom(p), email(e), telephone(t), motDePasseHash(hacherMotDePasse(mdp)),
      faculte(fac), matieres(mat), valide(false), estPaye(false), montantSalaire(0.0), statutCompte("Actif") {
    string pid = "PRF-";
    pid += (nom.empty() ? 'X' : (char)toupper(nom[0]));
    pid += (prenom.empty() ? 'X' : (char)toupper(prenom[0]));
    pid += "-";
    int r = 1000 + (rand() % 9000);
    pid += to_string(r);
    id = pid;
}

bool Professeur::verifierMotDePasse(const string &motDePasse) const {
    return hacherMotDePasse(motDePasse) == motDePasseHash;
}

void Professeur::modifierMotDePasse(const string &nouveau) {
    motDePasseHash = hacherMotDePasse(nouveau);
}

void Professeur::attribuerNote(const string &idEtudiant, const string &matiere, double valeur, const string &statut) {
    bool maj = false;
    for (auto &n : notes) {
        if (n.idEtudiant == idEtudiant && n.matiere == matiere) {
            n.valeur = valeur;
            n.statut = statut;
            maj = true;
            break;
        }
    }
    if (!maj) {
        notes.push_back(Note(idEtudiant, matiere, valeur, statut));
    }
}

void Professeur::ajouterPhotoExamen(const string &matiere, const string &chemin, const string &dateAjout) {
    ExamenPhoto ep;
    ep.matiere = matiere;
    ep.cheminFichier = chemin;
    ep.dateAjout = dateAjout;
    photosExamens.push_back(ep);
}

string Professeur::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << nettoyerChamp(nom) << SEPARATEUR
        << nettoyerChamp(prenom) << SEPARATEUR
        << nettoyerChamp(email) << SEPARATEUR
        << nettoyerChamp(telephone) << SEPARATEUR
        << motDePasseHash << SEPARATEUR
        << nettoyerChamp(faculte) << SEPARATEUR;

    for (size_t i = 0; i < matieres.size(); ++i) {
        if (i > 0) oss << ",";
        oss << matieres[i];
    }
    oss << SEPARATEUR << (valide ? "1" : "0") << SEPARATEUR;

    for (size_t i = 0; i < notes.size(); ++i) {
        if (i > 0) oss << ";";
        oss << notes[i].toChamp();
    }
    oss << SEPARATEUR;

    for (size_t i = 0; i < photosExamens.size(); ++i) {
        if (i > 0) oss << ";";
        oss << photosExamens[i].toChamp();
    }
    oss << SEPARATEUR
        << (estPaye ? "1" : "0") << SEPARATEUR
        << (datePaiement.empty() ? "N/A" : datePaiement) << SEPARATEUR
        << montantSalaire << SEPARATEUR
        << (statutCompte.empty() ? "Actif" : statutCompte);

    return oss.str();
}

vector<Professeur> chargerProfesseurs() {
    vector<Professeur> profs;
    ifstream f(FICHIER_PROFS);
    if (!f.is_open()) return profs;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        vector<string> champs = decouperLigne(ligne);
        if (champs.size() < 6) continue;

        Professeur p;
        p.setId(champs[0]);
        p.setNom(champs[1]);
        p.setPrenom(champs[2]);
        p.setEmail(champs[3]);
        p.setTelephone(champs[4]);
        p.setMotDePasseHash(champs[5]);
        if (champs.size() > 6) p.setFaculte(champs[6]);

        if (champs.size() > 7 && !champs[7].empty()) {
            vector<string> mats = decouperLigne(champs[7], ',');
            p.setMatieres(mats);
        }
        if (champs.size() > 8) p.setValide(champs[8] == "1");

        if (champs.size() > 9 && !champs[9].empty()) {
            vector<string> noteList = decouperLigne(champs[9], ';');
            vector<Note> notes;
            for (const auto &nStr : noteList) {
                vector<string> parts = decouperLigne(nStr, '~');
                if (parts.size() >= 3) {
                    try {
                        double val = stod(parts[2]);
                        string st = (parts.size() >= 4) ? parts[3] : "Reçue";
                        notes.push_back(Note(parts[0], parts[1], val, st));
                    } catch (...) {}
                }
            }
            p.setNotes(notes);
        }

        if (champs.size() > 10 && !champs[10].empty()) {
            vector<string> photoList = decouperLigne(champs[10], ';');
            vector<ExamenPhoto> photos;
            for (const auto &phStr : photoList) {
                vector<string> parts = decouperLigne(phStr, '~');
                if (parts.size() >= 3) {
                    ExamenPhoto ep;
                    ep.matiere = parts[0];
                    ep.cheminFichier = parts[1];
                    ep.dateAjout = parts[2];
                    photos.push_back(ep);
                }
            }
            p.setPhotosExamens(photos);
        }

        if (champs.size() > 11) p.setEstPaye(champs[11] == "1");
        if (champs.size() > 12) p.setDatePaiement(champs[12]);
        if (champs.size() > 13) {
            try { p.setMontantSalaire(stod(champs[13])); } catch (...) {}
        }
        if (champs.size() > 14) p.setStatutCompte(champs[14]);

        profs.push_back(p);
    }
    return profs;
}

void sauvegarderProfesseurs(const vector<Professeur> &profs) {
    ofstream f(FICHIER_PROFS, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &p : profs) {
        f << p.toLigne() << "\n";
    }
}

void enregistrerProfesseur(vector<Professeur> &profs, const Professeur &prof) {
    bool trouve = false;
    for (auto &p : profs) {
        if (p.getId() == prof.getId()) {
            p = prof;
            trouve = true;
            break;
        }
    }
    if (!trouve) profs.push_back(prof);
    sauvegarderProfesseurs(profs);
}

// ============================================================
//  Administrateur
// ============================================================
Administrateur::Administrateur()
    : estPaye(false), montantSalaire(0.0), statutCompte("Actif") {}

Administrateur::Administrateur(string n, string p, string t, string e, string pos, string fac,
                               string np, string pp, string tp, string mdp)
    : nom(n), prenom(p), telephone(t), email(e), poste(pos), faculte(fac),
      nomProche(np), prenomProche(pp), telProche(tp),
      motDePasseHash(hacherMotDePasse(mdp)), estPaye(false), montantSalaire(0.0), statutCompte("Actif") {
    string aid = "ADM-";
    aid += (nom.empty() ? 'X' : (char)toupper(nom[0]));
    aid += (prenom.empty() ? 'X' : (char)toupper(prenom[0]));
    aid += "-";
    int r = 1000 + (rand() % 9000);
    aid += to_string(r);
    id = aid;
}

bool Administrateur::verifierMotDePasse(const string &pwd) const {
    return hacherMotDePasse(pwd) == motDePasseHash;
}

void Administrateur::modifierMotDePasse(const string &pwd) {
    motDePasseHash = hacherMotDePasse(pwd);
}

string Administrateur::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << nettoyerChamp(nom) << SEPARATEUR
        << nettoyerChamp(prenom) << SEPARATEUR
        << nettoyerChamp(telephone) << SEPARATEUR
        << nettoyerChamp(email) << SEPARATEUR
        << nettoyerChamp(poste) << SEPARATEUR
        << nettoyerChamp(faculte) << SEPARATEUR
        << nettoyerChamp(nomProche) << SEPARATEUR
        << nettoyerChamp(prenomProche) << SEPARATEUR
        << nettoyerChamp(telProche) << SEPARATEUR
        << motDePasseHash << SEPARATEUR
        << (estPaye ? "1" : "0") << SEPARATEUR
        << (datePaiement.empty() ? "N/A" : datePaiement) << SEPARATEUR
        << montantSalaire << SEPARATEUR
        << (statutCompte.empty() ? "Actif" : statutCompte);
    return oss.str();
}

vector<Administrateur> chargerAdministrateurs() {
    vector<Administrateur> admins;
    ifstream f(FICHIER_ADMINS);
    if (!f.is_open()) return admins;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        vector<string> c = decouperLigne(ligne);
        if (c.size() < 6) continue;

        Administrateur a;
        a.setId(c[0]);
        a.setNom(c[1]);
        a.setPrenom(c[2]);
        a.setTelephone(c[3]);
        a.setEmail(c[4]);
        if (c.size() > 5) a.setPoste(c[5]);
        if (c.size() > 6) a.setFaculte(c[6]);
        if (c.size() > 7) a.setNomProche(c[7]);
        if (c.size() > 8) a.setPrenomProche(c[8]);
        if (c.size() > 9) a.setTelProche(c[9]);
        if (c.size() > 10) a.setMotDePasseHash(c[10]);
        if (c.size() > 11) a.setEstPaye(c[11] == "1");
        if (c.size() > 12) a.setDatePaiement(c[12]);
        if (c.size() > 13) {
            try { a.setMontantSalaire(stod(c[13])); } catch (...) {}
        }
        if (c.size() > 14) a.setStatutCompte(c[14]);

        admins.push_back(a);
    }
    return admins;
}

void sauvegarderAdministrateurs(const vector<Administrateur> &admins) {
    ofstream f(FICHIER_ADMINS, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &a : admins) {
        f << a.toLigne() << "\n";
    }
}

void enregistrerAdministrateur(vector<Administrateur> &admins, const Administrateur &admin) {
    bool trouve = false;
    for (auto &a : admins) {
        if (a.getId() == admin.getId()) {
            a = admin;
            trouve = true;
            break;
        }
    }
    if (!trouve) admins.push_back(admin);
    sauvegarderAdministrateurs(admins);
}

// ============================================================
//  Versements
// ============================================================
VersementEtudiant::VersementEtudiant(const string &id, const string &n, double mont,
                                     const string &d, const string &st, const string &meth)
    : idEtudiant(id), nom(n), montant(mont), date(d), statut(st), methode(meth) {}

string VersementEtudiant::toLigne() const {
    ostringstream oss;
    oss << idEtudiant << SEPARATEUR
        << nettoyerChamp(nom) << SEPARATEUR
        << montant << SEPARATEUR
        << date << SEPARATEUR
        << statut << SEPARATEUR
        << methode;
    return oss.str();
}

VersementEtudiant VersementEtudiant::fromLigne(const string &ligne) {
    vector<string> c = decouperLigne(ligne);
    if (c.size() < 4) return VersementEtudiant();
    double m = 0.0;
    try { m = stod(c[2]); } catch (...) {}
    string st = (c.size() >= 5) ? c[4] : "En attente";
    string meth = (c.size() >= 6) ? c[5] : "MonCash";
    return VersementEtudiant(c[0], c[1], m, c[3], st, meth);
}

vector<VersementEtudiant> chargerVersements() {
    vector<VersementEtudiant> list;
    ifstream f(FICHIER_VERSEMENTS);
    if (!f.is_open()) return list;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        VersementEtudiant v = VersementEtudiant::fromLigne(ligne);
        if (!v.idEtudiant.empty()) list.push_back(v);
    }
    return list;
}

void sauvegarderVersements(const vector<VersementEtudiant> &versements) {
    ofstream f(FICHIER_VERSEMENTS, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &v : versements) {
        f << v.toLigne() << "\n";
    }
}

void ajouterVersement(const VersementEtudiant &versement) {
    vector<VersementEtudiant> v = chargerVersements();
    v.push_back(versement);
    sauvegarderVersements(v);
}

bool validerVersementEtudiant(const string &idEtudiant, const string &dateVersement, const string &nouveauStatut) {
    vector<VersementEtudiant> list = chargerVersements();
    bool modifie = false;
    for (auto &v : list) {
        if (v.idEtudiant == idEtudiant && v.date == dateVersement) {
            v.statut = nouveauStatut;
            modifie = true;
            break;
        }
    }
    if (modifie) sauvegarderVersements(list);
    return modifie;
}

// ============================================================
//  Courriers
// ============================================================
Courrier::Courrier(const string &id, const string &type, const string &exp,
                   const string &dest, const string &titre, const string &contenu,
                   const string &date, const string &statut)
    : id(id), type(type), expediteur(exp), destinataire(dest),
      titre(titre), contenu(contenu), date(date), statut(statut) {}

string Courrier::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << type << SEPARATEUR
        << expediteur << SEPARATEUR
        << destinataire << SEPARATEUR
        << titre << SEPARATEUR
        << contenu << SEPARATEUR
        << date << SEPARATEUR
        << statut;
    return oss.str();
}

Courrier Courrier::fromLigne(const string &ligne) {
    vector<string> c = decouperLigne(ligne);
    if (c.size() < 7) return Courrier();
    string st = (c.size() >= 8) ? c[7] : "Envoyé";
    return Courrier(c[0], c[1], c[2], c[3], c[4], c[5], c[6], st);
}

vector<Courrier> chargerCourriers() {
    vector<Courrier> list;
    ifstream f(FICHIER_COURRIERS);
    if (!f.is_open()) return list;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        Courrier cr = Courrier::fromLigne(ligne);
        if (!cr.id.empty()) list.push_back(cr);
    }
    return list;
}

void sauvegarderCourriers(const vector<Courrier> &courriers) {
    ofstream f(FICHIER_COURRIERS, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &cr : courriers) {
        f << cr.toLigne() << "\n";
    }
}

void ajouterCourrier(const Courrier &courrier) {
    vector<Courrier> list = chargerCourriers();
    list.push_back(courrier);
    sauvegarderCourriers(list);
}

void marquerCourrierLu(const string &idCourrier) {
    vector<Courrier> list = chargerCourriers();
    for (auto &cr : list) {
        if (cr.id == idCourrier) {
            cr.statut = "Lu";
            break;
        }
    }
    sauvegarderCourriers(list);
}

// ============================================================
//  Annonces
// ============================================================
Annonce::Annonce(const string &id, const string &titre, const string &contenu,
                 const string &dateEnvoi, const vector<string> &destinataires,
                 const vector<string> &vuPar)
    : id(id), titre(titre), contenu(contenu), dateEnvoi(dateEnvoi),
      destinataires(destinataires), vuPar(vuPar) {}

bool Annonce::estConcerne(const string &roleCompte) const {
    for (const auto &d : destinataires) {
        if (d == "Tous" || d == roleCompte) return true;
    }
    return false;
}

bool Annonce::estVuPar(const string &idCompte) const {
    return find(vuPar.begin(), vuPar.end(), idCompte) != vuPar.end();
}

void Annonce::marquerVu(const string &idCompte) {
    if (!estVuPar(idCompte)) {
        vuPar.push_back(idCompte);
    }
}

string Annonce::destinatairesChaine() const {
    ostringstream oss;
    for (size_t i = 0; i < destinataires.size(); ++i) {
        if (i > 0) oss << ",";
        oss << destinataires[i];
    }
    return oss.str();
}

string Annonce::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << titre << SEPARATEUR
        << contenu << SEPARATEUR
        << dateEnvoi << SEPARATEUR
        << destinatairesChaine() << SEPARATEUR;

    for (size_t i = 0; i < vuPar.size(); ++i) {
        if (i > 0) oss << ",";
        oss << vuPar[i];
    }
    return oss.str();
}

Annonce Annonce::fromLigne(const string &ligne) {
    vector<string> c = decouperLigne(ligne);
    if (c.size() < 5) return Annonce();
    vector<string> dests = decouperLigne(c[4], ',');
    vector<string> vus;
    if (c.size() >= 6 && !c[5].empty()) {
        vus = decouperLigne(c[5], ',');
    }
    return Annonce(c[0], c[1], c[2], c[3], dests, vus);
}

vector<Annonce> chargerAnnonces() {
    vector<Annonce> list;
    ifstream f(FICHIER_ANNONCES);
    if (!f.is_open()) return list;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        Annonce a = Annonce::fromLigne(ligne);
        if (!a.id.empty()) list.push_back(a);
    }
    return list;
}

void sauvegarderAnnonces(const vector<Annonce> &annonces) {
    ofstream f(FICHIER_ANNONCES, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &a : annonces) {
        f << a.toLigne() << "\n";
    }
}

void ajouterAnnonce(const Annonce &annonce) {
    vector<Annonce> list = chargerAnnonces();
    list.push_back(annonce);
    sauvegarderAnnonces(list);
}

bool supprimerAnnonce(const string &idAnnonce) {
    vector<Annonce> list = chargerAnnonces();
    auto it = remove_if(list.begin(), list.end(), [&idAnnonce](const Annonce &a) {
        return a.id == idAnnonce;
    });
    if (it != list.end()) {
        list.erase(it, list.end());
        sauvegarderAnnonces(list);
        return true;
    }
    return false;
}

// ============================================================
//  Calendrier Scolaire
// ============================================================
EvenementScolaire::EvenementScolaire(const string &id, const string &titre, const string &date,
                                     const string &desc, const string &type)
    : id(id), titre(titre), date(date), description(desc), type(type) {}

string EvenementScolaire::toLigne() const {
    ostringstream oss;
    oss << id << SEPARATEUR
        << titre << SEPARATEUR
        << date << SEPARATEUR
        << description << SEPARATEUR
        << type;
    return oss.str();
}

EvenementScolaire EvenementScolaire::fromLigne(const string &ligne) {
    vector<string> c = decouperLigne(ligne);
    if (c.size() < 5) return EvenementScolaire();
    return EvenementScolaire(c[0], c[1], c[2], c[3], c[4]);
}

vector<EvenementScolaire> chargerEvenementsScolaires() {
    vector<EvenementScolaire> list;
    ifstream f(FICHIER_EVENTS);
    if (!f.is_open()) return list;

    string ligne;
    while (getline(f, ligne)) {
        if (ligne.empty()) continue;
        EvenementScolaire e = EvenementScolaire::fromLigne(ligne);
        if (!e.id.empty()) list.push_back(e);
    }
    return list;
}

void sauvegarderEvenementsScolaires(const vector<EvenementScolaire> &events) {
    ofstream f(FICHIER_EVENTS, ios::trunc);
    if (!f.is_open()) return;
    for (const auto &e : events) {
        f << e.toLigne() << "\n";
    }
}

void ajouterEvenementScolaire(const EvenementScolaire &event) {
    vector<EvenementScolaire> list = chargerEvenementsScolaires();
    list.push_back(event);
    sauvegarderEvenementsScolaires(list);
}

bool supprimerEvenementScolaire(const string &idEvent) {
    vector<EvenementScolaire> list = chargerEvenementsScolaires();
    auto it = remove_if(list.begin(), list.end(), [&idEvent](const EvenementScolaire &e) {
        return e.id == idEvent;
    });
    if (it != list.end()) {
        list.erase(it, list.end());
        sauvegarderEvenementsScolaires(list);
        return true;
    }
    return false;
}
