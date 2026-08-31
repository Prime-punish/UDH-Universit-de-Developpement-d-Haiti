#include "parametrescomptebase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include "paiementversement.h"

ParametresCompteBase::ParametresCompteBase(CompteEtudiant *compte, std::vector<CompteEtudiant> &comptesRef, QWidget *parent)
    : QWidget(parent), typeCompte(TypeCompte::Etudiant),
      currentEtudiant(compte), comptesList(&comptesRef),
      currentProf(nullptr), profsList(nullptr),
      currentAdmin(nullptr), adminsList(nullptr),
      lblId(nullptr), lblNomComplet(nullptr), lblRoleBadge(nullptr), lblStatutBadge(nullptr),
      editNom(nullptr), editPrenom(nullptr), editTel(nullptr), editEmail(nullptr),
      editNomProche(nullptr), editPrenomProche(nullptr), editTelProche(nullptr),
      lblFaculte(nullptr), lblMatieres(nullptr), lblPoste(nullptr), lblSalaireStatut(nullptr),
      btnPauseEtudiant(nullptr)
{
    setupUI();
}

ParametresCompteBase::ParametresCompteBase(Professeur *prof, std::vector<Professeur> &profsRef, QWidget *parent)
    : QWidget(parent), typeCompte(TypeCompte::Professeur),
      currentEtudiant(nullptr), comptesList(nullptr),
      currentProf(prof), profsList(&profsRef),
      currentAdmin(nullptr), adminsList(nullptr),
      lblId(nullptr), lblNomComplet(nullptr), lblRoleBadge(nullptr), lblStatutBadge(nullptr),
      editNom(nullptr), editPrenom(nullptr), editTel(nullptr), editEmail(nullptr),
      editNomProche(nullptr), editPrenomProche(nullptr), editTelProche(nullptr),
      lblFaculte(nullptr), lblMatieres(nullptr), lblPoste(nullptr), lblSalaireStatut(nullptr),
      btnPauseEtudiant(nullptr)
{
    setupUI();
}

ParametresCompteBase::ParametresCompteBase(Administrateur *admin, std::vector<Administrateur> &adminsRef, QWidget *parent)
    : QWidget(parent),
      currentEtudiant(nullptr), comptesList(nullptr),
      currentProf(nullptr), profsList(nullptr),
      currentAdmin(admin), adminsList(&adminsRef),
      lblId(nullptr), lblNomComplet(nullptr), lblRoleBadge(nullptr), lblStatutBadge(nullptr),
      editNom(nullptr), editPrenom(nullptr), editTel(nullptr), editEmail(nullptr),
      editNomProche(nullptr), editPrenomProche(nullptr), editTelProche(nullptr),
      lblFaculte(nullptr), lblMatieres(nullptr), lblPoste(nullptr), lblSalaireStatut(nullptr),
      btnPauseEtudiant(nullptr)
{
    std::string p = admin ? admin->getPoste() : "";
    if (p == "Recteur") typeCompte = TypeCompte::Recteur;
    else if (p == "Président" || p == "President") typeCompte = TypeCompte::President;
    else if (p.find("Secrétaire") != std::string::npos || p.find("Secretaire") != std::string::npos) typeCompte = TypeCompte::Secretaire;
    else typeCompte = TypeCompte::Coordonnateur;

    setupUI();
}

ParametresCompteBase::~ParametresCompteBase() {}

void ParametresCompteBase::setupUI()
{
    setStyleSheet("background-color: #f0f2f5; font-family: 'Segoe UI', sans-serif;");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *content = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(30, 25, 30, 30);
    layout->setSpacing(20);

    // Titre de la page
    QLabel *pageTitle = new QLabel("⚙️   Paramètres & Profil du Compte", content);
    pageTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; background: transparent;");
    layout->addWidget(pageTitle);

    // 1. Carte Profil / Identité
    layout->addWidget(createProfilCard());

    // 2. Carte Informations personnelles
    layout->addWidget(createInfosPersoCard());

    // 3. Carte Sécurité du compte
    layout->addWidget(createSecuriteCard());

    // 4. Carte Contact du proche (si applicable)
    layout->addWidget(createProcheCard());

    // 5. Carte Spécifique selon le rôle
    layout->addWidget(createRoleSpecifiqueCard());

    // 6. Carte Déconnexion
    layout->addWidget(createActionsCard());

    layout->addStretch();
    scroll->setWidget(content);
    rootLayout->addWidget(scroll);

    refreshData();
}

