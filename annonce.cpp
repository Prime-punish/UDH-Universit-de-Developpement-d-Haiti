#include "annonce.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

static const std::string FICHIER_ANNONCES = "annonces.txt";
static const char SEPARATEUR = '~';

static std::vector<std::string> decouperAnnonce(const std::string &texte, char sep) {
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

static std::string echapperTexte(std::string texte) {
    std::string res;
    for (char c : texte) {
        if (c == '\n') {
            res += "\\n";
        } else if (c == '\r') {
            // ignorer CR
        } else if (c == SEPARATEUR) {
            res += "-";
        } else {
            res += c;
        }
    }
    return res;
}

static std::string restaurerTexte(const std::string &texte) {
    std::string res;
    for (size_t i = 0; i < texte.size(); ++i) {
        if (texte[i] == '\\' && i + 1 < texte.size() && texte[i + 1] == 'n') {
            res += '\n';
            i++;
        } else {
            res += texte[i];
        }
    }
    return res;
}

Annonce::Annonce(const std::string &id, const std::string &titre, const std::string &contenu,
                 const std::string &dateEnvoi, const std::vector<std::string> &destinataires,
                 const std::vector<std::string> &vuPar)
    : id(id), titre(titre), contenu(contenu), dateEnvoi(dateEnvoi),
      destinataires(destinataires), vuPar(vuPar) {}

bool Annonce::estConcerne(const std::string &roleCompte) const {
    if (destinataires.empty()) return true;
    for (const auto &d : destinataires) {
        if (d == "Tous" || d == "TOUS" || d == roleCompte) {
            return true;
        }
    }
    return false;
}

bool Annonce::estVuPar(const std::string &idCompte) const {
    if (idCompte.empty()) return false;
    for (const auto &v : vuPar) {
        if (v == idCompte) {
            return true;
        }
    }
    return false;
}

void Annonce::marquerVu(const std::string &idCompte) {
    if (idCompte.empty()) return;
    if (!estVuPar(idCompte)) {
        vuPar.push_back(idCompte);
    }
}

std::string Annonce::destinatairesChaine() const {
    if (destinataires.empty()) return "Tous";
    std::string s;
    for (size_t i = 0; i < destinataires.size(); ++i) {
        s += destinataires[i];
        if (i + 1 < destinataires.size()) s += ", ";
    }
    return s;
}

std::string Annonce::toLigne() const {
    std::ostringstream oss;
    oss << id << SEPARATEUR
        << echapperTexte(titre) << SEPARATEUR
        << echapperTexte(contenu) << SEPARATEUR
        << dateEnvoi << SEPARATEUR;

    // Destinataires séparés par ';'
    for (size_t i = 0; i < destinataires.size(); ++i) {
        oss << destinataires[i];
        if (i + 1 < destinataires.size()) oss << ";";
    }
    oss << SEPARATEUR;

    // vuPar séparés par ';'
    for (size_t i = 0; i < vuPar.size(); ++i) {
        oss << vuPar[i];
        if (i + 1 < vuPar.size()) oss << ";";
    }

    return oss.str();
}

Annonce Annonce::fromLigne(const std::string &ligne) {
    auto c = decouperAnnonce(ligne, SEPARATEUR);
    if (c.size() < 4) return Annonce();

    std::string id = c[0];
    std::string titre = restaurerTexte(c[1]);
    std::string contenu = restaurerTexte(c[2]);
    std::string dateEnvoi = c[3];

    std::vector<std::string> dests;
    if (c.size() >= 5 && !c[4].empty()) {
        dests = decouperAnnonce(c[4], ';');
    }

    std::vector<std::string> vus;
    if (c.size() >= 6 && !c[5].empty()) {
        vus = decouperAnnonce(c[5], ';');
    }

    return Annonce(id, titre, contenu, dateEnvoi, dests, vus);
}

std::vector<Annonce> chargerAnnonces() {
    std::vector<Annonce> liste;
    std::ifstream f(FICHIER_ANNONCES);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        Annonce a = Annonce::fromLigne(ligne);
        if (!a.id.empty()) {
            liste.push_back(a);
        }
    }
    return liste;
}

void sauvegarderAnnonces(const std::vector<Annonce> &annonces) {
    std::ofstream f(FICHIER_ANNONCES, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &a : annonces) {
        if (!a.id.empty()) {
            f << a.toLigne() << "\n";
        }
    }
}

void ajouterAnnonce(const Annonce &annonce) {
    auto liste = chargerAnnonces();
    // Insérer en tête ou vérifier l'unicité
    bool existe = false;
    for (auto &a : liste) {
        if (a.id == annonce.id) {
            a = annonce;
            existe = true;
            break;
        }
    }
    if (!existe) {
        liste.insert(liste.begin(), annonce);
    }
    sauvegarderAnnonces(liste);
}

bool supprimerAnnonce(const std::string &idAnnonce) {
    auto liste = chargerAnnonces();
    auto it = std::remove_if(liste.begin(), liste.end(), [&idAnnonce](const Annonce &a) {
        return a.id == idAnnonce;
    });
    if (it != liste.end()) {
        liste.erase(it, liste.end());
        sauvegarderAnnonces(liste);
        return true;
    }
    return false;
}

void marquerAnnonceLue(const std::string &idAnnonce, const std::string &idCompte) {
    if (idAnnonce.empty() || idCompte.empty()) return;
    auto liste = chargerAnnonces();
    bool modifie = false;
    for (auto &a : liste) {
        if (a.id == idAnnonce) {
            if (!a.estVuPar(idCompte)) {
                a.marquerVu(idCompte);
                modifie = true;
            }
            break;
        }
    }
    if (modifie) {
        sauvegarderAnnonces(liste);
    }
}

int compterAnnoncesNonLues(const std::string &roleCompte, const std::string &idCompte) {
    if (idCompte.empty()) return 0;
    auto liste = chargerAnnonces();
    int count = 0;
    for (const auto &a : liste) {
        if (a.estConcerne(roleCompte) && !a.estVuPar(idCompte)) {
            count++;
        }
    }
    return count;
}
