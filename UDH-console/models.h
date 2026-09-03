#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// ============================================================
//  Programme d'études & Matières
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
//  Utilitaires de hachage et chaînes
// ============================================================
std::string genererId(const std::string &nom, const std::string &prenom);
std::string hacherMotDePasse(const std::string &motDePasse);
std::string nettoyerChamp(std::string champ);
std::string obtenirDateHeureActuelle();

// ============================================================
//  Inscription & Compte Étudiant
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
    std::string statutInscription; // "En attente", "Approuvé", "Rejeté"
    std::string dateSoumission;
    std::string motifRejet;
    std::string documentsSpec;

public:
    CompteEtudiant(std::string nom, std::string prenom, std::string email, std::string telephone, std::string motDePasse);
    CompteEtudiant();

    void setId(const std::string &v) { id = v; }
    void setNom(const std::string &v) { nom = v; }
    void setPrenom(const std::string &v) { prenom = v; }
    void setEmail(const std::string &v) { email = v; }
    void setTelephone(const std::string &v) { telephone = v; }
    void setMotDePasseHash(const std::string &v) { motDePasseHash = v; }
    void setActif(bool v) { actif = v; }
    void setInscriptionFaite(bool v) { inscriptionFaite = v; }
    void setEnPause(bool v) { enPause = v; }
    void setInscription(const Inscription &insc) { inscription = insc; }
    void setStatutInscription(const std::string &v) { statutInscription = v; }
    void setDateSoumission(const std::string &v) { dateSoumission = v; }
    void setMotifRejet(const std::string &v) { motifRejet = v; }
    void setDocumentsSpec(const std::string &v) { documentsSpec = v; }

    std::string getId() const { return id; }
    std::string getNom() const { return nom; }
    std::string getPrenom() const { return prenom; }
    std::string getEmail() const { return email; }
    std::string getTelephone() const { return telephone; }
    bool estActif() const { return actif; }
    bool estInscrit() const { return inscriptionFaite; }
    bool estEnPause() const { return enPause; }
    std::string getFaculte() const { return inscription.faculte; }
    const Inscription& getInscription() const { return inscription; }
    Inscription& getInscriptionRef() { return inscription; }
    std::string getStatutInscription() const { return statutInscription.empty() ? "En attente" : statutInscription; }
    std::string getDateSoumission() const { return dateSoumission; }
    std::string getMotifRejet() const { return motifRejet; }
    std::string getDocumentsSpec() const { return documentsSpec; }

    bool verifierMotDePasse(const std::string &motDePasse) const;
    void modifierMotDePasse(const std::string &nouveauMotDePasse);
    std::string toLigne() const;
};

void sauvegarderComptes(const std::vector<CompteEtudiant> &comptes);
std::vector<CompteEtudiant> chargerComptes();
void enregistrerCompte(std::vector<CompteEtudiant> &comptes, const CompteEtudiant &compte);

// ============================================================
//  Note & Professeur
// ============================================================
struct Note {
    std::string idEtudiant;
    std::string matiere;
    double valeur;
    std::string statut; // "Reçue", "Validée", "Rejetée"

    Note() : valeur(0.0), statut("Reçue") {}
    Note(const std::string &id, const std::string &m, double val, const std::string &st = "Reçue")
        : idEtudiant(id), matiere(m), valeur(val), statut(st) {}

    std::string toChamp() const {
        std::ostringstream oss;
        oss << idEtudiant << "~" << matiere << "~" << valeur << "~" << (statut.empty() ? "Reçue" : statut);
        return oss.str();
    }
};

struct ExamenPhoto {
    std::string matiere;
    std::string cheminFichier;
    std::string dateAjout;

    std::string toChamp() const {
        std::ostringstream oss;
        oss << matiere << "~" << cheminFichier << "~" << dateAjout;
        return oss.str();
    }
};

class Professeur {
private:
    std::string id;
    std::string nom;
    std::string prenom;
    std::string email;
    std::string telephone;
    std::string motDePasseHash;
    std::string faculte;
    std::vector<std::string> matieres;
    bool valide;
    std::vector<Note> notes;
    std::vector<ExamenPhoto> photosExamens;
    bool estPaye;
    std::string datePaiement;
    double montantSalaire;
    std::string statutCompte;

public:
    Professeur(std::string nom, std::string prenom, std::string email, std::string telephone,
               std::string motDePasse, std::string faculte, std::vector<std::string> matieres);
    Professeur();

