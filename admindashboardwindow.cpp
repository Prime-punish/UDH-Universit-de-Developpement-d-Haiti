#include "admindashboardwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QHeaderView>
#include <QMessageBox>
#include <set>

AdminDashboardWindow::AdminDashboardWindow(Administrateur admin, std::vector<Administrateur> &adminsRef, std::vector<Professeur> &profsRef, std::vector<CompteEtudiant> &comptesRef, QWidget *parent)
    : QMainWindow(parent), currentAdmin(admin), admins(adminsRef), profs(profsRef), comptes(comptesRef),
      m_faculteAssignee(QString::fromStdString(currentAdmin.getFaculte()).trimmed()),
      stackedWidget(nullptr), sidebarBtnGroup(nullptr),
      gestionSalairesWidget(nullptr), validationInscriptionsWidget(nullptr),
      gestionAnnoncesWidget(nullptr), annoncesRecuesWidget(nullptr),
      gestionCourrierWidget(nullptr), secretaryRdvWidget(nullptr),
      secCalWidget(nullptr), secReunionsWidget(nullptr),
      dossierNavBarWidget(nullptr), presidentConseilWidget(nullptr),
      presidentFinanceWidget(nullptr), recteurFinanceWidget(nullptr), adminEventsWidget(nullptr),
      supervisionWidget(nullptr), rapportGraphiqueWidget(nullptr),
      recteurVueEnsembleWidget(nullptr), recteurGestionMembresWidget(nullptr),
      recteurValidationWidget(nullptr), recteurGestionFacultesWidget(nullptr),
      cardSecNotesPending(nullptr), cardSecNotesTransferred(nullptr), cardSecNotesTotal(nullptr), secNotesTable(nullptr), secNotesFilterCombo(nullptr),
      cardCoordFacProfs(nullptr), cardCoordFacStudents(nullptr), cardCoordFacYears(nullptr), cardCoordFacCourses(nullptr),
      coordFacStudentsTable(nullptr), coordFacStudentSearch(nullptr),
      cardCoordProfsTotal(nullptr), cardCoordProfsPaid(nullptr), cardCoordProfsCourses(nullptr),
      coordProfsTable(nullptr), coordProfsSearch(nullptr),
      coordProgramYearCombo(nullptr), coordProgramTable(nullptr), coordProgramSummaryLbl(nullptr),
      coordEdtYearCombo(nullptr), coordEdtTable(nullptr),
      cardCoordNotesCount(nullptr), cardCoordNotesAverage(nullptr), cardCoordStudentsCount(nullptr),
      coordResultsTable(nullptr), coordResultsSearch(nullptr)
{
    setupUI();
}

AdminDashboardWindow::~AdminDashboardWindow() {}

void AdminDashboardWindow::setupUI()
{
    setMinimumSize(1100, 700);
    setWindowTitle(QString("UDH - Administration (%1)").arg(QString::fromStdString(currentAdmin.getPoste())));

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #f0f2f5; font-family: 'Segoe UI', sans-serif;");
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    QWidget *sidebar = createSidebar();
    mainLayout->addWidget(sidebar);

    // Main Content StackedWidget
    stackedWidget = new QStackedWidget(this);

    std::string p = currentAdmin.getPoste();

    if (p == "Recteur") {
        stackedWidget->addWidget(createProfilePage()); // 0: PROFIL
        stackedWidget->addWidget(createRecteurVueEnsemblePage()); // 1: VUE D'ENSEMBLE
        stackedWidget->addWidget(createRecteurGestionMembresPage()); // 2: GESTION DES MEMBRES
        stackedWidget->addWidget(createRecteurValidationAcademiquePage()); // 3: VALIDATION ACADÉMIQUE
        stackedWidget->addWidget(createRecteurGestionFacultesPage()); // 4: GESTION DES FACULTÉS
        stackedWidget->addWidget(createRecteurFinancePage()); // 5: GESTION FINANCIÈRE
        stackedWidget->addWidget(createAdminEventsPage()); // 6: ÉVÉNEMENTS ADMIN
        stackedWidget->addWidget(createSettingsPage()); // 7: PARAMÈTRES (UNIFIÉ)
    }
    else if (p == "Président" || p == "President") {
        stackedWidget->addWidget(createProfilePage()); // 0: PROFIL
        stackedWidget->addWidget(createPresidentConseilPage()); // 1: CONSEIL D'ADMINISTRATION
        stackedWidget->addWidget(createPresidentFinancePage()); // 2: GESTION FINANCIÈRE
        stackedWidget->addWidget(createAdminEventsPage()); // 3: ÉVÉNEMENTS ADMIN
        stackedWidget->addWidget(createSupervisionPage()); // 4: SUPERVISION
        stackedWidget->addWidget(createRapportGraphiquePage()); // 5: RAPPORT (QCHART)
        stackedWidget->addWidget(createAnnoncesRecuesPage()); // 6: ANNONCES
        stackedWidget->addWidget(createSettingsPage()); // 7: PARAMÈTRES (UNIFIÉ)
    }
    else if (p.find("Secrétaire") != std::string::npos || p.find("Secretaire") != std::string::npos) {
        stackedWidget->addWidget(createProfilePage()); // 0
        stackedWidget->addWidget(createSecretaryCourrierPage()); // 1: GESTION DU COURRIER (3 FLUX)
        stackedWidget->addWidget(createSecretaryRdvPage()); // 2: GESTION DES RENDEZ-VOUS (2 ONGLETS)
        stackedWidget->addWidget(createSecretaryDossiersPage()); // 3: SUIVI DOSSIERS (NAVBAR PAIEMENT, SUPPORT, RAPPORT)
        stackedWidget->addWidget(createValidationInscriptionsPage()); // 4: VALIDATION DES INSCRIPTIONS
        stackedWidget->addWidget(createSecretaryNotesTransferPage()); // 5: RÉSULTATS DES PROFESSEURS
        stackedWidget->addWidget(createSalaryManagementPage()); // 6: GESTION DES SALAIRES
        stackedWidget->addWidget(createSecretaryAnnoncesPage()); // 7: ANNONCES
        stackedWidget->addWidget(createSettingsPage()); // 8: PARAMÈTRES (UNIFIÉ)
    }
    else { // Coordonnateur de faculté
        stackedWidget->addWidget(createProfilePage()); // 0: PROFIL
        stackedWidget->addWidget(createCoordMaFacultePage()); // 1: MA FACULTÉ (ISOLÉ)
        stackedWidget->addWidget(createCoordProfsPage()); // 2: GESTION DES PROFESSEURS (ISOLÉ)
        stackedWidget->addWidget(createCoordProgrammesPage()); // 3: PROGRAMMES & MATIÈRES (ISOLÉ)
        stackedWidget->addWidget(createCoordEmploiDuTempsPage()); // 4: EMPLOI DU TEMPS (ISOLÉ)
        stackedWidget->addWidget(createCoordResultsPage()); // 5: RÉSULTATS DES ÉTUDIANTS (ISOLÉ)
        stackedWidget->addWidget(createAdminEventsPage()); // 6: ÉVÉNEMENTS ADMIN
        stackedWidget->addWidget(createAnnoncesRecuesPage()); // 7: ANNONCES
        stackedWidget->addWidget(createSettingsPage()); // 8: PARAMÈTRES (UNIFIÉ)
    }

    mainLayout->addWidget(stackedWidget, 1);
}

