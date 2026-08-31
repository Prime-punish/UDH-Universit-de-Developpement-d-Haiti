#ifndef PROFESSEUR_H
#define PROFESSEUR_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

// ============================================================
//  Structure Note (note attribuée par un prof à un étudiant)
// ============================================================
struct Note {
    std::string idEtudiant;
    std::string matiere;
    double valeur;
    std::string statut; 

    Note() : valeur(0.0), statut("Reçue") {}
    Note(const std::string &id, const std::string &m, double val, const std::string &st = "Reçue")
        : idEtudiant(id), matiere(m), valeur(val), statut(st) {}

    std::string toChamp() const {
        std::ostringstream oss;
        oss << idEtudiant << "~" << matiere << "~" << valeur << "~" << (statut.empty() ? "Reçue" : statut);
        return oss.str();
    }
};

// ============================================================
//  Structure ExamenPhoto (sujet d'examen uploadé par le prof)
// ============================================================
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

// ============================================================
//  Classe Professeur
// ============================================================
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
    bool estPaye;              // 12e champ : statut de salaire (0: non payé, 1: payé)
    std::string datePaiement;  // Date du dernier paiement (ex: 26/08/2026)
    double montantSalaire;     // Montant versé (en HTG)
    std::string statutCompte;  // "Actif", "En congé", "Rétrogradé", "Licencié", "Révoqué"

public:
    Professeur(std::string nom, std::string prenom, std::string email, std::string telephone,
               std::string motDePasse, std::string faculte, std::vector<std::string> matieres);
    Professeur();

    // Setters
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

    // Getters
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
    void modifierTelephone(const std::string &v) { telephone = v; }
    void modifierEmail(const std::string &v) { email = v; }
    void modifierNom(const std::string &nouveauNom);
    void modifierPrenom(const std::string &nouveauPrenom);

    void attribuerNote(const std::string &idEtudiant, const std::string &matiere, double valeur, const std::string &statut = "Reçue");
    void ajouterPhotoExamen(const std::string &matiere, const std::string &cheminFichier, const std::string &dateAjout);

    std::string toLigne() const;
};

// Functions
void sauvegarderProfesseurs(const std::vector<Professeur> &profs);
std::vector<Professeur> chargerProfesseurs();
void enregistrerProfesseur(std::vector<Professeur> &profs, const Professeur &prof);
bool rechercherEtudiant(const std::string &idEtudiant, std::string &nomComplet);

#endif // PROFESSEUR_H