    void setId(const std::string &v) { id = v; }
    void setNom(const std::string &v) { nom = v; }
    void setPrenom(const std::string &v) { prenom = v; }
    void setEmail(const std::string &v) { email = v; }
    void setTelephone(const std::string &v) { telephone = v; }
    void setMotDePasseHash(const std::string &v) { motDePasseHash = v; }
    void setFaculte(const std::string &v) { faculte = v; }
    void setMatieres(const std::vector<std::string> &v) { matieres = v; }
    void setValide(bool v) { valide = v; }
    void setNotes(const std::vector<Note> &v) { notes = v; }
    void setPhotosExamens(const std::vector<ExamenPhoto> &v) { photosExamens = v; }
    void setEstPaye(bool v) { estPaye = v; }
    void setDatePaiement(const std::string &v) { datePaiement = v; }
    void setMontantSalaire(double v) { montantSalaire = v; }
    void setStatutCompte(const std::string &v) { statutCompte = v; }

    std::string getId() const { return id; }
    std::string getNom() const { return nom; }
    std::string getPrenom() const { return prenom; }
    std::string getEmail() const { return email; }
    std::string getTelephone() const { return telephone; }
    std::string getFaculte() const { return faculte; }
    const std::vector<std::string>& getMatieres() const { return matieres; }
    bool estValide() const { return valide; }
    const std::vector<Note>& getNotes() const { return notes; }
    std::vector<Note>& getNotesRef() { return notes; }
    const std::vector<ExamenPhoto>& getPhotosExamens() const { return photosExamens; }
    bool getEstPaye() const { return estPaye; }
    std::string getDatePaiement() const { return datePaiement; }
    double getMontantSalaire() const { return montantSalaire; }
    std::string getStatutCompte() const { return statutCompte.empty() ? "Actif" : statutCompte; }

    bool verifierMotDePasse(const std::string &motDePasse) const;
    void modifierMotDePasse(const std::string &nouveauMotDePasse);
    void attribuerNote(const std::string &idEtudiant, const std::string &matiere, double valeur, const std::string &statut = "Reçue");
    void ajouterPhotoExamen(const std::string &matiere, const std::string &cheminFichier, const std::string &dateAjout);
    std::string toLigne() const;
};

void sauvegarderProfesseurs(const std::vector<Professeur> &profs);
std::vector<Professeur> chargerProfesseurs();
void enregistrerProfesseur(std::vector<Professeur> &profs, const Professeur &prof);

// ============================================================
//  Administrateur
// ============================================================
class Administrateur {
private:
    std::string id;
    std::string nom;
    std::string prenom;
    std::string telephone;
    std::string email;
    std::string poste;
    std::string faculte;
    std::string nomProche;
    std::string prenomProche;
    std::string telProche;
    std::string motDePasseHash;
    bool estPaye;
    std::string datePaiement;
    double montantSalaire;
    std::string statutCompte;

public:
    Administrateur(std::string nom, std::string prenom, std::string telephone, std::string email,
                   std::string poste, std::string faculte, std::string nomProche, std::string prenomProche,
                   std::string telProche, std::string motDePasse);
    Administrateur();

    void setId(const std::string &v) { id = v; }
    void setNom(const std::string &v) { nom = v; }
    void setPrenom(const std::string &v) { prenom = v; }
    void setTelephone(const std::string &v) { telephone = v; }
    void setEmail(const std::string &v) { email = v; }
    void setPoste(const std::string &v) { poste = v; }
    void setFaculte(const std::string &v) { faculte = v; }
    void setNomProche(const std::string &v) { nomProche = v; }
    void setPrenomProche(const std::string &v) { prenomProche = v; }
    void setTelProche(const std::string &v) { telProche = v; }
    void setMotDePasseHash(const std::string &v) { motDePasseHash = v; }
    void setEstPaye(bool v) { estPaye = v; }
    void setDatePaiement(const std::string &v) { datePaiement = v; }
    void setMontantSalaire(double v) { montantSalaire = v; }
    void setStatutCompte(const std::string &v) { statutCompte = v; }

    std::string getId() const { return id; }
    std::string getNom() const { return nom; }
    std::string getPrenom() const { return prenom; }
    std::string getTelephone() const { return telephone; }
    std::string getEmail() const { return email; }
    std::string getPoste() const { return poste; }
    std::string getFaculte() const { return faculte; }
    std::string getNomProche() const { return nomProche; }
    std::string getPrenomProche() const { return prenomProche; }
    std::string getTelProche() const { return telProche; }
    bool getEstPaye() const { return estPaye; }
    std::string getDatePaiement() const { return datePaiement; }
    double getMontantSalaire() const { return montantSalaire; }
    std::string getStatutCompte() const { return statutCompte.empty() ? "Actif" : statutCompte; }