QWidget* AdminDashboardWindow::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(270);
    sidebar->setStyleSheet("background-color: #0b1e36; color: white;");

    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(4);

    QLabel *logoImg = new QLabel();
    QPixmap logoPix(":/resources/logo.png");
    if (!logoPix.isNull()) {
        logoImg->setPixmap(logoPix.scaled(75, 75, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoImg->setAlignment(Qt::AlignCenter);
    logoImg->setStyleSheet("background: transparent; margin-top: 5px;");
    layout->addWidget(logoImg);

    QLabel *logoTitle = new QLabel("UDH");
    logoTitle->setAlignment(Qt::AlignCenter);
    logoTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #d4af37; background: transparent; letter-spacing: 2px;");
    layout->addWidget(logoTitle);

    QLabel *logoSub = new QLabel(QString("ESPACE %1").arg(QString::fromStdString(currentAdmin.getPoste()).toUpper()));
    logoSub->setAlignment(Qt::AlignCenter);
    logoSub->setWordWrap(true);
    logoSub->setStyleSheet("font-size: 10px; font-weight: bold; color: #90a4ae; margin-bottom: 15px; background: transparent; padding: 0 10px;");
    layout->addWidget(logoSub);

    std::string p = currentAdmin.getPoste();
    QStringList menuItems;

    if (p == "Recteur") {
        menuItems = {
            "👤   Profil",
            "🏛️   Vue d'ensemble",
            "👥   Gestion des membres",
            "📜   Validation académique",
            "🏫   Gestion des facultés",
            "💼   Gestion financière",
            "📅   Événements admin",
            "⚙️   Paramètres",
            "🚪   Déconnexion"
        };
    } else if (p == "Président" || p == "President") {
        menuItems = {
            "👤   Profil",
            "👥   Conseil d'administration",
            "💼   Gestion financière",
            "📅   Événements admin",
            "👑   Supervision",
            "📊   Rapport",
            "📢   Annonces",
            "⚙️   Paramètres",
            "🚪   Déconnexion"
        };
    } else if (p.find("Secrétaire") != std::string::npos || p.find("Secretaire") != std::string::npos) {
        menuItems = {
            "👤   Profil",
            "📩   Gestion du courrier",
            "📅   Gestion des rendez-vous",
            "📁   Dossiers administratifs",
            "📝   Validation des inscriptions",
            "📊   Résultats des professeurs",
            "💰   Gestion des salaires",
            "📢   Annonces",
            "⚙️   Paramètres",
            "🚪   Déconnexion"
        };
    } else { // Coordonnateur
        menuItems = {
            "👤   Profil",
            "🏫   Ma faculté",
            "👨‍🏫   Gestion des professeurs",
            "📚   Programmes & Matières",
            "🗓️   Emploi du temps",
            "📊   Résultats des étudiants",
            "📅   Événements admin",
            "📢   Annonces",
            "⚙️   Paramètres",
            "🚪   Déconnexion"
        };
    }

    sidebarBtnGroup = new QButtonGroup(this);
    sidebarBtnGroup->setExclusive(true);

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *btn = new QPushButton(menuItems[i]);
        btn->setFixedHeight(42);
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);

        bool isLogoutItem = (i == menuItems.size() - 1);

        if (isLogoutItem) {
            btn->setStyleSheet(
                "QPushButton { text-align: left; padding-left: 22px; border: none; font-size: 13px; background-color: transparent; color: #ff6b6b; font-weight: bold; margin-top: 15px; }"
                "QPushButton:hover { background-color: #3d232a; color: #ff8787; }"
            );
        } else {
            btn->setStyleSheet(
                "QPushButton { text-align: left; padding-left: 22px; border: none; font-size: 13px; background-color: transparent; color: #b3c2d6; }"
                "QPushButton:hover { background-color: #1a3353; color: white; }"
                "QPushButton:checked { background-color: #1a3353; color: #d4af37; font-weight: bold; border-left: 4px solid #d4af37; }"
            );
        }

        if (i == 0) btn->setChecked(true);

        sidebarBtnGroup->addButton(btn, i);
        layout->addWidget(btn);
    }

    connect(sidebarBtnGroup, &QButtonGroup::idClicked, this, &AdminDashboardWindow::onSidebarBtnClicked);

    layout->addStretch();

    updateAnnoncesBadge();

    return sidebar;
}

void AdminDashboardWindow::updateAnnoncesBadge()
{
    std::string p = currentAdmin.getPoste();
    int annoncesBtnIndex = -1;
    if (p == "Recteur") annoncesBtnIndex = -1; // Recteur n'a plus de section Annonces séparée
    else if (p == "Président" || p == "President") annoncesBtnIndex = 6;
    else if (p.find("Secrétaire") != std::string::npos || p.find("Secretaire") != std::string::npos) annoncesBtnIndex = 7;
    else annoncesBtnIndex = 7; // Coordonnateur

    if (annoncesBtnIndex >= 0 && p.find("Secrétaire") == std::string::npos && p.find("Secretaire") == std::string::npos) {
        int nonLues = compterAnnoncesNonLues("Administrateurs", currentAdmin.getId());
        if (sidebarBtnGroup && sidebarBtnGroup->button(annoncesBtnIndex)) {
            if (nonLues > 0) {
                sidebarBtnGroup->button(annoncesBtnIndex)->setText(QString("📢   Annonces  (%1)").arg(nonLues));
            } else {
                sidebarBtnGroup->button(annoncesBtnIndex)->setText("📢   Annonces");
            }
        }
    }
}

void AdminDashboardWindow::onSidebarBtnClicked(int index)
{
    if (index == stackedWidget->count()) {
        emit logoutRequested();
        return;
    }

    if (index >= 0 && index < stackedWidget->count()) {
        std::string p = currentAdmin.getPoste();
        if (p.find("Secrétaire") != std::string::npos || p.find("Secretaire") != std::string::npos) {
            if (index == 1 && gestionCourrierWidget) {
                gestionCourrierWidget->refreshData();
            } else if (index == 2) {
                if (secCalWidget) secCalWidget->refreshData();
                if (secReunionsWidget) secReunionsWidget->refreshData();
            } else if (index == 3 && dossierNavBarWidget) {
                dossierNavBarWidget->refreshData();
            } else if (index == 4 && validationInscriptionsWidget) {
                validationInscriptionsWidget->refreshData();
            } else if (index == 5) {
                refreshSecretaryNotesTable();
            } else if (index == 6 && gestionSalairesWidget) {
                gestionSalairesWidget->refreshData();
            } else if (index == 7 && gestionAnnoncesWidget) {
                gestionAnnoncesWidget->refreshData();
            }
        } else if (p == "Président" || p == "President") {
            if (index == 1 && presidentConseilWidget) {
                presidentConseilWidget->refreshData();
            } else if (index == 2 && presidentFinanceWidget) {
                presidentFinanceWidget->refreshData();
            } else if (index == 3 && adminEventsWidget) {
                adminEventsWidget->refreshData();
            } else if (index == 4 && supervisionWidget) {
                supervisionWidget->refreshData();
            } else if (index == 5 && rapportGraphiqueWidget) {
                rapportGraphiqueWidget->refreshData();
            } else if (index == 6 && annoncesRecuesWidget) {
                annoncesRecuesWidget->refreshData();
                updateAnnoncesBadge();
            }
        } else if (p == "Recteur") {
            if (index == 1 && recteurVueEnsembleWidget) {
                recteurVueEnsembleWidget->refreshData();
            } else if (index == 2 && recteurGestionMembresWidget) {
                recteurGestionMembresWidget->refreshData();
            } else if (index == 3 && recteurValidationWidget) {
                recteurValidationWidget->refreshData();
            } else if (index == 4 && recteurGestionFacultesWidget) {
                recteurGestionFacultesWidget->refreshData();
            } else if (index == 5 && recteurFinanceWidget) {
                recteurFinanceWidget->refreshData();
            } else if (index == 6 && adminEventsWidget) {
                adminEventsWidget->refreshData();
            }
        } else { // Coordonnateur
            if (index == 1) {
                refreshCoordMaFaculte();
            } else if (index == 2) {
                refreshCoordProfs();
            } else if (index == 3) {
                refreshCoordProgrammes();
            } else if (index == 4) {
                refreshCoordEmploiDuTemps();
            } else if (index == 5) {
                refreshCoordResultsTable();
            } else if (index == 6 && adminEventsWidget) {
                adminEventsWidget->refreshData();
            } else if (index == 7 && annoncesRecuesWidget) {
                annoncesRecuesWidget->refreshData();
                updateAnnoncesBadge();
            }
        }

        stackedWidget->setCurrentIndex(index);
    }
}

