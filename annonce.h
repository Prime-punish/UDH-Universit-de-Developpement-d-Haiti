#ifndef ANNONCE_H
#define ANNONCE_H

#include <string>
#include <vector>

// ============================================================
//  Structure Annonce (communication institutionnelle)
// ============================================================
struct Annonce {
    std::string id;
    std::string titre;
    std::string contenu;
    std::string dateEnvoi;
    std::vector<std::string> destinataires; // "Etudiants", "Professeurs", "Administrateurs", "Tous"
    std::vector<std::string> vuPar;         // Liste des ID des comptes ayant lu l'annonce

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

// ============================================================
//  Fonctions de persistance pour annonces.txt
// ============================================================
std::vector<Annonce> chargerAnnonces();
void sauvegarderAnnonces(const std::vector<Annonce> &annonces);
void ajouterAnnonce(const Annonce &annonce);
bool supprimerAnnonce(const std::string &idAnnonce);
void marquerAnnonceLue(const std::string &idAnnonce, const std::string &idCompte);
int compterAnnoncesNonLues(const std::string &roleCompte, const std::string &idCompte);

#endif // ANNONCE_H