QWidget* ParametresCompteBase::createProfilCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(12);
    shadow->setColor(QColor(0, 0, 0, 10));
    shadow->setOffset(0, 3);
    card->setGraphicsEffect(shadow);

    QHBoxLayout *layout = new QHBoxLayout(card);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(20);

    QLabel *avatar = new QLabel("👤", card);
    avatar->setStyleSheet("font-size: 48px; background: #ebf8ff; border-radius: 35px; min-width: 70px; min-height: 70px; max-width: 70px; max-height: 70px; border: 2px solid #bee3f8;");
    avatar->setAlignment(Qt::AlignCenter);
    layout->addWidget(avatar);

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);

    lblNomComplet = new QLabel("—", card);
    lblNomComplet->setStyleSheet("font-size: 20px; font-weight: bold; color: #1a202c; border: none;");
    infoLayout->addWidget(lblNomComplet);

    QHBoxLayout *badgesLayout = new QHBoxLayout();
    badgesLayout->setSpacing(10);

    lblId = new QLabel("ID: —", card);
    lblId->setStyleSheet("font-size: 13px; font-weight: bold; color: #4a5568; background: #edf2f7; padding: 4px 10px; border-radius: 6px; border: none;");
    badgesLayout->addWidget(lblId);

    lblRoleBadge = new QLabel("Rôle", card);
    lblRoleBadge->setStyleSheet("font-size: 12px; font-weight: bold; color: #2b6cb0; background: #ebf8ff; padding: 4px 10px; border-radius: 6px; border: 1px solid #bee3f8;");
    badgesLayout->addWidget(lblRoleBadge);

    lblStatutBadge = new QLabel("Statut", card);
    lblStatutBadge->setStyleSheet("font-size: 12px; font-weight: bold; color: #27ae60; background: #e8f8f0; padding: 4px 10px; border-radius: 6px; border: 1px solid #c6f6d5;");
    badgesLayout->addWidget(lblStatutBadge);

    badgesLayout->addStretch();
    infoLayout->addLayout(badgesLayout);

    layout->addLayout(infoLayout, 1);
    return card;
}

QWidget* ParametresCompteBase::createInfosPersoCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *t = new QLabel("📝   Informations Personnelles", card);
    t->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    layout->addWidget(t);

    const QString inputStyle = "QLineEdit { background-color: #f8fafc; color: #2d3748; border: 1.5px solid #cbd5e0; "
                               "border-radius: 6px; padding: 6px 12px; font-size: 13px; }"
                               "QLineEdit:focus { border: 1.5px solid #3182ce; background-color: #ffffff; }";
    const QString btnStyle = "QPushButton { background-color: #edf2f7; color: #2b6cb0; border: 1px solid #cbd5e0; "
                             "border-radius: 6px; padding: 6px 14px; font-size: 12px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #ebf8ff; border-color: #3182ce; }";

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(15);
    grid->setVerticalSpacing(10);

    // Nom
    grid->addWidget(new QLabel("Nom de famille :", card), 0, 0);
    editNom = new QLineEdit(card); editNom->setStyleSheet(inputStyle); editNom->setFixedHeight(36);
    grid->addWidget(editNom, 0, 1);
    QPushButton *btnNom = new QPushButton("Modifier", card); btnNom->setStyleSheet(btnStyle); btnNom->setFixedHeight(36);
    connect(btnNom, &QPushButton::clicked, this, &ParametresCompteBase::onModifierNom);
    grid->addWidget(btnNom, 0, 2);

    // Prénom
    grid->addWidget(new QLabel("Prénom :", card), 1, 0);
    editPrenom = new QLineEdit(card); editPrenom->setStyleSheet(inputStyle); editPrenom->setFixedHeight(36);
    grid->addWidget(editPrenom, 1, 1);
    QPushButton *btnPrenom = new QPushButton("Modifier", card); btnPrenom->setStyleSheet(btnStyle); btnPrenom->setFixedHeight(36);
    connect(btnPrenom, &QPushButton::clicked, this, &ParametresCompteBase::onModifierPrenom);
    grid->addWidget(btnPrenom, 1, 2);

    // Téléphone
    grid->addWidget(new QLabel("Numéro de téléphone :", card), 2, 0);
    editTel = new QLineEdit(card); editTel->setStyleSheet(inputStyle); editTel->setFixedHeight(36);
    grid->addWidget(editTel, 2, 1);
    QPushButton *btnTel = new QPushButton("Modifier", card); btnTel->setStyleSheet(btnStyle); btnTel->setFixedHeight(36);
    connect(btnTel, &QPushButton::clicked, this, &ParametresCompteBase::onModifierTelephone);
    grid->addWidget(btnTel, 2, 2);

    // Email
    grid->addWidget(new QLabel("Adresse email :", card), 3, 0);
    editEmail = new QLineEdit(card); editEmail->setStyleSheet(inputStyle); editEmail->setFixedHeight(36);
    grid->addWidget(editEmail, 3, 1);
    QPushButton *btnEmail = new QPushButton("Modifier", card); btnEmail->setStyleSheet(btnStyle); btnEmail->setFixedHeight(36);
    connect(btnEmail, &QPushButton::clicked, this, &ParametresCompteBase::onModifierEmail);
    grid->addWidget(btnEmail, 3, 2);

    layout->addLayout(grid);
    return card;
}