QWidget* AdminDashboardWindow::createProfilePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 14px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(35, 30, 35, 30);
    cLayout->setSpacing(16);

    QLabel *title = new QLabel("Profil Administrateur", card);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; border: none;");

    auto makeRow = [](const QString &k, const QString &v) {
        QHBoxLayout *h = new QHBoxLayout();
        QLabel *kl = new QLabel(k + " :"); kl->setFixedWidth(160);
        kl->setStyleSheet("font-weight: bold; color: #4a5568; font-size: 14px; border: none;");
        QLabel *vl = new QLabel(v);
        vl->setStyleSheet("color: #1a202c; font-size: 14px; border: none;");
        h->addWidget(kl); h->addWidget(vl, 1);
        return h;
    };

    cLayout->addWidget(title);
    cLayout->addSpacing(10);
    cLayout->addLayout(makeRow("Identifiant", QString::fromStdString(currentAdmin.getId())));
    cLayout->addLayout(makeRow("Nom complet", QString("%1 %2").arg(QString::fromStdString(currentAdmin.getNom()), QString::fromStdString(currentAdmin.getPrenom()))));
    cLayout->addLayout(makeRow("Poste", QString::fromStdString(currentAdmin.getPoste())));
    if (!currentAdmin.getFaculte().empty()) {
        cLayout->addLayout(makeRow("Faculté assignée", QString::fromStdString(currentAdmin.getFaculte())));
    }
    cLayout->addLayout(makeRow("Email professionnel", QString::fromStdString(currentAdmin.getEmail())));
    cLayout->addLayout(makeRow("Téléphone", QString::fromStdString(currentAdmin.getTelephone())));
    cLayout->addLayout(makeRow("Contact d'urgence", QString("%1 %2 (%3)")
                               .arg(QString::fromStdString(currentAdmin.getNomProche()),
                                    QString::fromStdString(currentAdmin.getPrenomProche()),
                                    QString::fromStdString(currentAdmin.getTelProche()))));

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* AdminDashboardWindow::createSecretaryCourrierPage()
{
    gestionCourrierWidget = new GestionCourrier(profs, admins, this);
    return gestionCourrierWidget;
}

QWidget* AdminDashboardWindow::createSecretaryRdvPage()
{
    secretaryRdvWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(secretaryRdvWidget);
    layout->setContentsMargins(15, 10, 15, 15);
    layout->setSpacing(10);

    QTabWidget *tabWidget = new QTabWidget(secretaryRdvWidget);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; background: #ffffff; border-radius: 10px; top: -1px; }"
        "QTabBar::tab { background: #edf2f7; color: #4a5568; padding: 10px 24px; font-weight: bold; font-size: 13px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0b1e36; border-bottom: 3px solid #0b1e36; }"
        "QTabBar::tab:hover:!selected { background: #e2e8f0; }"
    );

    secCalWidget = new CalendrierScolaire(true, tabWidget);
    secReunionsWidget = new GestionReunions(tabWidget);

    tabWidget->addTab(secCalWidget, "🗓️ Onglet A — Calendrier Scolaire");
    tabWidget->addTab(secReunionsWidget, "👥 Onglet B — Réunions avec le Comité");

    layout->addWidget(tabWidget);

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int idx) {
        if (idx == 0 && secCalWidget) secCalWidget->refreshData();
        else if (idx == 1 && secReunionsWidget) secReunionsWidget->refreshData();
    });

    return secretaryRdvWidget;
}

QWidget* AdminDashboardWindow::createSecretaryDossiersPage()
{
    dossierNavBarWidget = new DossierAdministratifNavBar(comptes, profs, this);
    return dossierNavBarWidget;
}

QWidget* AdminDashboardWindow::createPresidentConseilPage()
{
    presidentConseilWidget = new CourriersPresidentView(this);
    return presidentConseilWidget;
}

QWidget* AdminDashboardWindow::createPresidentFinancePage()
{
    presidentFinanceWidget = new CourriersRecteurView(this);
    return presidentFinanceWidget;
}

QWidget* AdminDashboardWindow::createRecteurVueEnsemblePage()
{
    recteurVueEnsembleWidget = new ModuleVueEnsemble(admins, profs, comptes, this);
    return recteurVueEnsembleWidget;
}

QWidget* AdminDashboardWindow::createRecteurGestionMembresPage()
{
    recteurGestionMembresWidget = new ModuleGestionMembres(admins, profs, this);
    return recteurGestionMembresWidget;
}

QWidget* AdminDashboardWindow::createRecteurValidationAcademiquePage()
{
    recteurValidationWidget = new ModuleValidationAcademique(this);
    return recteurValidationWidget;
}

QWidget* AdminDashboardWindow::createRecteurGestionFacultesPage()
{
    recteurGestionFacultesWidget = new ModuleGestionFacultes(admins, profs, comptes, this);
    return recteurGestionFacultesWidget;
}

QWidget* AdminDashboardWindow::createRecteurFinancePage()
{
    recteurFinanceWidget = new CourriersRecteurView(this);
    return recteurFinanceWidget;
}

QWidget* AdminDashboardWindow::createAdminEventsPage()
{
    adminEventsWidget = new EvenementsAdminWidget(currentAdmin.getPoste(), currentAdmin.getFaculte(), this);
    return adminEventsWidget;
}

QWidget* AdminDashboardWindow::createSupervisionPage()
{
    supervisionWidget = new ModuleSupervision(this);
    return supervisionWidget;
}

QWidget* AdminDashboardWindow::createRapportGraphiquePage()
{
    rapportGraphiqueWidget = new ModuleRapportGraphique(this);
    return rapportGraphiqueWidget;
}

QWidget* AdminDashboardWindow::createSalaryManagementPage()
{
    gestionSalairesWidget = new GestionSalaires(profs, admins, this);
    return gestionSalairesWidget;
}

QWidget* AdminDashboardWindow::createValidationInscriptionsPage()
{
    validationInscriptionsWidget = new ValidationInscriptions(comptes, this);
    return validationInscriptionsWidget;
}

