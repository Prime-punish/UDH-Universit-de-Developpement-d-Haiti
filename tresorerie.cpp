#include "tresorerie.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <QDateTime>
#include <algorithm>

static const std::string FICHIER_JOURNAL_TRESORERIE = "journal_tresorerie.txt";
static const char SEPARATEUR_TRX = '~';

static std::vector<std::string> decouperTrx(const std::string &texte, char sep) {
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
        if (c == '\n') res += "\\n";
        else if (c == '\r') continue;
        else if (c == '~') res += "-";
        else res += c;
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

TransactionTresorerie::TransactionTresorerie()
    : montant(0.0), soldeApres(0.0) {}

TransactionTresorerie::TransactionTresorerie(const std::string &id, const std::string &type,
                                             const std::string &categorie, const std::string &personne,
                                             double montant, const std::string &date, double soldeApres,
                                             const std::string &methode, const std::string &desc)
    : id(id), type(type), categorie(categorie), beneficiaireOuPayeur(personne),
      montant(montant), date(date), soldeApres(soldeApres), methode(methode), description(desc) {}

std::string TransactionTresorerie::toLigne() const {
    std::ostringstream oss;
    oss << id << "~"
        << type << "~"
        << categorie << "~"
        << echapperTexte(beneficiaireOuPayeur) << "~"
        << std::fixed << std::setprecision(2) << montant << "~"
        << date << "~"
        << std::fixed << std::setprecision(2) << soldeApres << "~"
        << echapperTexte(methode) << "~"
        << echapperTexte(description);
    return oss.str();
}

TransactionTresorerie TransactionTresorerie::fromLigne(const std::string &ligne) {
    auto c = decouperTrx(ligne, '~');
    if (c.size() < 7) return TransactionTresorerie();

    std::string id = c[0];
    std::string type = c[1];
    std::string cat = c[2];
    std::string pers = restaurerTexte(c[3]);
    double mont = 0.0;
    try { mont = std::stod(c[4]); } catch (...) { mont = 0.0; }
    std::string dt = c[5];
    double solde = 0.0;
    try { solde = std::stod(c[6]); } catch (...) { solde = 0.0; }
    std::string meth = (c.size() >= 8) ? restaurerTexte(c[7]) : "";
    std::string desc = (c.size() >= 9) ? restaurerTexte(c[8]) : "";

    return TransactionTresorerie(id, type, cat, pers, mont, dt, solde, meth, desc);
}

double calculerRecettesEtudiantesTotales() {
    std::vector<VersementEtudiant> versements = chargerVersements();
    double total = 0.0;
    for (const auto &v : versements) {
        if (v.statut == "Validé" || v.statut == "Confirmé") {
            total += v.montant;
        }
    }
    return total;
}

double calculerSalairesVersesTotaux() {
    std::vector<Professeur> profs = chargerProfesseurs();
    std::vector<Administrateur> admins = chargerAdministrateurs();
    return calculerSalairesVersesTotaux(profs, admins);
}

double calculerSalairesVersesTotaux(const std::vector<Professeur> &profs, const std::vector<Administrateur> &admins) {
    double total = 0.0;
    for (const auto &p : profs) {
        if (p.getEstPaye() && p.getMontantSalaire() > 0) {
            total += p.getMontantSalaire();
        }
    }
    for (const auto &a : admins) {
        if (a.getEstPaye() && a.getMontantSalaire() > 0) {
            total += a.getMontantSalaire();
        }
    }
    return total;
}

double calculerSoldeTresorerieDisponible() {
    double recettes = calculerRecettesEtudiantesTotales();
    double salaires = calculerSalairesVersesTotaux();
    return recettes - salaires;
}

double calculerSoldeTresorerieDisponible(const std::vector<Professeur> &profs, const std::vector<Administrateur> &admins) {
    double recettes = calculerRecettesEtudiantesTotales();
    double salaires = calculerSalairesVersesTotaux(profs, admins);
    return recettes - salaires;
}

bool verifierSoldeSuffisant(double montantDemande, double &soldeActuel) {
    soldeActuel = calculerSoldeTresorerieDisponible();
    return soldeActuel >= montantDemande;
}

std::vector<TransactionTresorerie> chargerJournalTresorerie() {
    std::vector<TransactionTresorerie> liste;
    std::ifstream f(FICHIER_JOURNAL_TRESORERIE);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        TransactionTresorerie t = TransactionTresorerie::fromLigne(ligne);
        if (!t.id.empty()) {
            liste.push_back(t);
        }
    }
    return liste;
}