QWidget* ParametresCompteBase::createSecuriteCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #f7fafc; border-radius: 12px; border: 1.5px solid #e2e8f0; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *t = new QLabel("🔒   Sécurité & Mot de Passe", card);
    t->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    layout->addWidget(t);

    QLabel *desc = new QLabel("Pour garantir la sécurité de votre compte UDH, utilisez un mot de passe robuste d'au moins 4 caractères.", card);
    desc->setStyleSheet("font-size: 12px; color: #718096; border: none;");
    layout->addWidget(desc);

    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setSpacing(15);

    QPushButton *btnChangePwd = new QPushButton("🔑 Modifier mon mot de passe", card);
    btnChangePwd->setFixedHeight(40);
    btnChangePwd->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 16px; border: none; }"
                                "QPushButton:hover { background-color: #1a3a60; }");
    connect(btnChangePwd, &QPushButton::clicked, this, &ParametresCompteBase::onModifierMotDePasse);
    btnsLayout->addWidget(btnChangePwd);

    QPushButton *btnResetPwd = new QPushButton("🔄 Réinitialiser le mot de passe", card);
    btnResetPwd->setFixedHeight(40);
    btnResetPwd->setStyleSheet("QPushButton { background-color: #ffffff; color: #c53030; border: 1.5px solid #feb2b2; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 16px; }"
                               "QPushButton:hover { background-color: #fff5f5; border-color: #e53e3e; }");
    connect(btnResetPwd, &QPushButton::clicked, this, &ParametresCompteBase::onReinitialiserMotDePasse);
    btnsLayout->addWidget(btnResetPwd);

    btnsLayout->addStretch();
    layout->addLayout(btnsLayout);

    return card;
}

QWidget* ParametresCompteBase::createProcheCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *t = new QLabel("👥   Contact du Proche / Responsable d'Urgence", card);
    t->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    layout->addWidget(t);

    const QString inputStyle = "QLineEdit { background-color: #f8fafc; color: #2d3748; border: 1.5px solid #cbd5e0; "
                               "border-radius: 6px; padding: 6px 12px; font-size: 13px; }"
                               "QLineEdit:focus { border: 1.5px solid #3182ce; background-color: #ffffff; }";

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(15);
    grid->setVerticalSpacing(10);

    grid->addWidget(new QLabel("Nom du proche :", card), 0, 0);
    editNomProche = new QLineEdit(card); editNomProche->setStyleSheet(inputStyle); editNomProche->setFixedHeight(36);
    grid->addWidget(editNomProche, 0, 1);

    grid->addWidget(new QLabel("Prénom du proche :", card), 1, 0);
    editPrenomProche = new QLineEdit(card); editPrenomProche->setStyleSheet(inputStyle); editPrenomProche->setFixedHeight(36);
    grid->addWidget(editPrenomProche, 1, 1);

    grid->addWidget(new QLabel("Téléphone du proche :", card), 2, 0);
    editTelProche = new QLineEdit(card); editTelProche->setStyleSheet(inputStyle); editTelProche->setFixedHeight(36);
    grid->addWidget(editTelProche, 2, 1);

    QPushButton *btnProche = new QPushButton("💾 Enregistrer le contact du proche", card);
    btnProche->setFixedHeight(36);
    btnProche->setStyleSheet("QPushButton { background-color: #2b6cb0; color: white; border-radius: 6px; font-weight: bold; font-size: 12px; border: none; padding: 0 16px; }"
                             "QPushButton:hover { background-color: #2c5282; }");
    connect(btnProche, &QPushButton::clicked, this, &ParametresCompteBase::onModifierProche);
    grid->addWidget(btnProche, 3, 1, 1, 1, Qt::AlignLeft);

    layout->addLayout(grid);
    return card;
}