QWidget* AdminDashboardWindow::createSecretaryNotesTransferPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(20);

    // 3 Stat Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    cardSecNotesPending     = new StatCard("⏳", "0", "Notes en attente de transfert", "#E67E22", page);
    cardSecNotesTransferred = new StatCard("✅", "0", "Notes transférées aux étudiants", "#27AE60", page);
    cardSecNotesTotal       = new StatCard("📋", "0", "Total des notes reçues", "#2980B9", page);

    statsLayout->addWidget(cardSecNotesPending);
    statsLayout->addWidget(cardSecNotesTransferred);
    statsLayout->addWidget(cardSecNotesTotal);
    mainLayout->addLayout(statsLayout);

    // Card Frame for Table
    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(15);

    // Header Controls Row
    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *title = new QLabel("Transmission des Notes des Professeurs", tableCard);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #1a202c; border: none;");

    secNotesFilterCombo = new QComboBox(tableCard);
    secNotesFilterCombo->setFixedWidth(180);
    secNotesFilterCombo->setFixedHeight(38);
    secNotesFilterCombo->setStyleSheet(
        "QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding-left: 10px; font-weight: bold; font-size: 13px; }"
        "QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
    );
    secNotesFilterCombo->addItems({"Toutes", "En attente (Reçue)", "Transférées"});
    connect(secNotesFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminDashboardWindow::refreshSecretaryNotesTable);

    QPushButton *transferAllBtn = new QPushButton("⚡ Transférer toutes les notes en attente", tableCard);
    transferAllBtn->setFixedHeight(38);
    transferAllBtn->setCursor(Qt::PointingHandCursor);
    transferAllBtn->setStyleSheet(
        "QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 14px; border: none; outline: none; }"
        "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
    );
    connect(transferAllBtn, &QPushButton::clicked, this, [this]() {
        std::vector<Professeur> currentProfs = chargerProfesseurs();
        int count = 0;
        for (auto &p : currentProfs) {
            auto notes = p.getNotes();
            for (auto &n : notes) {
                if (n.statut != "Transférée") {
                    n.statut = "Transférée";
                    count++;
                }
            }
            p.setNotes(notes);
        }
        if (count > 0) {
            sauvegarderProfesseurs(currentProfs);
            profs = currentProfs;
            QMessageBox::information(this, "Succès", QString("%1 note(s) transférée(s) avec succès aux étudiants.").arg(count));
            refreshSecretaryNotesTable();
        } else {
            QMessageBox::information(this, "Information", "Aucune note en attente de transfert.");
        }
    });

    headerRow->addWidget(title);
    headerRow->addStretch();
    headerRow->addWidget(new QLabel("Filtre :", tableCard));
    headerRow->addWidget(secNotesFilterCombo);
    headerRow->addWidget(transferAllBtn);
    tcLayout->addLayout(headerRow);

    secNotesTable = new QTableWidget(tableCard);
    secNotesTable->setColumnCount(7);
    secNotesTable->setHorizontalHeaderLabels({"Professeur", "Faculté", "ID Étudiant", "Matière", "Note", "Statut", "Action"});
    secNotesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    secNotesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    secNotesTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    secNotesTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    secNotesTable->verticalHeader()->setVisible(false);
    secNotesTable->verticalHeader()->setDefaultSectionSize(38);
    secNotesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    secNotesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    secNotesTable->setAlternatingRowColors(true);
    secNotesTable->setShowGrid(false);
    secNotesTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );

    tcLayout->addWidget(secNotesTable);
    mainLayout->addWidget(tableCard);

    refreshSecretaryNotesTable();
    return page;
}

void AdminDashboardWindow::refreshSecretaryNotesTable()
{
    if (!secNotesTable) return;

    std::vector<Professeur> freshProfs = chargerProfesseurs();
    profs = freshProfs;

    int pending = 0;
    int transferred = 0;
    int total = 0;

    struct TableRow {
        QString profName;
        QString faculte;
        QString studentId;
        QString matiere;
        double note;
        QString statut;
        std::string profId;
        size_t noteIndex;
    };

    std::vector<TableRow> rows;
    QString filter = secNotesFilterCombo ? secNotesFilterCombo->currentText() : "Toutes";

    for (const auto &p : profs) {
        auto notes = p.getNotes();
        for (size_t i = 0; i < notes.size(); ++i) {
            const auto &n = notes[i];
            total++;
            bool isTransferred = (n.statut == "Transférée");
            if (isTransferred) transferred++;
            else pending++;

            if (filter == "En attente (Reçue)" && isTransferred) continue;
            if (filter == "Transférées" && !isTransferred) continue;

            TableRow r;
            r.profName = QString("%1 %2").arg(QString::fromStdString(p.getNom()), QString::fromStdString(p.getPrenom()));
            r.faculte = QString::fromStdString(p.getFaculte());
            r.studentId = QString::fromStdString(n.idEtudiant);
            r.matiere = QString::fromStdString(n.matiere);
            r.note = n.valeur;
            r.statut = QString::fromStdString(n.statut.empty() ? "Reçue" : n.statut);
            r.profId = p.getId();
            r.noteIndex = i;
            rows.push_back(r);
        }
    }

    secNotesTable->setRowCount((int)rows.size());

    for (int r = 0; r < (int)rows.size(); ++r) {
        const auto &data = rows[r];

        secNotesTable->setItem(r, 0, new QTableWidgetItem(data.profName));
        secNotesTable->setItem(r, 1, new QTableWidgetItem(data.faculte));
        secNotesTable->setItem(r, 2, new QTableWidgetItem(data.studentId));
        secNotesTable->setItem(r, 3, new QTableWidgetItem(data.matiere));

        auto *noteItem = new QTableWidgetItem(QString::number(data.note, 'f', 1) + " / 100");
        noteItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        noteItem->setForeground(QColor(data.note >= 70 ? "#27ae60" : (data.note >= 50 ? "#f39c12" : "#e74c3c")));
        secNotesTable->setItem(r, 4, noteItem);

        // Badge Statut
        QWidget *badgeWidget = new QWidget();
        QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
        bLayout->setContentsMargins(4, 2, 4, 2);
        bLayout->setAlignment(Qt::AlignCenter);

        QLabel *badge = new QLabel(data.statut);
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedWidth(110);
        badge->setFixedHeight(24);

        if (data.statut == "Transférée") {
            badge->setStyleSheet("background-color: #DEF7EC; color: #03543F; font-weight: bold; border-radius: 12px; font-size: 11px; border: 1px solid #BCF0DA;");
        } else {
            badge->setStyleSheet("background-color: #FEF3C7; color: #92400E; font-weight: bold; border-radius: 12px; font-size: 11px; border: 1px solid #FDE68A;");
        }
        bLayout->addWidget(badge);
        secNotesTable->setCellWidget(r, 5, badgeWidget);

        // Action Button
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *aLayout = new QHBoxLayout(actionWidget);
        aLayout->setContentsMargins(4, 2, 4, 2);
        aLayout->setAlignment(Qt::AlignCenter);

        if (data.statut != "Transférée") {
            QPushButton *btn = new QPushButton("Transférer");
            btn->setCursor(Qt::PointingHandCursor);
            btn->setStyleSheet(
                "QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 11px; border: none; outline: none; }"
                "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
            );

            std::string pId = data.profId;
            size_t nIdx = data.noteIndex;

            connect(btn, &QPushButton::clicked, this, [this, pId, nIdx]() {
                std::vector<Professeur> curProfs = chargerProfesseurs();
                for (auto &p : curProfs) {
                    if (p.getId() == pId) {
                        auto notes = p.getNotes();
                        if (nIdx < notes.size()) {
                            notes[nIdx].statut = "Transférée";
                            p.setNotes(notes);
                            sauvegarderProfesseurs(curProfs);
                            profs = curProfs;
                            refreshSecretaryNotesTable();
                            break;
                        }
                    }
                }
            });
            aLayout->addWidget(btn);
        } else {
            QLabel *lblDone = new QLabel("Transférée");
            lblDone->setStyleSheet("color: #718096; font-size: 11px; font-style: italic;");
            aLayout->addWidget(lblDone);
        }

        secNotesTable->setCellWidget(r, 6, actionWidget);
    }

    if (cardSecNotesPending) cardSecNotesPending->updateValue(QString::number(pending));
    if (cardSecNotesTransferred) cardSecNotesTransferred->updateValue(QString::number(transferred));
    if (cardSecNotesTotal) cardSecNotesTotal->updateValue(QString::number(total));
}

// =========================================================================
//  MODULES COORDONNATEUR DE FACULTÉ (ISOLATION DES DONNÉES PAR FACULTÉ)
// =========================================================================

