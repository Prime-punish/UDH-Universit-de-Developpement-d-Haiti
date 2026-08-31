#ifndef ADMINISTRATEUR_H
#define ADMINISTRATEUR_H

#include <string>
#include <vector>
#include <sstream>

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
    std::string statutCompte; // "Actif", "En congé", "Rétrogradé", "Licencié", "Révoqué"

public:
    Administrateur(std::string nom, std::string prenom, std::string telephone, std::string email,
                   std::string poste, std::string faculte, std::string nomProche, std::string prenomProche,
                   std::string telProche, std::string motDePasse);
    Administrateur();

    // Setters
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

    // Getters
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

// Persistence functions
void sauvegarderAdministrateurs(const std::vector<Administrateur> &admins);
std::vector<Administrateur> chargerAdministrateurs();
void enregistrerAdministrateur(std::vector<Administrateur> &admins, const Administrateur &admin);
bool estPostePris(const std::vector<Administrateur> &admins, const std::string &poste, const std::string &faculte = "");

#endif // ADMINISTRATEUR_H