QWidget* ParametresCompteBase::createRoleSpecifiqueCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *t = new QLabel("🏛️   Détails Institutionnels & Statut", card);
    t->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    layout->addWidget(t);

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(15);
    grid->setVerticalSpacing(10);

    if (typeCompte == TypeCompte::Etudiant) {
        grid->addWidget(new QLabel("Faculté / Filière :", card), 0, 0);
        lblFaculte = new QLabel("—", card);
        lblFaculte->setStyleSheet("font-size: 13px; font-weight: bold; color: #2d3748;");
        grid->addWidget(lblFaculte, 0, 1);

        grid->addWidget(new QLabel("Dossier d'apprentissage :", card), 1, 0);
        btnPauseEtudiant = new QPushButton("⏸ Mettre mes études en pause", card);
        btnPauseEtudiant->setFixedHeight(36);
        btnPauseEtudiant->setStyleSheet("QPushButton { background-color: #d69e2e; color: white; border-radius: 6px; font-weight: bold; font-size: 12px; border: none; padding: 0 14px; }"
                                        "QPushButton:hover { background-color: #b7791f; }");
        connect(btnPauseEtudiant, &QPushButton::clicked, this, &ParametresCompteBase::onBasculerPauseEtudiant);
        grid->addWidget(btnPauseEtudiant, 1, 1, Qt::AlignLeft);
    }
    else if (typeCompte == TypeCompte::Professeur) {
        grid->addWidget(new QLabel("Faculté d'affectation :", card), 0, 0);
        lblFaculte = new QLabel("—", card);
        lblFaculte->setStyleSheet("font-size: 13px; font-weight: bold; color: #2d3748;");
        grid->addWidget(lblFaculte, 0, 1);

        grid->addWidget(new QLabel("Matières enseignées :", card), 1, 0);
        lblMatieres = new QLabel("—", card);
        lblMatieres->setStyleSheet("font-size: 13px; font-weight: bold; color: #2b6cb0;");
        grid->addWidget(lblMatieres, 1, 1);

        grid->addWidget(new QLabel("Rémunération / Salaire :", card), 2, 0);
        lblSalaireStatut = new QLabel("—", card);
        grid->addWidget(lblSalaireStatut, 2, 1);
    }
    else { // Admins (Recteur, Président, Secrétaire, Coordonnateur)
        grid->addWidget(new QLabel("Poste / Fonction officielle :", card), 0, 0);
        lblPoste = new QLabel("—", card);
        lblPoste->setStyleSheet("font-size: 13px; font-weight: bold; color: #1a202c;");
        grid->addWidget(lblPoste, 0, 1);

        if (typeCompte == TypeCompte::Coordonnateur) {
            grid->addWidget(new QLabel("Faculté assignée :", card), 1, 0);
            lblFaculte = new QLabel("—", card);
            lblFaculte->setStyleSheet("font-size: 13px; font-weight: bold; color: #2b6cb0;");
            grid->addWidget(lblFaculte, 1, 1);
        }

        grid->addWidget(new QLabel("Rémunération / Salaire :", card), 2, 0);
        lblSalaireStatut = new QLabel("—", card);
        grid->addWidget(lblSalaireStatut, 2, 1);
    }

    layout->addLayout(grid);
    return card;
}

QWidget* ParametresCompteBase::createActionsCard()
{
    QFrame *card = new QFrame();
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QHBoxLayout *layout = new QHBoxLayout(card);
    layout->setContentsMargins(25, 18, 25, 18);

    QLabel *info = new QLabel("Session active sur le portail UDH", card);
    info->setStyleSheet("font-size: 13px; color: #718096; border: none;");
    layout->addWidget(info);

    layout->addStretch();

    QPushButton *btnLogout = new QPushButton("🚪 Déconnexion", card);
    btnLogout->setFixedHeight(38);
    btnLogout->setStyleSheet("QPushButton { background-color: #fff5f5; color: #e53e3e; border: 1.5px solid #feb2b2; "
                             "border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 20px; }"
                             "QPushButton:hover { background-color: #fed7d7; }");
    connect(btnLogout, &QPushButton::clicked, this, &ParametresCompteBase::logoutRequested);
    layout->addWidget(btnLogout);

    return card;
}