// 1. MA FACULTÉ
QWidget* AdminDashboardWindow::createCoordMaFacultePage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    // Header card
    QFrame *headerCard = new QFrame(page);
    headerCard->setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0b1e36, stop:1 #1a3a60); border-radius: 12px; }");
    QVBoxLayout *hcLayout = new QVBoxLayout(headerCard);
    hcLayout->setContentsMargins(20, 16, 20, 16);
    hcLayout->setSpacing(4);

    QLabel *hTitle = new QLabel(QString("🏫   Espace Coordonnateur — %1").arg(m_faculteAssignee.isEmpty() ? "Faculté non assignée" : m_faculteAssignee), headerCard);
    hTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff; background: transparent;");
    QLabel *hSub = new QLabel(QString("Coordonnateur : %1 %2 | Isolation des données active pour cette faculté.")
                              .arg(QString::fromStdString(currentAdmin.getPrenom()), QString::fromStdString(currentAdmin.getNom())), headerCard);
    hSub->setStyleSheet("font-size: 12px; color: #90caf9; background: transparent;");
    hcLayout->addWidget(hTitle);
    hcLayout->addWidget(hSub);
    mainLayout->addWidget(headerCard);

    // 4 Stat Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(15);
    cardCoordFacProfs    = new StatCard("👨‍🏫", "0", "Enseignants affectés", "#2980B9", page);
    cardCoordFacStudents = new StatCard("🎓", "0", "Étudiants inscrits", "#27AE60", page);
    cardCoordFacYears    = new StatCard("📅", "0 ans", "Durée du cursus", "#8E44AD", page);
    cardCoordFacCourses  = new StatCard("📚", "0", "Matières au programme", "#D35400", page);
    statsLayout->addWidget(cardCoordFacProfs);
    statsLayout->addWidget(cardCoordFacStudents);
    statsLayout->addWidget(cardCoordFacYears);
    statsLayout->addWidget(cardCoordFacCourses);
    mainLayout->addLayout(statsLayout);

    // Card with Student table
    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(20, 18, 20, 20);
    tcLayout->setSpacing(12);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel("Étudiants inscrits dans ma faculté", tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    topRow->addWidget(tTitle);
    topRow->addStretch();

    coordFacStudentSearch = new QLineEdit(tableCard);
    coordFacStudentSearch->setPlaceholderText("🔍 Rechercher un étudiant...");
    coordFacStudentSearch->setFixedWidth(260);
    coordFacStudentSearch->setStyleSheet("QLineEdit { background: #f8fafc; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 12px; color: #2d3748; }");
    connect(coordFacStudentSearch, &QLineEdit::textChanged, this, &AdminDashboardWindow::refreshCoordMaFaculte);
    topRow->addWidget(coordFacStudentSearch);
    tcLayout->addLayout(topRow);

    coordFacStudentsTable = new QTableWidget(tableCard);
    coordFacStudentsTable->setColumnCount(6);
    coordFacStudentsTable->setHorizontalHeaderLabels({"ID Étudiant", "Nom & Prénom", "Email", "Téléphone", "Statut Inscription", "Statut Compte"});
    coordFacStudentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordFacStudentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    coordFacStudentsTable->verticalHeader()->setVisible(false);
    coordFacStudentsTable->verticalHeader()->setDefaultSectionSize(36);
    coordFacStudentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coordFacStudentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coordFacStudentsTable->setAlternatingRowColors(true);
    coordFacStudentsTable->setShowGrid(false);
    coordFacStudentsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );
    tcLayout->addWidget(coordFacStudentsTable);
    mainLayout->addWidget(tableCard, 1);

    refreshCoordMaFaculte();
    return page;
}

void AdminDashboardWindow::refreshCoordMaFaculte()
{
    if (!coordFacStudentsTable) return;

    if (m_faculteAssignee.isEmpty()) {
        coordFacStudentsTable->setRowCount(0);
        return;
    }

    std::vector<CompteEtudiant> allComptes = chargerComptes();
    comptes = allComptes;
    std::vector<CompteEtudiant> myStudents = IsolationFaculteUtils::filtrerComptesParFaculte(comptes, m_faculteAssignee);

    std::vector<Professeur> allProfs = chargerProfesseurs();
    profs = allProfs;
    std::vector<Professeur> myProfs = IsolationFaculteUtils::filtrerProfsParFaculte(profs, m_faculteAssignee);

    ProgrammeEtudes prog = IsolationFaculteUtils::filtrerProgrammeParFaculte(construireProgrammes(), m_faculteAssignee);

    int totalCourses = 0;
    for (const auto &an : prog.annees) {
        totalCourses += (int)an.matieres.size();
    }

    if (cardCoordFacProfs) cardCoordFacProfs->updateValue(QString::number(myProfs.size()));
    if (cardCoordFacStudents) cardCoordFacStudents->updateValue(QString::number(myStudents.size()));
    if (cardCoordFacYears) cardCoordFacYears->updateValue(QString("%1 ans").arg(prog.annees.size()));
    if (cardCoordFacCourses) cardCoordFacCourses->updateValue(QString::number(totalCourses));

    QString search = coordFacStudentSearch ? coordFacStudentSearch->text().trimmed().toLower() : "";

    std::vector<CompteEtudiant> filtered;
    for (const auto &c : myStudents) {
        QString sId = QString::fromStdString(c.getId()).toLower();
        QString sNom = QString::fromStdString(c.getNom() + " " + c.getPrenom()).toLower();
        if (search.isEmpty() || sId.contains(search) || sNom.contains(search)) {
            filtered.push_back(c);
        }
    }

    coordFacStudentsTable->setRowCount((int)filtered.size());
    for (int r = 0; r < (int)filtered.size(); ++r) {
        const auto &c = filtered[r];
        coordFacStudentsTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(c.getId())));
        coordFacStudentsTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(c.getNom() + " " + c.getPrenom())));
        coordFacStudentsTable->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(c.getEmail())));
        coordFacStudentsTable->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(c.getTelephone())));

        QString statutInsc = c.estInscrit() ? "Inscrit / Validé" : "En attente";
        auto *inscItem = new QTableWidgetItem(statutInsc);
        inscItem->setForeground(QColor(c.estInscrit() ? "#27ae60" : "#e67e22"));
        coordFacStudentsTable->setItem(r, 4, inscItem);

        QString statutCpt = c.estEnPause() ? "En pause" : (c.estActif() ? "Actif" : "Inactif");
        auto *cptItem = new QTableWidgetItem(statutCpt);
        cptItem->setForeground(QColor(c.estActif() && !c.estEnPause() ? "#27ae60" : "#e74c3c"));
        coordFacStudentsTable->setItem(r, 5, cptItem);
    }
}

