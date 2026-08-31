#ifndef TRESORERIE_H
#define TRESORERIE_H

#include <string>
#include <vector>
#include "professeur.h"
#include "administrateur.h"
#include "paiementversement.h"

// ============================================================
//  Structure TransactionTresorerie
//  Enregistrement horodaté d'un flux financier (entrée/sortie)
// ============================================================
struct TransactionTresorerie {
    std::string id;                  // Ex: TRX-20260831-1234
    std::string type;                // "Recette" ou "Dépense"
    std::string categorie;           // "Frais de scolarité", "Salaire Professeur", "Salaire Administrateur", etc.
    std::string beneficiaireOuPayeur;// Nom & ID de l'étudiant ou de l'employé
    double montant;                  // Montant en HTG
    std::string date;                // "dd/MM/yyyy HH:mm"
    double soldeApres;               // Solde disponible après l'opération
    std::string methode;             // "MonCash", "Carte Bancaire", "Virement", "Caisse", etc.
    std::string description;         // Motif ou détails

    TransactionTresorerie();
    TransactionTresorerie(const std::string &id, const std::string &type,
                          const std::string &categorie, const std::string &personne,
                          double montant, const std::string &date, double soldeApres,
                          const std::string &methode = "", const std::string &desc = "");

    std::string toLigne() const;
    static TransactionTresorerie fromLigne(const std::string &ligne);
};

// ============================================================
//  Fonctions de calcul du Solde & Trésorerie
// ============================================================

// Calcule la somme de tous les versements étudiants validés
double calculerRecettesEtudiantesTotales();

// Calcule la somme de tous les salaires déjà versés au personnel
double calculerSalairesVersesTotaux();
double calculerSalairesVersesTotaux(const std::vector<Professeur> &profs, const std::vector<Administrateur> &admins);

// Calcule le solde disponible = Recettes étudiantes validées - Salaires versés
double calculerSoldeTresorerieDisponible();
double calculerSoldeTresorerieDisponible(const std::vector<Professeur> &profs, const std::vector<Administrateur> &admins);

// Vérifie si le solde disponible couvre le montant demandé
bool verifierSoldeSuffisant(double montantDemande, double &soldeActuel);

// ============================================================
//  Journal de Caisse & Traçabilité (Persistance)
// ============================================================
std::vector<TransactionTresorerie> chargerJournalTresorerie();
void sauvegarderJournalTresorerie(const std::vector<TransactionTresorerie> &journal);
void enregistrerOperationTresorerie(const std::string &type,
                                    const std::string &categorie,
                                    const std::string &beneficiaireOuPayeur,
                                    double montant,
                                    double soldeApres,
                                    const std::string &methode = "Caisse / Système",
                                    const std::string &description = "");

// Initialise / synchronise l'historique de base si le journal est vide
void synchroniserJournalTresorerieSiVide();

#endif // TRESORERIE_H