void sauvegarderJournalTresorerie(const std::vector<TransactionTresorerie> &journal) {
    std::ofstream f(FICHIER_JOURNAL_TRESORERIE, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &t : journal) {
        f << t.toLigne() << "\n";
    }
}

void enregistrerOperationTresorerie(const std::string &type,
                                    const std::string &categorie,
                                    const std::string &beneficiaireOuPayeur,
                                    double montant,
                                    double soldeApres,
                                    const std::string &methode,
                                    const std::string &description) {
    std::vector<TransactionTresorerie> journal = chargerJournalTresorerie();
    std::string nowStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm").toStdString();
    std::string uniqueId = "TRX-" + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmsszzz").toStdString();

    TransactionTresorerie trx(uniqueId, type, categorie, beneficiaireOuPayeur,
                              montant, nowStr, soldeApres, methode, description);
    journal.push_back(trx);
    sauvegarderJournalTresorerie(journal);
}

void synchroniserJournalTresorerieSiVide() {
    std::vector<TransactionTresorerie> journal = chargerJournalTresorerie();
    if (!journal.empty()) return;

    // Si le journal est vierge, réconstituer l'historique initial
    double soldeCourant = 0.0;
    std::vector<VersementEtudiant> versements = chargerVersements();
    for (const auto &v : versements) {
        if (v.statut == "Validé" || v.statut == "Confirmé") {
            soldeCourant += v.montant;
            std::string trxId = "TRX-INIT-REC-" + std::to_string(rand() % 100000);
            journal.emplace_back(trxId, "Recette", "Frais de scolarité",
                                 v.nom + " (" + v.idEtudiant + ")",
                                 v.montant, v.date, soldeCourant, v.methode,
                                 "Versement étudiant validé");
        }
    }

    std::vector<Professeur> profs = chargerProfesseurs();
    for (const auto &p : profs) {
        if (p.getEstPaye() && p.getMontantSalaire() > 0) {
            soldeCourant -= p.getMontantSalaire();
            std::string trxId = "TRX-INIT-DEP-" + std::to_string(rand() % 100000);
            std::string dateP = p.getDatePaiement().empty() ? "01/08/2026 10:00" : p.getDatePaiement();
            journal.emplace_back(trxId, "Dépense", "Salaire Professeur",
                                 p.getPrenom() + " " + p.getNom() + " (" + p.getId() + ")",
                                 p.getMontantSalaire(), dateP, soldeCourant, "Virement Bancaire",
                                 "Salaire mensuel du professeur");
        }
    }

    std::vector<Administrateur> admins = chargerAdministrateurs();
    for (const auto &a : admins) {
        if (a.getEstPaye() && a.getMontantSalaire() > 0) {
            soldeCourant -= a.getMontantSalaire();
            std::string trxId = "TRX-INIT-DEP-" + std::to_string(rand() % 100000);
            std::string dateP = a.getDatePaiement().empty() ? "01/08/2026 10:00" : a.getDatePaiement();
            journal.emplace_back(trxId, "Dépense", "Salaire Administrateur",
                                 a.getPrenom() + " " + a.getNom() + " (" + a.getPoste() + ")",
                                 a.getMontantSalaire(), dateP, soldeCourant, "Virement Bancaire",
                                 "Salaire mensuel de l'administrateur");
        }
    }

    sauvegarderJournalTresorerie(journal);
}