    bool verifierMotDePasse(const std::string &pwd) const;
    void modifierMotDePasse(const std::string &pwd);
    std::string toLigne() const;
};

void sauvegarderAdministrateurs(const std::vector<Administrateur> &admins);
std::vector<Administrateur> chargerAdministrateurs();
void enregistrerAdministrateur(std::vector<Administrateur> &admins, const Administrateur &admin);

// ============================================================
//  Versements & Paiements
// ============================================================
struct VersementEtudiant {
    std::string idEtudiant;
    std::string nom;
    double montant;
    std::string date;
    std::string statut; // "En attente", "Validé", "Rejeté"
    std::string methode; // "MonCash", "Carte Bancaire", "Natcash", "Virement Bancaire"

    VersementEtudiant() : montant(0.0), statut("En attente") {}
    VersementEtudiant(const std::string &id, const std::string &nom, double montant,
                      const std::string &date, const std::string &statut = "En attente",
                      const std::string &methode = "MonCash");

    std::string toLigne() const;
    static VersementEtudiant fromLigne(const std::string &ligne);
};

std::vector<VersementEtudiant> chargerVersements();
void sauvegarderVersements(const std::vector<VersementEtudiant> &versements);
void ajouterVersement(const VersementEtudiant &versement);
bool validerVersementEtudiant(const std::string &idEtudiant, const std::string &dateVersement, const std::string &nouveauStatut);

// ============================================================
//  Courriers
// ============================================================
struct Courrier {
    std::string id;
    std::string type; // "Compte-rendu", "Liste des paiements", "Convocation", "Officiel"
    std::string expediteur;
    std::string destinataire;
    std::string titre;
    std::string contenu;
    std::string date;
    std::string statut; // "Envoyé", "Lu"

    Courrier() = default;
    Courrier(const std::string &id, const std::string &type, const std::string &exp,
             const std::string &dest, const std::string &titre, const std::string &contenu,
             const std::string &date, const std::string &statut = "Envoyé");

    std::string toLigne() const;
    static Courrier fromLigne(const std::string &ligne);
};

std::vector<Courrier> chargerCourriers();
void sauvegarderCourriers(const std::vector<Courrier> &courriers);
void ajouterCourrier(const Courrier &courrier);
void marquerCourrierLu(const std::string &idCourrier);

// ============================================================
//  Annonces
// ============================================================
struct Annonce {
    std::string id;
    std::string titre;
    std::string contenu;
    std::string dateEnvoi;
    std::vector<std::string> destinataires; // "Etudiants", "Professeurs", "Administrateurs", "Tous"
    std::vector<std::string> vuPar;

    Annonce() = default;
    Annonce(const std::string &id, const std::string &titre, const std::string &contenu,
            const std::string &dateEnvoi, const std::vector<std::string> &destinataires,
            const std::vector<std::string> &vuPar = {});

    bool estConcerne(const std::string &roleCompte) const;
    bool estVuPar(const std::string &idCompte) const;
    void marquerVu(const std::string &idCompte);
    std::string toLigne() const;
    static Annonce fromLigne(const std::string &ligne);
    std::string destinatairesChaine() const;
};

std::vector<Annonce> chargerAnnonces();
void sauvegarderAnnonces(const std::vector<Annonce> &annonces);
void ajouterAnnonce(const Annonce &annonce);
bool supprimerAnnonce(const std::string &idAnnonce);

// ============================================================
//  Calendrier Scolaire
// ============================================================
struct EvenementScolaire {
    std::string id;
    std::string titre;
    std::string date;
    std::string description;
    std::string type; // "Examen", "Rentrée", "Vacances", "Autre"

    EvenementScolaire() = default;
    EvenementScolaire(const std::string &id, const std::string &titre, const std::string &date,
                      const std::string &description, const std::string &type);

    std::string toLigne() const;
    static EvenementScolaire fromLigne(const std::string &ligne);
};

std::vector<EvenementScolaire> chargerEvenementsScolaires();
void sauvegarderEvenementsScolaires(const std::vector<EvenementScolaire> &events);
void ajouterEvenementScolaire(const EvenementScolaire &event);
bool supprimerEvenementScolaire(const std::string &idEvent);

#endif // MODELS_H
