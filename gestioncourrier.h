#ifndef GESTIONCOURRIER_H
#define GESTIONCOURRIER_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <string>
#include <vector>
#include "professeur.h"
#include "administrateur.h"

// ============================================================
//  Structure Courrier
// ============================================================
struct Courrier {
    std::string id;
    std::string type; // "Compte-rendu", "Liste des paiements", "Autre"
    std::string expediteur; // "Secrétaire"
    std::string destinataire; // "Président", "Recteur", "Tous"
    std::string titre;
    std::string contenu;
    std::string date; // "dd/MM/yyyy HH:mm"
    std::string statut; // "Envoyé", "Lu"

    Courrier() = default;
    Courrier(const std::string &id, const std::string &type, const std::string &exp,
             const std::string &dest, const std::string &titre, const std::string &contenu,
             const std::string &date, const std::string &statut = "Envoyé");

    std::string toLigne() const;
    static Courrier fromLigne(const std::string &ligne);
};

// ============================================================
//  Persistance courriers.txt
// ============================================================
std::vector<Courrier> chargerCourriers();
void sauvegarderCourriers(const std::vector<Courrier> &courriers);
void ajouterCourrier(const Courrier &courrier);
void marquerCourrierLu(const std::string &idCourrier);

// ============================================================
//  Widget GestionCourrier (Secrétaire - 3 flux)
// ============================================================
class GestionCourrier : public QWidget {
    Q_OBJECT

public:
    explicit GestionCourrier(std::vector<Professeur> &profsRef, std::vector<Administrateur> &adminsRef, QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onSendCompteRenduClicked();
    void onSendPaiementsListClicked();
    void onViewCourrierClicked(const QString &idCourrier);

private:
    void setupUI();
    QWidget* createCompteRenduTab();
    QWidget* createPaiementsTab();
    QWidget* createOutboxTab();

    std::vector<Professeur> &profs;
    std::vector<Administrateur> &admins;

    QTabWidget *tabWidget;

    // Compte-rendu tab widgets
    QLineEdit *crTitleEdit;
    QDateEdit *crDateEdit;
    QTextEdit *crContentEdit;

    // Paiements tab widgets
    QLabel *paiementsPreviewLabel;
    QTextEdit *paiementsPreviewText;

    // Outbox tab widgets
    QTableWidget *outboxTable;
};

// ============================================================
//  Widget CourriersPresidentView (Conseil d'administration)
// ============================================================
class CourriersPresidentView : public QWidget {
    Q_OBJECT

public:
    explicit CourriersPresidentView(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onViewReportClicked(const QString &idCourrier);

private:
    void setupUI();
    QTableWidget *reportsTable;
};

// ============================================================
//  Widget CourriersRecteurView (Gestion financière reçue)
// ============================================================
class CourriersRecteurView : public QWidget {
    Q_OBJECT

public:
    explicit CourriersRecteurView(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onViewPaiementDocClicked(const QString &idCourrier);

private:
    void setupUI();
    QTableWidget *paiementsTable;
};

#endif // GESTIONCOURRIER_H