void ParametresCompteBase::refreshData()
{
    if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
        if (lblNomComplet) lblNomComplet->setText(QString::fromStdString(currentEtudiant->getNom() + " " + currentEtudiant->getPrenom()));
        if (lblId) lblId->setText("ID: " + QString::fromStdString(currentEtudiant->getId()));
        if (lblRoleBadge) lblRoleBadge->setText("Étudiant");
        
        QString stText = currentEtudiant->estEnPause() ? "En pause" : (currentEtudiant->estActif() ? "Actif" : "Inactif");
        if (lblStatutBadge) {
            lblStatutBadge->setText(stText);
            if (currentEtudiant->estActif() && !currentEtudiant->estEnPause()) {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #03543F; background: #DEF7EC; padding: 4px 10px; border-radius: 12px; border: 1px solid #BCF0DA;");
            } else if (currentEtudiant->estEnPause()) {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #92400E; background: #FEF3C7; padding: 4px 10px; border-radius: 12px; border: 1px solid #FDE68A;");
            } else {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #9B1C1C; background: #FDE8E8; padding: 4px 10px; border-radius: 12px; border: 1px solid #FBD5D5;");
            }
        }

        if (editNom) editNom->setText(QString::fromStdString(currentEtudiant->getNom()));
        if (editPrenom) editPrenom->setText(QString::fromStdString(currentEtudiant->getPrenom()));
        if (editTel) editTel->setText(QString::fromStdString(currentEtudiant->getTelephone()));
        if (editEmail) editEmail->setText(QString::fromStdString(currentEtudiant->getEmail()));

        if (editNomProche) editNomProche->setText(QString::fromStdString(currentEtudiant->getInscription().nomProche));
        if (editPrenomProche) editPrenomProche->setText(QString::fromStdString(currentEtudiant->getInscription().prenomProche));
        if (editTelProche) editTelProche->setText(QString::fromStdString(currentEtudiant->getInscription().telProche));

        if (lblFaculte) lblFaculte->setText(QString::fromStdString(currentEtudiant->getFaculte().empty() ? "Non renseignée" : currentEtudiant->getFaculte()));
        if (btnPauseEtudiant) {
            btnPauseEtudiant->setText(currentEtudiant->estEnPause() ? "▶ Reprendre mes études" : "⏸ Mettre mes études en pause");
            btnPauseEtudiant->setStyleSheet(currentEtudiant->estEnPause()
                ? "QPushButton { background-color: #38a169; color: white; border-radius: 6px; font-weight: bold; font-size: 12px; border: none; padding: 0 14px; }"
                : "QPushButton { background-color: #d69e2e; color: white; border-radius: 6px; font-weight: bold; font-size: 12px; border: none; padding: 0 14px; }");
        }
    }
    else if (typeCompte == TypeCompte::Professeur && currentProf) {
        if (lblNomComplet) lblNomComplet->setText(QString::fromStdString(currentProf->getNom() + " " + currentProf->getPrenom()));
        if (lblId) lblId->setText("ID: " + QString::fromStdString(currentProf->getId()));
        if (lblRoleBadge) lblRoleBadge->setText("Professeur");

        QString stText = QString::fromStdString(currentProf->getStatutCompte());
        if (lblStatutBadge) {
            lblStatutBadge->setText(stText);
            if (stText == "Actif") {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #03543F; background: #DEF7EC; padding: 4px 10px; border-radius: 12px; border: 1px solid #BCF0DA;");
            } else if (stText == "En congé") {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #92400E; background: #FEF3C7; padding: 4px 10px; border-radius: 12px; border: 1px solid #FDE68A;");
            } else {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #9B1C1C; background: #FDE8E8; padding: 4px 10px; border-radius: 12px; border: 1px solid #FBD5D5;");
            }
        }

        if (editNom) editNom->setText(QString::fromStdString(currentProf->getNom()));
        if (editPrenom) editPrenom->setText(QString::fromStdString(currentProf->getPrenom()));
        if (editTel) editTel->setText(QString::fromStdString(currentProf->getTelephone()));
        if (editEmail) editEmail->setText(QString::fromStdString(currentProf->getEmail()));

        // Professeur proche information
        if (lblFaculte) lblFaculte->setText(QString::fromStdString(currentProf->getFaculte()));
        
        QString mats = "";
        for (size_t i = 0; i < currentProf->getMatieres().size(); ++i) {
            if (i > 0) mats += ", ";
            mats += QString::fromStdString(currentProf->getMatieres()[i]);
        }
        if (lblMatieres) lblMatieres->setText(mats.isEmpty() ? "Aucune matière assignée" : mats);

        if (lblSalaireStatut) {
            if (currentProf->getEstPaye()) {
                lblSalaireStatut->setText(QString("🟢 Payé (%1 HTG)").arg(currentProf->getMontantSalaire(), 0, 'f', 0));
                lblSalaireStatut->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 13px;");
            } else {
                lblSalaireStatut->setText("🔴 Non payé ce mois");
                lblSalaireStatut->setStyleSheet("color: #e53e3e; font-weight: bold; font-size: 13px;");
            }
        }
    }
    else if (currentAdmin) {
        if (lblNomComplet) lblNomComplet->setText(QString::fromStdString(currentAdmin->getNom() + " " + currentAdmin->getPrenom()));
        if (lblId) lblId->setText("ID: " + QString::fromStdString(currentAdmin->getId()));
        if (lblRoleBadge) lblRoleBadge->setText(QString::fromStdString(currentAdmin->getPoste()));

        QString stText = QString::fromStdString(currentAdmin->getStatutCompte());
        if (lblStatutBadge) {
            lblStatutBadge->setText(stText);
            if (stText == "Actif") {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #03543F; background: #DEF7EC; padding: 4px 10px; border-radius: 12px; border: 1px solid #BCF0DA;");
            } else if (stText == "En congé") {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #92400E; background: #FEF3C7; padding: 4px 10px; border-radius: 12px; border: 1px solid #FDE68A;");
            } else {
                lblStatutBadge->setStyleSheet("font-size: 11px; font-weight: bold; color: #9B1C1C; background: #FDE8E8; padding: 4px 10px; border-radius: 12px; border: 1px solid #FBD5D5;");
            }
        }

        if (editNom) editNom->setText(QString::fromStdString(currentAdmin->getNom()));
        if (editPrenom) editPrenom->setText(QString::fromStdString(currentAdmin->getPrenom()));
        if (editTel) editTel->setText(QString::fromStdString(currentAdmin->getTelephone()));
        if (editEmail) editEmail->setText(QString::fromStdString(currentAdmin->getEmail()));

        if (editNomProche) editNomProche->setText(QString::fromStdString(currentAdmin->getNomProche()));
        if (editPrenomProche) editPrenomProche->setText(QString::fromStdString(currentAdmin->getPrenomProche()));
        if (editTelProche) editTelProche->setText(QString::fromStdString(currentAdmin->getTelProche()));

        if (lblPoste) lblPoste->setText(QString::fromStdString(currentAdmin->getPoste()));
        if (lblFaculte) lblFaculte->setText(QString::fromStdString(currentAdmin->getFaculte()));

        if (lblSalaireStatut) {
            if (currentAdmin->getEstPaye()) {
                lblSalaireStatut->setText(QString("🟢 Payé (%1 HTG)").arg(currentAdmin->getMontantSalaire(), 0, 'f', 0));
                lblSalaireStatut->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 13px;");
            } else {
                lblSalaireStatut->setText("🔴 Non payé ce mois");
                lblSalaireStatut->setStyleSheet("color: #e53e3e; font-weight: bold; font-size: 13px;");
            }
        }
    }
}