// 2. GESTION DES PROFESSEURS
QWidget* AdminDashboardWindow::createCoordProfsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    // 3 Stat Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(15);
    cardCoordProfsTotal   = new StatCard("👨‍🏫", "0", "Enseignants de la faculté", "#2980B9", page);
    cardCoordProfsPaid    = new StatCard("💰", "0", "Rémunérés ce mois", "#27AE60", page);
    cardCoordProfsCourses = new StatCard("📖", "0", "Matières dispensées", "#8E44AD", page);
    statsLayout->addWidget(cardCoordProfsTotal);
    statsLayout->addWidget(cardCoordProfsPaid);
    statsLayout->addWidget(cardCoordProfsCourses);
    mainLayout->addLayout(statsLayout);

    // Table Card
    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(20, 18, 20, 20);
    tcLayout->setSpacing(12);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel(QString("Corps Enseignant — %1").arg(m_faculteAssignee), tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    topRow->addWidget(tTitle);
    topRow->addStretch();

    coordProfsSearch = new QLineEdit(tableCard);
    coordProfsSearch->setPlaceholderText("🔍 Filtrer par nom ou matière...");
    coordProfsSearch->setFixedWidth(260);
    coordProfsSearch->setStyleSheet("QLineEdit { background: #f8fafc; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 12px; color: #2d3748; }");
    connect(coordProfsSearch, &QLineEdit::textChanged, this, &AdminDashboardWindow::refreshCoordProfs);
    topRow->addWidget(coordProfsSearch);
    tcLayout->addLayout(topRow);

    coordProfsTable = new QTableWidget(tableCard);
    coordProfsTable->setColumnCount(7);
    coordProfsTable->setHorizontalHeaderLabels({"ID Professeur", "Nom & Prénom", "Email", "Téléphone", "Matières Enseignées", "Statut Compte", "Rémunération"});
    coordProfsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordProfsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    coordProfsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    coordProfsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    coordProfsTable->verticalHeader()->setVisible(false);
    coordProfsTable->verticalHeader()->setDefaultSectionSize(38);
    coordProfsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coordProfsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coordProfsTable->setAlternatingRowColors(true);
    coordProfsTable->setShowGrid(false);
    coordProfsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );
    tcLayout->addWidget(coordProfsTable);
    mainLayout->addWidget(tableCard, 1);

    refreshCoordProfs();
    return page;
}

void AdminDashboardWindow::refreshCoordProfs()
{
    if (!coordProfsTable) return;

    if (m_faculteAssignee.isEmpty()) {
        coordProfsTable->setRowCount(0);
        return;
    }

    std::vector<Professeur> allProfs = chargerProfesseurs();
    profs = allProfs;
    std::vector<Professeur> myProfs = IsolationFaculteUtils::filtrerProfsParFaculte(profs, m_faculteAssignee);

    int paidCount = 0;
    std::set<std::string> uniqueMatieres;
    for (const auto &p : myProfs) {
        if (p.getEstPaye()) paidCount++;
        for (const auto &m : p.getMatieres()) {
            uniqueMatieres.insert(m);
        }
    }

    if (cardCoordProfsTotal) cardCoordProfsTotal->updateValue(QString::number(myProfs.size()));
    if (cardCoordProfsPaid) cardCoordProfsPaid->updateValue(QString::number(paidCount));
    if (cardCoordProfsCourses) cardCoordProfsCourses->updateValue(QString::number(uniqueMatieres.size()));

    QString search = coordProfsSearch ? coordProfsSearch->text().trimmed().toLower() : "";

    std::vector<Professeur> filtered;
    for (const auto &p : myProfs) {
        QString pId = QString::fromStdString(p.getId()).toLower();
        QString pName = QString::fromStdString(p.getNom() + " " + p.getPrenom()).toLower();
        QString pMats = "";
        for (const auto &m : p.getMatieres()) pMats += QString::fromStdString(m) + " ";
        pMats = pMats.toLower();

        if (search.isEmpty() || pId.contains(search) || pName.contains(search) || pMats.contains(search)) {
            filtered.push_back(p);
        }
    }

    coordProfsTable->setRowCount((int)filtered.size());
    for (int r = 0; r < (int)filtered.size(); ++r) {
        const auto &p = filtered[r];
        coordProfsTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(p.getId())));
        coordProfsTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(p.getNom() + " " + p.getPrenom())));
        coordProfsTable->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(p.getEmail())));
        coordProfsTable->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(p.getTelephone())));

        QString mats = "";
        for (size_t i = 0; i < p.getMatieres().size(); ++i) {
            if (i > 0) mats += ", ";
            mats += QString::fromStdString(p.getMatieres()[i]);
        }
        coordProfsTable->setItem(r, 4, new QTableWidgetItem(mats.isEmpty() ? "Aucune" : mats));

        auto *stItem = new QTableWidgetItem(QString::fromStdString(p.getStatutCompte()));
        stItem->setForeground(QColor(p.getStatutCompte() == "Actif" ? "#27ae60" : "#e67e22"));
        coordProfsTable->setItem(r, 5, stItem);

        QString remStr = p.getEstPaye() ? QString("Payé (%1 HTG)").arg(p.getMontantSalaire(), 0, 'f', 0) : "Non payé";
        auto *remItem = new QTableWidgetItem(remStr);
        remItem->setForeground(QColor(p.getEstPaye() ? "#27ae60" : "#e74c3c"));
        coordProfsTable->setItem(r, 6, remItem);
    }
}

// 3. PROGRAMMES & MATIÈRES
QWidget* AdminDashboardWindow::createCoordProgrammesPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(14);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel(QString("Programme & Maquette Pédagogique — %1").arg(m_faculteAssignee), tableCard);
    tTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #1a202c; border: none;");
    topRow->addWidget(tTitle);
    topRow->addStretch();

    QLabel *selLbl = new QLabel("Niveau / Année :", tableCard);
    selLbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #4a5568; border: none;");
    topRow->addWidget(selLbl);

    coordProgramYearCombo = new QComboBox(tableCard);
    coordProgramYearCombo->setFixedWidth(150);
    coordProgramYearCombo->setStyleSheet("QComboBox { background: #f8fafc; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 12px; font-weight: bold; color: #2d3748; }");
    connect(coordProgramYearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminDashboardWindow::refreshCoordProgrammes);
    topRow->addWidget(coordProgramYearCombo);
    tcLayout->addLayout(topRow);

    coordProgramSummaryLbl = new QLabel(tableCard);
    coordProgramSummaryLbl->setStyleSheet("font-size: 13px; color: #4a5568; background: #ebf8ff; padding: 8px 12px; border-radius: 6px; border: 1px solid #bee3f8;");
    tcLayout->addWidget(coordProgramSummaryLbl);

    coordProgramTable = new QTableWidget(tableCard);
    coordProgramTable->setColumnCount(4);
    coordProgramTable->setHorizontalHeaderLabels({"N°", "Intitulé de la Matière / Cours", "Enseignant(s) Référent(s) (Faculté)", "Statut / Charge"});
    coordProgramTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordProgramTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    coordProgramTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    coordProgramTable->verticalHeader()->setVisible(false);
    coordProgramTable->verticalHeader()->setDefaultSectionSize(40);
    coordProgramTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coordProgramTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coordProgramTable->setAlternatingRowColors(true);
    coordProgramTable->setShowGrid(false);
    coordProgramTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );
    tcLayout->addWidget(coordProgramTable);
    mainLayout->addWidget(tableCard, 1);

    // Initial load of years into combo
    ProgrammeEtudes prog = IsolationFaculteUtils::filtrerProgrammeParFaculte(construireProgrammes(), m_faculteAssignee);
    coordProgramYearCombo->blockSignals(true);
    coordProgramYearCombo->clear();
    for (const auto &an : prog.annees) {
        coordProgramYearCombo->addItem(QString("Année %1").arg(an.numero), an.numero);
    }
    coordProgramYearCombo->blockSignals(false);

    refreshCoordProgrammes();
    return page;
}

