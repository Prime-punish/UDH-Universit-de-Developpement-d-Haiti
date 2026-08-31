#ifndef PAIEMENTVERSEMENT_H
#define PAIEMENTVERSEMENT_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>
#include "etudiant.h"

// ============================================================
//  Structure VersementEtudiant
// ============================================================
struct VersementEtudiant {
    std::string idEtudiant;
    std::string nom;
    double montant;
    std::string date; // "dd/MM/yyyy HH:mm"
    std::string statut; // "En attente", "Validé", "Rejeté"
    std::string methode; // "MonCash", "Carte Bancaire (Visa/Mastercard)", "Virement bancaire"

    VersementEtudiant() : montant(0.0), statut("En attente") {}
    VersementEtudiant(const std::string &id, const std::string &nom, double montant,
                      const std::string &date, const std::string &statut = "En attente",
                      const std::string &methode = "MonCash");

    std::string toLigne() const;
    static VersementEtudiant fromLigne(const std::string &ligne);
};

// ============================================================
//  Persistance versements.txt
// ============================================================
std::vector<VersementEtudiant> chargerVersements();
void sauvegarderVersements(const std::vector<VersementEtudiant> &versements);
void ajouterVersement(const VersementEtudiant &versement);
bool validerVersementEtudiant(const std::string &idEtudiant, const std::string &dateVersement, const std::string &nouveauStatut);

// ============================================================
//  Widget PaiementVersement
// ============================================================
class PaiementVersement : public QWidget {
    Q_OBJECT

public:
    enum Mode { EtudiantMode, SecretaireMode };

    explicit PaiementVersement(Mode mode, const std::string &idEtudiant = "",
                               const std::string &nomEtudiant = "",
                               std::vector<CompteEtudiant> *comptesPtr = nullptr,
                               QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onConfirmPaymentClicked();
    void onValidatePaymentClicked(const QString &idEtudiant, const QString &dateVersement);
    void onRejectPaymentClicked(const QString &idEtudiant, const QString &dateVersement);

private:
    void setupEtudiantUI();
    void setupSecretaireUI();

    Mode currentMode;
    std::string studentId;
    std::string studentName;
    std::vector<CompteEtudiant> *comptes;

    // Etudiant mode widgets
    QLineEdit *amountEdit;
    QComboBox *methodCombo;
    QLineEdit *refTransactionEdit;
    QTableWidget *studentHistoryTable;
    QLabel *totalPaidLabel;

    // Secretaire mode widgets
    QTableWidget *adminVersementsTable;
    QComboBox *filterStatusCombo;
    QLabel *statPendingLabel;
    QLabel *statValidatedLabel;
    QLabel *statTotalAmountLabel;
};

#endif // PAIEMENTVERSEMENT_H