void ParametresCompteBase::sauvegarderDonnees()
{
    try {
        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            if (comptesList) {
                bool found = false;
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId() || (!c.getEmail().empty() && c.getEmail() == currentEtudiant->getEmail())) {
                        c = *currentEtudiant;
                        found = true;
                        break;
                    }
                }
                if (!found) comptesList->push_back(*currentEtudiant);
                sauvegarderComptes(*comptesList);
            } else {
                std::vector<CompteEtudiant> allComptes = chargerComptes();
                enregistrerCompte(allComptes, *currentEtudiant);
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            if (profsList) {
                bool found = false;
                for (auto &p : *profsList) {
                    if (p.getId() == currentProf->getId() || (!p.getEmail().empty() && p.getEmail() == currentProf->getEmail())) {
                        p = *currentProf;
                        found = true;
                        break;
                    }
                }
                if (!found) profsList->push_back(*currentProf);
                sauvegarderProfesseurs(*profsList);
            } else {
                std::vector<Professeur> allProfs = chargerProfesseurs();
                enregistrerProfesseur(allProfs, *currentProf);
            }
        }
        else if (currentAdmin) {
            if (adminsList) {
                bool found = false;
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId() || (!a.getEmail().empty() && a.getEmail() == currentAdmin->getEmail())) {
                        a = *currentAdmin;
                        found = true;
                        break;
                    }
                }
                if (!found) adminsList->push_back(*currentAdmin);
                sauvegarderAdministrateurs(*adminsList);
            } else {
                std::vector<Administrateur> allAdmins = chargerAdministrateurs();
                enregistrerAdministrateur(allAdmins, *currentAdmin);
            }
        }
        // Émission asynchrone sécurisée pour éviter tout conflit avec la pile d'exécution du widget actif
        QMetaObject::invokeMethod(this, &ParametresCompteBase::profilUpdated, Qt::QueuedConnection);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur de sauvegarde", QString("Impossible d'enregistrer les données : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur de sauvegarde", "Une erreur inattendue est survenue lors de l'enregistrement.");
    }
}

void ParametresCompteBase::propagerNouveauId(const std::string &ancienId, const std::string &nouvelId)
{
    if (ancienId.empty() || nouvelId.empty() || ancienId == nouvelId) return;

    try {
        // Propager aux versements
        std::vector<VersementEtudiant> versements = chargerVersements();
        bool versModif = false;
        for (auto &v : versements) {
            if (v.idEtudiant == ancienId) {
                v.idEtudiant = nouvelId;
                versModif = true;
            }
        }
        if (versModif) sauvegarderVersements(versements);

        // Propager aux notes des professeurs
        std::vector<Professeur> allProfs = chargerProfesseurs();
        bool profModif = false;
        for (auto &p : allProfs) {
            auto notes = p.getNotes();
            bool localMod = false;
            for (auto &n : notes) {
                if (n.idEtudiant == ancienId) {
                    n.idEtudiant = nouvelId;
                    localMod = true;
                    profModif = true;
                }
            }
            if (localMod) p.setNotes(notes);
        }
        if (profModif) {
            sauvegarderProfesseurs(allProfs);
            if (profsList) *profsList = allProfs;
        }
    } catch (...) {
        // Safe fallback
    }
}

