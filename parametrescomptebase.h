#ifndef PARAMETRESCOMPTEBASE_H
#define PARAMETRESCOMPTEBASE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "etudiant.h"
#include "professeur.h"
#include "administrateur.h"

enum class TypeCompte {
    Etudiant,
    Professeur,
    Recteur,
    President,
    Secretaire,
    Coordonnateur
};

class ParametresCompteBase : public QWidget {
    Q_OBJECT

public:
    // Constructeur pour Étudiant
    explicit ParametresCompteBase(CompteEtudiant *compte, std::vector<CompteEtudiant> &comptesRef, QWidget *parent = nullptr);

    // Constructeur pour Professeur
    explicit ParametresCompteBase(Professeur *prof, std::vector<Professeur> &profsRef, QWidget *parent = nullptr);

    // Constructeur pour Administrateur (Recteur, Président, Secrétaire, Coordonnateur)
    explicit ParametresCompteBase(Administrateur *admin, std::vector<Administrateur> &adminsRef, QWidget *parent = nullptr);

    ~ParametresCompteBase() override;

    void refreshData();

signals:
    void logoutRequested();
    void profilUpdated();

private slots:
    void onModifierNom();
    void onModifierPrenom();
    void onModifierTelephone();
    void onModifierEmail();
    void onModifierMotDePasse();
    void onReinitialiserMotDePasse();
    void onModifierProche();
    void onBasculerPauseEtudiant();

private:
    void setupUI();
    QWidget* createProfilCard();
    QWidget* createInfosPersoCard();
    QWidget* createSecuriteCard();
    QWidget* createProcheCard();
    QWidget* createRoleSpecifiqueCard();
    QWidget* createActionsCard();

    void sauvegarderDonnees();
    void propagerNouveauId(const std::string &ancienId, const std::string &nouvelId);

    TypeCompte typeCompte;

    // Pointeurs vers l'entité connectée (un seul est non-nul)
    CompteEtudiant *currentEtudiant;
    std::vector<CompteEtudiant> *comptesList;

    Professeur *currentProf;
    std::vector<Professeur> *profsList;

    Administrateur *currentAdmin;
    std::vector<Administrateur> *adminsList;

    // UI Elements communs
    QLabel *lblId;
    QLabel *lblNomComplet;
    QLabel *lblRoleBadge;
    QLabel *lblStatutBadge;

    QLineEdit *editNom;
    QLineEdit *editPrenom;
    QLineEdit *editTel;
    QLineEdit *editEmail;

    // Contact proche
    QLineEdit *editNomProche;
    QLineEdit *editPrenomProche;
    QLineEdit *editTelProche;

    // Éléments spécifiques
    QLabel *lblFaculte;
    QLabel *lblMatieres;
    QLabel *lblPoste;
    QLabel *lblSalaireStatut;
    QPushButton *btnPauseEtudiant;
};

#endif // PARAMETRESCOMPTEBASE_H