void AdminDashboardWindow::refreshCoordProgrammes()
{
    if (!coordProgramTable) return;

    if (m_faculteAssignee.isEmpty()) {
        coordProgramTable->setRowCount(0);
        return;
    }

    ProgrammeEtudes prog = IsolationFaculteUtils::filtrerProgrammeParFaculte(construireProgrammes(), m_faculteAssignee);
    std::vector<Professeur> myProfs = IsolationFaculteUtils::filtrerProfsParFaculte(chargerProfesseurs(), m_faculteAssignee);

    int totalCourses = 0;
    for (const auto &an : prog.annees) totalCourses += (int)an.matieres.size();

    int selectedYear = coordProgramYearCombo ? coordProgramYearCombo->currentData().toInt() : 1;
    if (selectedYear <= 0) selectedYear = 1;

    if (coordProgramSummaryLbl) {
        coordProgramSummaryLbl->setText(QString("📋 Maquette pédagogique : Programme sur %1 années • %2 matières au total dans le cursus.")
                                        .arg(prog.annees.size()).arg(totalCourses));
    }

    const Annee *anPtr = nullptr;
    for (const auto &an : prog.annees) {
        if (an.numero == selectedYear) {
            anPtr = &an;
            break;
        }
    }

    if (!anPtr) {
        coordProgramTable->setRowCount(0);
        return;
    }

    coordProgramTable->setRowCount((int)anPtr->matieres.size());
    for (int r = 0; r < (int)anPtr->matieres.size(); ++r) {
        QString mat = QString::fromStdString(anPtr->matieres[r]);
        coordProgramTable->setItem(r, 0, new QTableWidgetItem(QString::number(r + 1)));

        auto *nameItem = new QTableWidgetItem(mat);
        nameItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        coordProgramTable->setItem(r, 1, nameItem);

        // Find teachers of this faculty teaching this course
        QStringList teachers;
        for (const auto &p : myProfs) {
            for (const auto &m : p.getMatieres()) {
                if (QString::fromStdString(m).trimmed().compare(mat.trimmed(), Qt::CaseInsensitive) == 0) {
                    teachers.append(QString("%1 %2").arg(QString::fromStdString(p.getNom()), QString::fromStdString(p.getPrenom())));
                    break;
                }
            }
        }

        QString profStr = teachers.isEmpty() ? "Enseignant à assigner" : teachers.join(", ");
        auto *profItem = new QTableWidgetItem(profStr);
        if (teachers.isEmpty()) profItem->setForeground(QColor("#a0aec0"));
        else profItem->setForeground(QColor("#2b6cb0"));
        coordProgramTable->setItem(r, 2, profItem);

        coordProgramTable->setItem(r, 3, new QTableWidgetItem(QString("Semestre %1 / 60h").arg((r % 2) + 1)));
    }
}

// 4. EMPLOI DU TEMPS
QWidget* AdminDashboardWindow::createCoordEmploiDuTempsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(14);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel(QString("Emploi du Temps Hebdomadaire — %1").arg(m_faculteAssignee), tableCard);
    tTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #1a202c; border: none;");
    topRow->addWidget(tTitle);
    topRow->addStretch();

    QLabel *selLbl = new QLabel("Niveau / Promotion :", tableCard);
    selLbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #4a5568; border: none;");
    topRow->addWidget(selLbl);

    coordEdtYearCombo = new QComboBox(tableCard);
    coordEdtYearCombo->setFixedWidth(150);
    coordEdtYearCombo->setStyleSheet("QComboBox { background: #f8fafc; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 12px; font-weight: bold; color: #2d3748; }");
    connect(coordEdtYearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminDashboardWindow::refreshCoordEmploiDuTemps);
    topRow->addWidget(coordEdtYearCombo);
    tcLayout->addLayout(topRow);

    coordEdtTable = new QTableWidget(3, 5, tableCard);
    coordEdtTable->setHorizontalHeaderLabels({"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"});
    coordEdtTable->setVerticalHeaderLabels({"08h00 - 10h00", "10h15 - 12h15", "13h30 - 15h30"});
    coordEdtTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordEdtTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordEdtTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coordEdtTable->setSelectionMode(QAbstractItemView::NoSelection);
    coordEdtTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; }"
        "QHeaderView::section { background-color: #0b1e36; color: #ffffff; font-weight: bold; padding: 10px; border: 1px solid #1a3353; font-size: 12px; }"
        "QTableWidget::item { padding: 10px; border: 1px solid #e2e8f0; }"
    );
    tcLayout->addWidget(coordEdtTable);
    mainLayout->addWidget(tableCard, 1);

    // Initial load of years
    ProgrammeEtudes prog = IsolationFaculteUtils::filtrerProgrammeParFaculte(construireProgrammes(), m_faculteAssignee);
    coordEdtYearCombo->blockSignals(true);
    coordEdtYearCombo->clear();
    for (const auto &an : prog.annees) {
        coordEdtYearCombo->addItem(QString("Année %1").arg(an.numero), an.numero);
    }
    coordEdtYearCombo->blockSignals(false);

    refreshCoordEmploiDuTemps();
    return page;
}

void AdminDashboardWindow::refreshCoordEmploiDuTemps()
{
    if (!coordEdtTable) return;

    if (m_faculteAssignee.isEmpty()) {
        return;
    }

    ProgrammeEtudes prog = IsolationFaculteUtils::filtrerProgrammeParFaculte(construireProgrammes(), m_faculteAssignee);
    std::vector<Professeur> myProfs = IsolationFaculteUtils::filtrerProfsParFaculte(chargerProfesseurs(), m_faculteAssignee);

    int selectedYear = coordEdtYearCombo ? coordEdtYearCombo->currentData().toInt() : 1;
    if (selectedYear <= 0) selectedYear = 1;

    const Annee *anPtr = nullptr;
    for (const auto &an : prog.annees) {
        if (an.numero == selectedYear) {
            anPtr = &an;
            break;
        }
    }

    if (!anPtr || anPtr->matieres.empty()) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 5; ++c) {
                coordEdtTable->setItem(r, c, new QTableWidgetItem("—"));
            }
        }
        return;
    }

    const auto &matieres = anPtr->matieres;
    int idx = 0;

    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row < 3; ++row) {
            QString mat = QString::fromStdString(matieres[idx % matieres.size()]);
            idx++;

            // Find teacher for this course in my faculty
            QString teacherName = "Enseignant référent";
            for (const auto &p : myProfs) {
                for (const auto &m : p.getMatieres()) {
                    if (QString::fromStdString(m).trimmed().compare(mat.trimmed(), Qt::CaseInsensitive) == 0) {
                        teacherName = QString("Pr. %1").arg(QString::fromStdString(p.getNom()));
                        break;
                    }
                }
            }

            QString cellText = QString("%1\n(%2)").arg(mat, teacherName);
            auto *item = new QTableWidgetItem(cellText);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(QFont("Segoe UI", 9, QFont::Bold));
            item->setForeground(QColor("#1a365d"));
            coordEdtTable->setItem(row, col, item);
        }
    }
}

// 5. RÉSULTATS DES ÉTUDIANTS (STRICTEMENT ISOLÉ)
QWidget* AdminDashboardWindow::createCoordResultsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    // 3 Stat Cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(15);

    cardCoordNotesCount     = new StatCard("📊", "0", "Notes enregistrées (Ma faculté)", "#2980B9", page);
    cardCoordNotesAverage   = new StatCard("📈", "0 / 100", "Moyenne générale de la faculté", "#27AE60", page);
    cardCoordStudentsCount  = new StatCard("👥", "0", "Étudiants évalués", "#8E44AD", page);

    statsLayout->addWidget(cardCoordNotesCount);
    statsLayout->addWidget(cardCoordNotesAverage);
    statsLayout->addWidget(cardCoordStudentsCount);
    mainLayout->addLayout(statsLayout);

    // Card Frame for Table
    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(20, 18, 20, 20);
    tcLayout->setSpacing(12);

    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *title = new QLabel(QString("Résultats Académiques — %1").arg(m_faculteAssignee), tableCard);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    topRow->addWidget(title);
    topRow->addStretch();

    coordResultsSearch = new QLineEdit(tableCard);
    coordResultsSearch->setPlaceholderText("🔍 Filtrer par étudiant, matière ou enseignant...");
    coordResultsSearch->setFixedWidth(280);
    coordResultsSearch->setStyleSheet("QLineEdit { background: #f8fafc; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 12px; color: #2d3748; }");
    connect(coordResultsSearch, &QLineEdit::textChanged, this, &AdminDashboardWindow::refreshCoordResultsTable);
    topRow->addWidget(coordResultsSearch);
    tcLayout->addLayout(topRow);

    coordResultsTable = new QTableWidget(tableCard);
    coordResultsTable->setColumnCount(7);
    coordResultsTable->setHorizontalHeaderLabels({"ID Étudiant", "Nom / Prénom", "Faculté", "Matière", "Enseignant", "Note", "Mention"});
    coordResultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    coordResultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    coordResultsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    coordResultsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    coordResultsTable->verticalHeader()->setVisible(false);
    coordResultsTable->verticalHeader()->setDefaultSectionSize(38);
    coordResultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    coordResultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coordResultsTable->setAlternatingRowColors(true);
    coordResultsTable->setShowGrid(false);
    coordResultsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tcLayout->addWidget(coordResultsTable);
    mainLayout->addWidget(tableCard, 1);

    refreshCoordResultsTable();
    return page;
}