void ParametresCompteBase::onModifierNom()
{
    try {
        QString nvNom = editNom ? editNom->text().trimmed() : "";
        if (nvNom.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Le nom ne peut pas être vide.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            std::string ancienId = currentEtudiant->getId();
            currentEtudiant->modifierNom(nvNom.toStdString());
            std::string nouvelId = currentEtudiant->getId();

            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == ancienId || (!c.getEmail().empty() && c.getEmail() == currentEtudiant->getEmail())) {
                        c = *currentEtudiant;
                        break;
                    }
                }
            }

            if (ancienId != nouvelId) {
                propagerNouveauId(ancienId, nouvelId);
                QMessageBox::information(this, "Identifiant mis à jour",
                    QString("Votre nom a été modifié. La première lettre ayant changé, votre nouvel ID est : %1").arg(QString::fromStdString(nouvelId)));
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            std::string ancienId = currentProf->getId();
            currentProf->modifierNom(nvNom.toStdString());
            std::string nouvelId = currentProf->getId();

            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == ancienId || (!p.getEmail().empty() && p.getEmail() == currentProf->getEmail())) {
                        p = *currentProf;
                        break;
                    }
                }
            }

            if (ancienId != nouvelId) {
                QMessageBox::information(this, "Identifiant mis à jour",
                    QString("Votre nom a été modifié. Votre nouvel ID est : %1").arg(QString::fromStdString(nouvelId)));
            }
        }
        else if (currentAdmin) {
            std::string ancienId = currentAdmin->getId();
            currentAdmin->setNom(nvNom.toStdString());

            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == ancienId || (!a.getEmail().empty() && a.getEmail() == currentAdmin->getEmail())) {
                        a = *currentAdmin;
                        break;
                    }
                }
            }
        }

        sauvegarderDonnees();
        refreshData();
        QMessageBox::information(this, "Succès", "Nom de famille mis à jour avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de modifier le nom : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onModifierPrenom()
{
    try {
        QString nvPrenom = editPrenom ? editPrenom->text().trimmed() : "";
        if (nvPrenom.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Le prénom ne peut pas être vide.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            std::string ancienId = currentEtudiant->getId();
            currentEtudiant->modifierPrenom(nvPrenom.toStdString());
            std::string nouvelId = currentEtudiant->getId();

            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == ancienId || (!c.getEmail().empty() && c.getEmail() == currentEtudiant->getEmail())) {
                        c = *currentEtudiant;
                        break;
                    }
                }
            }

            if (ancienId != nouvelId) {
                propagerNouveauId(ancienId, nouvelId);
                QMessageBox::information(this, "Identifiant mis à jour",
                    QString("Votre prénom a été modifié. Votre nouvel ID est : %1").arg(QString::fromStdString(nouvelId)));
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            std::string ancienId = currentProf->getId();
            currentProf->modifierPrenom(nvPrenom.toStdString());
            std::string nouvelId = currentProf->getId();

            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == ancienId || (!p.getEmail().empty() && p.getEmail() == currentProf->getEmail())) {
                        p = *currentProf;
                        break;
                    }
                }
            }

            if (ancienId != nouvelId) {
                QMessageBox::information(this, "Identifiant mis à jour",
                    QString("Votre prénom a été modifié. Votre nouvel ID est : %1").arg(QString::fromStdString(nouvelId)));
            }
        }
        else if (currentAdmin) {
            std::string ancienId = currentAdmin->getId();
            currentAdmin->setPrenom(nvPrenom.toStdString());

            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == ancienId || (!a.getEmail().empty() && a.getEmail() == currentAdmin->getEmail())) {
                        a = *currentAdmin;
                        break;
                    }
                }
            }
        }

        sauvegarderDonnees();
        refreshData();
        QMessageBox::information(this, "Succès", "Prénom mis à jour avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de modifier le prénom : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onModifierTelephone()
{
    try {
        QString nvTel = editTel ? editTel->text().trimmed() : "";
        if (nvTel.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Le numéro de téléphone ne peut pas être vide.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->modifierTelephone(nvTel.toStdString());
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) { c.modifierTelephone(nvTel.toStdString()); break; }
                }
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            currentProf->modifierTelephone(nvTel.toStdString());
            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == currentProf->getId()) { p.modifierTelephone(nvTel.toStdString()); break; }
                }
            }
        }
        else if (currentAdmin) {
            currentAdmin->setTelephone(nvTel.toStdString());
            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId()) { a.setTelephone(nvTel.toStdString()); break; }
                }
            }
        }

        sauvegarderDonnees();
        refreshData();
        QMessageBox::information(this, "Succès", "Numéro de téléphone mis à jour avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de modifier le téléphone : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onModifierEmail()
{
    try {
        QString nvEmail = editEmail ? editEmail->text().trimmed() : "";
        if (nvEmail.isEmpty() || !nvEmail.contains("@")) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir une adresse email valide.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->modifierEmail(nvEmail.toStdString());
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) { c.modifierEmail(nvEmail.toStdString()); break; }
                }
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            currentProf->modifierEmail(nvEmail.toStdString());
            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == currentProf->getId()) { p.modifierEmail(nvEmail.toStdString()); break; }
                }
            }
        }
        else if (currentAdmin) {
            currentAdmin->setEmail(nvEmail.toStdString());
            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId()) { a.setEmail(nvEmail.toStdString()); break; }
                }
            }
        }

        sauvegarderDonnees();
        refreshData();
        QMessageBox::information(this, "Succès", "Adresse email mise à jour avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de modifier l'email : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onModifierMotDePasse()
{
    try {
        bool ok;
        QString curPwd = QInputDialog::getText(this, "Sécurité", "Mot de passe actuel :", QLineEdit::Password, "", &ok);
        if (!ok || curPwd.isEmpty()) return;

        bool verifOk = false;
        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) verifOk = currentEtudiant->verifierMotDePasse(curPwd.toStdString());
        else if (typeCompte == TypeCompte::Professeur && currentProf) verifOk = currentProf->verifierMotDePasse(curPwd.toStdString());
        else if (currentAdmin) verifOk = currentAdmin->verifierMotDePasse(curPwd.toStdString());

        if (!verifOk) {
            QMessageBox::warning(this, "Erreur", "Le mot de passe actuel est incorrect.");
            return;
        }

        QString nvPwd = QInputDialog::getText(this, "Sécurité", "Nouveau mot de passe (min. 4 caractères) :", QLineEdit::Password, "", &ok);
        if (!ok || nvPwd.length() < 4) {
            QMessageBox::warning(this, "Erreur", "Le mot de passe doit contenir au moins 4 caractères.");
            return;
        }

        QString confPwd = QInputDialog::getText(this, "Sécurité", "Confirmez le nouveau mot de passe :", QLineEdit::Password, "", &ok);
        if (!ok || nvPwd != confPwd) {
            QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->modifierMotDePasse(nvPwd.toStdString());
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) { c.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            currentProf->modifierMotDePasse(nvPwd.toStdString());
            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == currentProf->getId()) { p.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }
        else if (currentAdmin) {
            currentAdmin->modifierMotDePasse(nvPwd.toStdString());
            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId()) { a.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }

        sauvegarderDonnees();
        QMessageBox::information(this, "Succès", "Votre mot de passe a été modifié avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de changer le mot de passe : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onReinitialiserMotDePasse()
{
    try {
        bool ok;
        QString emailConf = QInputDialog::getText(this, "Réinitialisation", "Confirmez votre adresse email enregistrée :", QLineEdit::Normal, "", &ok);
        if (!ok || emailConf.trimmed().isEmpty()) return;

        std::string emailCompte = "";
        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) emailCompte = currentEtudiant->getEmail();
        else if (typeCompte == TypeCompte::Professeur && currentProf) emailCompte = currentProf->getEmail();
        else if (currentAdmin) emailCompte = currentAdmin->getEmail();

        if (emailConf.trimmed().toStdString() != emailCompte) {
            QMessageBox::warning(this, "Erreur", "L'adresse email ne correspond pas à ce compte.");
            return;
        }

        QString nvPwd = QInputDialog::getText(this, "Nouveau mot de passe", "Saisissez votre nouveau mot de passe :", QLineEdit::Password, "", &ok);
        if (!ok || nvPwd.length() < 4) {
            QMessageBox::warning(this, "Erreur", "Le mot de passe doit comporter au moins 4 caractères.");
            return;
        }

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->modifierMotDePasse(nvPwd.toStdString());
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) { c.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }
        else if (typeCompte == TypeCompte::Professeur && currentProf) {
            currentProf->modifierMotDePasse(nvPwd.toStdString());
            if (profsList) {
                for (auto &p : *profsList) {
                    if (p.getId() == currentProf->getId()) { p.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }
        else if (currentAdmin) {
            currentAdmin->modifierMotDePasse(nvPwd.toStdString());
            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId()) { a.modifierMotDePasse(nvPwd.toStdString()); break; }
                }
            }
        }

        sauvegarderDonnees();
        QMessageBox::information(this, "Succès", "Votre mot de passe a été réinitialisé avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de réinitialiser le mot de passe : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onModifierProche()
{
    try {
        QString nProche = editNomProche ? editNomProche->text().trimmed() : "";
        QString pProche = editPrenomProche ? editPrenomProche->text().trimmed() : "";
        QString tProche = editTelProche ? editTelProche->text().trimmed() : "";

        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->modifierProche(nProche.toStdString(), pProche.toStdString(), tProche.toStdString());
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) {
                        c.modifierProche(nProche.toStdString(), pProche.toStdString(), tProche.toStdString());
                        break;
                    }
                }
            }
        }
        else if (currentAdmin) {
            currentAdmin->setNomProche(nProche.toStdString());
            currentAdmin->setPrenomProche(pProche.toStdString());
            currentAdmin->setTelProche(tProche.toStdString());
            if (adminsList) {
                for (auto &a : *adminsList) {
                    if (a.getId() == currentAdmin->getId()) {
                        a.setNomProche(nProche.toStdString());
                        a.setPrenomProche(pProche.toStdString());
                        a.setTelProche(tProche.toStdString());
                        break;
                    }
                }
            }
        }

        sauvegarderDonnees();
        refreshData();
        QMessageBox::information(this, "Succès", "Informations du contact proche enregistrées avec succès.");
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible d'enregistrer le contact du proche : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}

void ParametresCompteBase::onBasculerPauseEtudiant()
{
    try {
        if (typeCompte == TypeCompte::Etudiant && currentEtudiant) {
            currentEtudiant->basculerPause();
            if (comptesList) {
                for (auto &c : *comptesList) {
                    if (c.getId() == currentEtudiant->getId()) {
                        c.setEnPause(currentEtudiant->estEnPause());
                        break;
                    }
                }
            }
            sauvegarderDonnees();
            refreshData();
            QMessageBox::information(this, "Statut Scolarité",
                currentEtudiant->estEnPause()
                ? "Votre dossier d'apprentissage est désormais EN PAUSE."
                : "Votre dossier d'apprentissage a été RÉACTIVÉ.");
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Erreur", QString("Impossible de modifier le statut : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Une erreur inattendue est survenue.");
    }
}