void AdminDashboardWindow::refreshCoordResultsTable()
{
    if (!coordResultsTable) return;

    if (m_faculteAssignee.isEmpty()) {
        coordResultsTable->setRowCount(0);
        return;
    }

    std::vector<Professeur> freshProfs = chargerProfesseurs();
    profs = freshProfs;
    std::vector<CompteEtudiant> freshComptes = chargerComptes();
    comptes = freshComptes;

    // Strict Security & Isolation: Get students authorized for this coordinator
    std::vector<CompteEtudiant> myStudents = IsolationFaculteUtils::filtrerComptesParFaculte(comptes, m_faculteAssignee);
    std::map<std::string, CompteEtudiant> studentMap;
    for (const auto &c : myStudents) {
        studentMap[c.getId()] = c;
    }

    struct CoordRow {
        QString studentId;
        QString studentName;
        QString faculte;
        QString matiere;
        QString profName;
        double note;
    };

    std::vector<CoordRow> rows;
    std::set<std::string> uniqueStudents;
    double sum = 0.0;

    QString search = coordResultsSearch ? coordResultsSearch->text().trimmed().toLower() : "";

    // Scan all notes from professors, strictly checking if the student belongs to m_faculteAssignee
    for (const auto &p : profs) {
        QString pName = QString("%1 %2").arg(QString::fromStdString(p.getNom()), QString::fromStdString(p.getPrenom()));
        for (const auto &n : p.getNotes()) {
            auto it = studentMap.find(n.idEtudiant);
            if (it != studentMap.end()) {
                // Student belongs to this coordinator's faculty!
                const CompteEtudiant &st = it->second;
                QString sName = QString("%1 %2").arg(QString::fromStdString(st.getNom()), QString::fromStdString(st.getPrenom()));
                QString sId = QString::fromStdString(st.getId());
                QString sMat = QString::fromStdString(n.matiere);

                if (search.isEmpty() || sId.toLower().contains(search) || sName.toLower().contains(search) || sMat.toLower().contains(search) || pName.toLower().contains(search)) {
                    CoordRow r;
                    r.studentId = sId;
                    r.studentName = sName;
                    r.faculte = m_faculteAssignee;
                    r.matiere = sMat;
                    r.profName = pName;
                    r.note = n.valeur;
                    rows.push_back(r);

                    uniqueStudents.insert(n.idEtudiant);
                    sum += n.valeur;
                }
            } else {
                // Not in this coordinator's faculty -> STRICTLY BLOCKED / IGNORED
            }
        }
    }

    coordResultsTable->setRowCount((int)rows.size());

    for (int r = 0; r < (int)rows.size(); ++r) {
        const auto &data = rows[r];

        coordResultsTable->setItem(r, 0, new QTableWidgetItem(data.studentId));
        coordResultsTable->setItem(r, 1, new QTableWidgetItem(data.studentName));
        coordResultsTable->setItem(r, 2, new QTableWidgetItem(data.faculte));
        coordResultsTable->setItem(r, 3, new QTableWidgetItem(data.matiere));
        coordResultsTable->setItem(r, 4, new QTableWidgetItem(data.profName));

        auto *valItem = new QTableWidgetItem(QString::number(data.note, 'f', 1) + " / 100");
        valItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        valItem->setForeground(QColor(data.note >= 70 ? "#27ae60" : (data.note >= 50 ? "#f39c12" : "#e74c3c")));
        coordResultsTable->setItem(r, 5, valItem);

        QString mention = data.note >= 90 ? "A+ (Très Bien)" : (data.note >= 80 ? "A (Bien)" : (data.note >= 70 ? "B (Assez Bien)" : (data.note >= 60 ? "C (Passable)" : (data.note >= 50 ? "D (Admis)" : "E (Ajourné)"))));
        QString mentionColor = data.note >= 80 ? "#27ae60" : (data.note >= 50 ? "#3182ce" : "#e74c3c");
        auto *mentionItem = new QTableWidgetItem(mention);
        mentionItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        mentionItem->setForeground(QColor(mentionColor));
        coordResultsTable->setItem(r, 6, mentionItem);
    }

    if (cardCoordNotesCount) cardCoordNotesCount->updateValue(QString::number(rows.size()));
    if (cardCoordStudentsCount) cardCoordStudentsCount->updateValue(QString::number(uniqueStudents.size()));
    if (cardCoordNotesAverage) {
        QString avgStr = rows.empty() ? "—" : QString::number(sum / rows.size(), 'f', 1) + " / 100";
        cardCoordNotesAverage->updateValue(avgStr);
    }
}

QWidget* AdminDashboardWindow::createGenericPage(const QString &titleText, const QString &descriptionText, const QString &icon)
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 14px; border: 1px solid #e2e8f0; }");

    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(40, 40, 40, 40);
    cLayout->setSpacing(15);
    cLayout->setAlignment(Qt::AlignCenter);

    QLabel *ic = new QLabel(icon, card);
    ic->setStyleSheet("font-size: 50px; border: none;");

    QLabel *t = new QLabel(titleText, card);
    t->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; border: none;");

    QLabel *d = new QLabel(descriptionText, card);
    d->setStyleSheet("font-size: 14px; color: #718096; border: none; text-align: center;");
    d->setWordWrap(true);
    d->setAlignment(Qt::AlignCenter);

    cLayout->addWidget(ic, 0, Qt::AlignCenter);
    cLayout->addWidget(t, 0, Qt::AlignCenter);
    cLayout->addWidget(d, 0, Qt::AlignCenter);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* AdminDashboardWindow::createSecretaryAnnoncesPage()
{
    gestionAnnoncesWidget = new GestionAnnonces(this);
    return gestionAnnoncesWidget;
}

QWidget* AdminDashboardWindow::createAnnoncesRecuesPage()
{
    annoncesRecuesWidget = new AnnoncesRecues("Administrateurs", currentAdmin.getId(), this);
    connect(annoncesRecuesWidget, &AnnoncesRecues::lectureEffectuee, this, &AdminDashboardWindow::updateAnnoncesBadge);
    return annoncesRecuesWidget;
}

QWidget* AdminDashboardWindow::createSettingsPage()
{
    ParametresCompteBase *sett = new ParametresCompteBase(&currentAdmin, admins, this);
    connect(sett, &ParametresCompteBase::logoutRequested, this, &AdminDashboardWindow::logoutRequested);
    connect(sett, &ParametresCompteBase::profilUpdated, this, [this]() {
        admins = chargerAdministrateurs();
        for (const auto &a : admins) {
            if (a.getId() == currentAdmin.getId() || (!a.getEmail().empty() && a.getEmail() == currentAdmin.getEmail())) {
                currentAdmin = a;
                break;
            }
        }
    });
    return sett;
}

