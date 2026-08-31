#include "teacherdashboardwindow.h"
#include "statutsalaireprofesseur.h"
#include "annoncesrecues.h"
#include "calendrierscolaire.h"
#include "modulesupport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

// ============================================================
//  StatCard Implementation
// ============================================================
StatCard::StatCard(const QString &icon, const QString &value, const QString &label, const QString &circleColor, QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    setMinimumHeight(100);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(15);

    // Circle Icon container
    QLabel *iconCircle = new QLabel(icon, this);
    iconCircle->setFixedSize(54, 54);
    iconCircle->setAlignment(Qt::AlignCenter);
    iconCircle->setStyleSheet(QString(
        "background-color: %1; "
        "color: #ffffff; "
        "border-radius: 27px; "
        "font-size: 24px; "
        "border: none;"
    ).arg(circleColor));

    // Text Container
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    textLayout->setAlignment(Qt::AlignVCenter);

    valLabel = new QLabel(value, this);
    valLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a202c; background: transparent; border: none;");

    QLabel *subLabel = new QLabel(label, this);
    subLabel->setStyleSheet("font-size: 12px; font-weight: 500; color: #718096; background: transparent; border: none;");

    textLayout->addWidget(valLabel);
    textLayout->addWidget(subLabel);

    layout->addWidget(iconCircle);
    layout->addLayout(textLayout);
    layout->addStretch();
}

void StatCard::updateValue(const QString &newValue) {
    if (valLabel) {
        valLabel->setText(newValue);
    }
}

// ============================================================
//  TeacherDashboardWindow Implementation
// ============================================================
TeacherDashboardWindow::TeacherDashboardWindow(Professeur prof, std::vector<Professeur> &profsRef, QWidget *parent)
    : QMainWindow(parent), currentProf(prof), profs(profsRef),
      stackedWidget(nullptr), sidebarBtnGroup(nullptr),
      cardSubjects(nullptr), cardGrades(nullptr), cardPhotos(nullptr),
      salaryWidget(nullptr), annoncesRecuesWidget(nullptr),
      calendrierWidget(nullptr), supportWidget(nullptr),
      dashGradesTable(nullptr), viewGradesTable(nullptr), viewPhotosTable(nullptr),
      gradeSubjectCombo(nullptr), gradeStudentIdEdit(nullptr), gradeValueEdit(nullptr),
      photoSubjectCombo(nullptr), photoFilePathEdit(nullptr),
      settPhoneEdit(nullptr), settEmailEdit(nullptr),
      settCurrentPwdEdit(nullptr), settNewPwdEdit(nullptr)
{
    setupUI();
}

TeacherDashboardWindow::~TeacherDashboardWindow() {}

void TeacherDashboardWindow::setupUI()
{
    setMinimumSize(1100, 700);
    setWindowTitle("UDH - Espace Enseignant");

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #f0f2f5; font-family: 'Segoe UI', sans-serif;");
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    QWidget *sidebar = createSidebar();
    mainLayout->addWidget(sidebar);

    // Main Content Area (StackedWidget inside ScrollArea)
    stackedWidget = new QStackedWidget(this);

    stackedWidget->addWidget(createDashboardPage());     // 0: Tableau de bord (Defaut)
    stackedWidget->addWidget(createProfilePage());       // 1: Profil
    stackedWidget->addWidget(createSubjectsPage());      // 2: Mes matières
    stackedWidget->addWidget(createTimetablePage());     // 3: Emploi du temps
    stackedWidget->addWidget(createAssignGradePage());   // 4: Attribuer / modifier une note
    stackedWidget->addWidget(createViewGradesPage());    // 5: Consulter les notes enregistrées
    stackedWidget->addWidget(createUploadPhotoPage());   // 6: Uploader une photo d'examen
    stackedWidget->addWidget(createViewPhotosPage());   // 7: Consulter les photos d'examens
    stackedWidget->addWidget(createSalaryPage());       // 8: Statut de salaire
    stackedWidget->addWidget(createAnnoncesPage());     // 9: Annonces
    stackedWidget->addWidget(createEventsPage());       // 10: Événements (Calendrier scolaire)
    stackedWidget->addWidget(createSupportPage());      // 11: Support
    stackedWidget->addWidget(createSettingsPage());     // 12: Paramètres

    mainLayout->addWidget(stackedWidget, 1);
}

QWidget* TeacherDashboardWindow::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(260);
    sidebar->setStyleSheet("background-color: #0b1e36; color: white;");

    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(4);

    // University Logo in Sidebar
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

    QLabel *logoSub = new QLabel("PORTAIL ENSEIGNANT");
    logoSub->setAlignment(Qt::AlignCenter);
    logoSub->setStyleSheet("font-size: 10px; font-weight: bold; color: #90a4ae; margin-bottom: 15px; background: transparent; letter-spacing: 1px;");
    layout->addWidget(logoSub);

    // 13 Sidebar Items
    QStringList menuItems = {
        "📊   Tableau de bord",
        "👤   Profil",
        "📚   Mes matières",
        "🗓️   Emploi du temps",
        "✏️   Attribuer / modifier note",
        "📋   Consulter les notes",
        "📤   Uploader photo d'examen",
        "🖼️   Photos d'examens",
        "💳   Statut de salaire",
        "📢   Annonces",
        "📅   Événements",
        "🎧   Support",
        "⚙️   Paramètres"
    };

    sidebarBtnGroup = new QButtonGroup(this);
    sidebarBtnGroup->setExclusive(true);

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *btn = new QPushButton(menuItems[i]);
        btn->setFixedHeight(42);
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { text-align: left; padding-left: 22px; border: none; font-size: 13px; background-color: transparent; color: #b3c2d6; }"
            "QPushButton:hover { background-color: #1a3353; color: white; }"
            "QPushButton:checked { background-color: #1a3353; color: #d4af37; font-weight: bold; border-left: 4px solid #d4af37; }"
        );
        if (i == 0) btn->setChecked(true);

        sidebarBtnGroup->addButton(btn, i);
        layout->addWidget(btn);
    }

    connect(sidebarBtnGroup, &QButtonGroup::idClicked, this, &TeacherDashboardWindow::onSidebarBtnClicked);

    layout->addStretch();

    updateAnnoncesBadge();

    // Déconnexion
    QPushButton *logoutBtn = new QPushButton("🚪   Déconnexion");
    logoutBtn->setFixedHeight(42);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { text-align: left; padding-left: 22px; border: none; font-size: 13px; background-color: transparent; color: #ff6b6b; font-weight: bold; }"
        "QPushButton:hover { background-color: #3d232a; color: #ff8787; }"
    );
    connect(logoutBtn, &QPushButton::clicked, this, &TeacherDashboardWindow::logoutRequested);
    layout->addWidget(logoutBtn);

    return sidebar;
}

void TeacherDashboardWindow::updateAnnoncesBadge()
{
    int nonLues = compterAnnoncesNonLues("Professeurs", currentProf.getId());
    int annoncesBtnIndex = 9; // index of Annonces in sidebar
    if (sidebarBtnGroup && sidebarBtnGroup->button(annoncesBtnIndex)) {
        if (nonLues > 0) {
            sidebarBtnGroup->button(annoncesBtnIndex)->setText(QString("📢   Annonces  (%1)").arg(nonLues));
        } else {
            sidebarBtnGroup->button(annoncesBtnIndex)->setText("📢   Annonces");
        }
    }
}

void TeacherDashboardWindow::onSidebarBtnClicked(int index)
{
    if (index >= 0 && index < stackedWidget->count()) {
        // Recharger les données du professeur depuis le fichier
        std::vector<Professeur> freshProfs = chargerProfesseurs();
        for (const auto &p : freshProfs) {
            if (p.getId() == currentProf.getId()) {
                currentProf = p;
                break;
            }
        }
        // Synchroniser aussi la référence
        for (size_t i = 0; i < profs.size(); ++i) {
            if (profs[i].getId() == currentProf.getId()) {
                profs[i] = currentProf;
                break;
            }
        }
        refreshGradesTable();
        refreshPhotosTable();
        refreshSalaryPage();
        if (index == 9 && annoncesRecuesWidget) {
            annoncesRecuesWidget->refreshData();
            updateAnnoncesBadge();
        } else if (index == 10 && calendrierWidget) {
            calendrierWidget->refreshData();
        } else if (index == 11 && supportWidget) {
            supportWidget->refreshData();
        }
        stackedWidget->setCurrentIndex(index);
    }
}

// ============================================================
//  0. DASHBOARD PAGE
// ============================================================
QWidget* TeacherDashboardWindow::createDashboardPage()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f0f2f5; color: #333;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(22);

    // Welcome Banner
    QWidget *banner = new QWidget();
    banner->setMinimumHeight(120);
    banner->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #0b1e36, stop:0.5 #1a365d, stop:1 #2b6cb0);"
        "border-radius: 15px; color: white;"
    );
    QHBoxLayout *bannerLayout = new QHBoxLayout(banner);
    bannerLayout->setContentsMargins(30, 20, 30, 20);

    QVBoxLayout *bannerTextLayout = new QVBoxLayout();
    bannerTextLayout->setSpacing(5);

    QLabel *greetingLabel = new QLabel(QString("Bonjour, Prof. %1 %2 ! 👋")
        .arg(QString::fromStdString(currentProf.getPrenom()))
        .arg(QString::fromStdString(currentProf.getNom())));
    greetingLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white; background: transparent;");

    QLabel *subtitleLabel = new QLabel(QString("Espace d'enseignement — %1").arg(QString::fromStdString(currentProf.getFaculte())));
    subtitleLabel->setStyleSheet("font-size: 13px; color: rgba(255,255,255,0.9); background: transparent;");

    bannerTextLayout->addWidget(greetingLabel);
    bannerTextLayout->addWidget(subtitleLabel);

    QPushButton *logoutBannerBtn = new QPushButton("Déconnexion");
    logoutBannerBtn->setCursor(Qt::PointingHandCursor);
    logoutBannerBtn->setStyleSheet(
        "QPushButton { background-color: rgba(255,255,255,0.15); color: white; border: 1px solid rgba(255,255,255,0.3); border-radius: 8px; padding: 8px 18px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(231,76,60,0.8); border: 1px solid #e74c3c; }"
    );
    connect(logoutBannerBtn, &QPushButton::clicked, this, &TeacherDashboardWindow::logoutRequested);

    bannerLayout->addLayout(bannerTextLayout, 1);
    bannerLayout->addWidget(logoutBannerBtn, 0, Qt::AlignVCenter);
    layout->addWidget(banner);

    // 3 Stat Cards Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    int numSubjects = (int)currentProf.getMatieres().size();
    int numGrades   = (int)currentProf.getNotes().size();
    int numPhotos   = (int)currentProf.getPhotosExamens().size();

    cardSubjects = new StatCard("📚", QString::number(numSubjects), "Matières enseignées", "#2b6cb0", this);
    cardGrades   = new StatCard("📝", QString::number(numGrades), "Notes enregistrées", "#27ae60", this);
    cardPhotos   = new StatCard("📷", QString::number(numPhotos), "Photos d'examens", "#8e44ad", this);

    statsLayout->addWidget(cardSubjects);
    statsLayout->addWidget(cardGrades);
    statsLayout->addWidget(cardPhotos);
    layout->addLayout(statsLayout);

    // Card Frame for Table
    QFrame *tableCard = new QFrame(page);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect(tableCard);
    tableShadow->setBlurRadius(15);
    tableShadow->setColor(QColor(0, 0, 0, 15));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(15);

    // Header Controls
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *tableTitle = new QLabel("Notes Attribuées", tableCard);
    tableTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #1a202c; background: transparent; border: none;");

    QPushButton *addGradeBtn = new QPushButton("➕ Attribuer une note", tableCard);
    addGradeBtn->setCursor(Qt::PointingHandCursor);
    addGradeBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; padding: 6px 14px; font-weight: bold; font-size: 12px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(addGradeBtn, &QPushButton::clicked, [this]() {
        sidebarBtnGroup->button(3)->setChecked(true);
        stackedWidget->setCurrentIndex(3);
    });

    headerLayout->addWidget(tableTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(addGradeBtn);
    tcLayout->addLayout(headerLayout);

    // Table Widget
    dashGradesTable = new QTableWidget(tableCard);
    dashGradesTable->setColumnCount(5);
    dashGradesTable->setHorizontalHeaderLabels({"Matière", "Nom / Prénom", "ID Étudiant", "Note (/100)", "Statut"});
    dashGradesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dashGradesTable->verticalHeader()->setVisible(false);
    dashGradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dashGradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dashGradesTable->setAlternatingRowColors(true);
    dashGradesTable->setShowGrid(false);
    dashGradesTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; gridline-color: transparent; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );

    refreshGradesTable();
    dashGradesTable->setFixedHeight(260);

    tcLayout->addWidget(dashGradesTable);
    layout->addWidget(tableCard);

    layout->addStretch();

    // Footer
    QLabel *footer = new QLabel("© 2024 Université de Développement d'Haïti (UDH) — Espace Enseignant");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("font-size: 11px; color: #a0aec0; background: transparent; padding: 5px;");
    layout->addWidget(footer);

    scrollArea->setWidget(page);
    return scrollArea;
}

void TeacherDashboardWindow::refreshGradesTable()
{
    for (const auto &p : profs) {
        if (p.getId() == currentProf.getId()) {
            currentProf = p;
            break;
        }
    }

    const auto &notes = currentProf.getNotes();

    auto populate = [this, &notes](QTableWidget *table) {
        if (!table) return;
        table->setRowCount((int)notes.size());

        for (int i = 0; i < (int)notes.size(); ++i) {
            const Note &n = notes[i];

            std::string nomCompletEtud = "Étudiant Inconnu";
            rechercherEtudiant(n.idEtudiant, nomCompletEtud);

            auto *itemMat = new QTableWidgetItem(QString::fromStdString(n.matiere));
            auto *itemNom = new QTableWidgetItem(QString::fromStdString(nomCompletEtud));
            auto *itemId  = new QTableWidgetItem(QString::fromStdString(n.idEtudiant));
            auto *itemVal = new QTableWidgetItem(QString::number(n.valeur, 'f', 1) + " / 100");

            itemMat->setForeground(QColor("#1a202c"));
            itemNom->setForeground(QColor("#2d3748"));
            itemId->setForeground(QColor("#718096"));
            itemVal->setFont(QFont("Segoe UI", 10, QFont::Bold));
            itemVal->setForeground(QColor(n.valeur >= 70 ? "#27ae60" : (n.valeur >= 50 ? "#f39c12" : "#e74c3c")));

            QString stText = (n.statut == "Transférée") ? "✅ Transférée aux étudiants" : "⏳ Reçue (En attente de transfert)";
            QString stColor = (n.statut == "Transférée") ? "#27ae60" : "#e67e22";
            auto *itemStatut = new QTableWidgetItem(stText);
            itemStatut->setFont(QFont("Segoe UI", 9, QFont::Bold));
            itemStatut->setForeground(QColor(stColor));

            table->setItem(i, 0, itemMat);
            table->setItem(i, 1, itemNom);
            table->setItem(i, 2, itemId);
            table->setItem(i, 3, itemVal);
            table->setItem(i, 4, itemStatut);
        }
    };

    populate(dashGradesTable);
    populate(viewGradesTable);

    if (cardGrades) {
        cardGrades->updateValue(QString::number(notes.size()));
    }
}

// ============================================================
//  1. PROFILE PAGE
// ============================================================
QWidget* TeacherDashboardWindow::createProfilePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(30, 30, 30, 30);
    cLayout->setSpacing(15);

    QLabel *title = new QLabel("Profil Enseignant");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");

    auto makeField = [](const QString &lbl, const QString &val) {
        QHBoxLayout *h = new QHBoxLayout();
        QLabel *l = new QLabel(lbl + " :");
        l->setFixedWidth(140);
        l->setStyleSheet("font-weight: bold; color: #4a5568; font-size: 14px; border: none;");
        QLabel *v = new QLabel(val);
        v->setStyleSheet("color: #1a202c; font-size: 14px; border: none;");
        h->addWidget(l);
        h->addWidget(v, 1);
        return h;
    };

    cLayout->addWidget(title);
    cLayout->addSpacing(10);
    cLayout->addLayout(makeField("ID Professeur", QString::fromStdString(currentProf.getId())));
    cLayout->addLayout(makeField("Nom complet", QString("Prof. %1 %2").arg(QString::fromStdString(currentProf.getPrenom()), QString::fromStdString(currentProf.getNom()))));
    cLayout->addLayout(makeField("Email", QString::fromStdString(currentProf.getEmail())));
    cLayout->addLayout(makeField("Téléphone", QString::fromStdString(currentProf.getTelephone())));
    cLayout->addLayout(makeField("Faculté", QString::fromStdString(currentProf.getFaculte())));
    cLayout->addLayout(makeField("Statut du compte", currentProf.estValide() ? "✅ Valide" : "⏳ En attente de validation"));

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ============================================================
//  2. MES MATIÈRES
// ============================================================
QWidget* TeacherDashboardWindow::createSubjectsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(30, 25, 30, 25);
    cLayout->setSpacing(15);

    QLabel *title = new QLabel("Matières Enseignées");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    const auto &matieres = currentProf.getMatieres();
    if (matieres.empty()) {
        QLabel *empty = new QLabel("Aucune matière n'est actuellement assignée à votre profil.");
        empty->setStyleSheet("color: #718096; font-size: 14px; border: none;");
        cLayout->addWidget(empty);
    } else {
        for (size_t i = 0; i < matieres.size(); ++i) {
            QFrame *item = new QFrame();
            item->setStyleSheet("QFrame { background-color: #f8fafc; border-radius: 8px; border: 1px solid #e2e8f0; }");
            QHBoxLayout *h = new QHBoxLayout(item);
            h->setContentsMargins(15, 12, 15, 12);
            QLabel *icon = new QLabel("📚");
            icon->setStyleSheet("font-size: 18px; border: none;");
            QLabel *name = new QLabel(QString::fromStdString(matieres[i]));
            name->setStyleSheet("font-weight: bold; color: #2d3748; font-size: 14px; border: none;");
            h->addWidget(icon);
            h->addWidget(name, 1);
            cLayout->addWidget(item);
        }
    }

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ============================================================
//  3. EMPLOI DU TEMPS
// ============================================================
QWidget* TeacherDashboardWindow::createTimetablePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(25, 20, 25, 25);
    cLayout->setSpacing(15);

    QLabel *title = new QLabel("Emploi du temps des cours");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    QTableWidget *ttTable = new QTableWidget(3, 5, card);
    ttTable->setHorizontalHeaderLabels({"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"});
    ttTable->setVerticalHeaderLabels({"08h00 - 10h00", "10h15 - 12h15", "13h30 - 15h30"});
    ttTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ttTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ttTable->setStyleSheet(
        "QTableWidget { background-color: white; border: 1px solid #e2e8f0; font-size: 12px; }"
        "QHeaderView::section { background-color: #0b1e36; color: white; font-weight: bold; padding: 8px; }"
        "QTableWidget::item { padding: 8px; border: 1px solid #edf2f7; text-align: center; }"
    );

    const auto &matieres = currentProf.getMatieres();
    int idx = 0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 5; ++c) {
            QString m = matieres.empty() ? "N/A" : QString::fromStdString(matieres[idx % matieres.size()]);
            QTableWidgetItem *item = new QTableWidgetItem(m);
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor("#2b6cb0"));
            item->setFont(QFont("Segoe UI", 9, QFont::Bold));
            ttTable->setItem(r, c, item);
            idx++;
        }
    }
    ttTable->setFixedHeight(220);
    cLayout->addWidget(ttTable);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ============================================================
//  4. ATTRIBUER / MODIFIER UNE NOTE
// ============================================================
QWidget* TeacherDashboardWindow::createAssignGradePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(35, 30, 35, 30);
    cLayout->setSpacing(18);

    QLabel *title = new QLabel("Attribuer ou modifier une note");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    const QString inputStyle = 
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: 500; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 34px; padding: 6px 10px; color: #1a202c; background-color: #ffffff; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }";

    gradeSubjectCombo = new QComboBox();
    gradeSubjectCombo->setFixedHeight(42);
    gradeSubjectCombo->setStyleSheet(inputStyle);
    for (const auto &m : currentProf.getMatieres()) {
        gradeSubjectCombo->addItem(QString::fromStdString(m));
    }

    gradeStudentIdEdit = new QLineEdit();
    gradeStudentIdEdit->setPlaceholderText("ID Étudiant (ex: ETU-PM-12345)");
    gradeStudentIdEdit->setFixedHeight(42);
    gradeStudentIdEdit->setStyleSheet(inputStyle);

    gradeValueEdit = new QLineEdit();
    gradeValueEdit->setPlaceholderText("Note sur 100 (ex: 85.5)");
    gradeValueEdit->setFixedHeight(42);
    gradeValueEdit->setStyleSheet(inputStyle);

    QPushButton *saveBtn = new QPushButton("💾 Enregistrer la note");
    saveBtn->setFixedHeight(45);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 15px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(saveBtn, &QPushButton::clicked, this, &TeacherDashboardWindow::onSaveNoteClicked);

    cLayout->addWidget(new QLabel("Sélectionnez la matière :"));
    cLayout->addWidget(gradeSubjectCombo);
    cLayout->addWidget(new QLabel("ID de l'étudiant :"));
    cLayout->addWidget(gradeStudentIdEdit);
    cLayout->addWidget(new QLabel("Note attribuée (/100) :"));
    cLayout->addWidget(gradeValueEdit);
    cLayout->addSpacing(10);
    cLayout->addWidget(saveBtn);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

void TeacherDashboardWindow::onSaveNoteClicked()
{
    QString subject = gradeSubjectCombo->currentText();
    QString studentId = gradeStudentIdEdit->text().trimmed();
    QString valStr = gradeValueEdit->text().trimmed();

    if (subject.isEmpty() || studentId.isEmpty() || valStr.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir tous les champs du formulaire.");
        return;
    }

    bool ok;
    double val = valStr.toDouble(&ok);
    if (!ok || val < 0 || val > 100) {
        QMessageBox::warning(this, "Valeur invalide", "La note doit être un nombre compris entre 0 et 100.");
        return;
    }

    currentProf.attribuerNote(studentId.toStdString(), subject.toStdString(), val);
    enregistrerProfesseur(profs, currentProf);

    refreshGradesTable();

    QMessageBox::information(this, "Succès", QString("Note de %1/100 enregistrée pour l'étudiant %2 dans la matière %3.")
        .arg(val).arg(studentId, subject));

    gradeStudentIdEdit->clear();
    gradeValueEdit->clear();
}

// ============================================================
//  5. CONSULTER LES NOTES ENREGISTRÉES
// ============================================================
QWidget* TeacherDashboardWindow::createViewGradesPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(25, 20, 25, 25);
    cLayout->setSpacing(15);

    QLabel *title = new QLabel("Liste complète des notes enregistrées");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    viewGradesTable = new QTableWidget(card);
    viewGradesTable->setColumnCount(5);
    viewGradesTable->setHorizontalHeaderLabels({"Matière", "Nom / Prénom", "ID Étudiant", "Note (/100)", "Statut"});
    viewGradesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    viewGradesTable->verticalHeader()->setVisible(false);
    viewGradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewGradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    viewGradesTable->setAlternatingRowColors(true);
    viewGradesTable->setShowGrid(false);
    viewGradesTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; gridline-color: transparent; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );

    refreshGradesTable();
    viewGradesTable->setMinimumHeight(350);

    cLayout->addWidget(viewGradesTable);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ============================================================
//  6. UPLOADER UNE PHOTO D'EXAMEN
// ============================================================
QWidget* TeacherDashboardWindow::createUploadPhotoPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(35, 30, 35, 30);
    cLayout->setSpacing(18);

    QLabel *title = new QLabel("Uploader une photo d'examen");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    const QString inputStyle = 
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 14px; font-weight: 500; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 34px; padding: 6px 10px; color: #1a202c; background-color: #ffffff; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }";

    photoSubjectCombo = new QComboBox();
    photoSubjectCombo->setFixedHeight(42);
    photoSubjectCombo->setStyleSheet(inputStyle);
    for (const auto &m : currentProf.getMatieres()) {
        photoSubjectCombo->addItem(QString::fromStdString(m));
    }

    QHBoxLayout *fileLayout = new QHBoxLayout();
    photoFilePathEdit = new QLineEdit();
    photoFilePathEdit->setPlaceholderText("Aucun fichier sélectionné...");
    photoFilePathEdit->setFixedHeight(42);
    photoFilePathEdit->setStyleSheet(inputStyle);

    QPushButton *browseBtn = new QPushButton("📁 Parcourir...");
    browseBtn->setFixedHeight(42);
    browseBtn->setCursor(Qt::PointingHandCursor);
    browseBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; padding: 0 16px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(browseBtn, &QPushButton::clicked, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Sélectionner une photo d'examen", "", "Images (*.png *.jpg *.jpeg *.pdf)");
        if (!file.isEmpty()) {
            photoFilePathEdit->setText(file);
        }
    });

    fileLayout->addWidget(photoFilePathEdit, 1);
    fileLayout->addWidget(browseBtn);

    QPushButton *uploadBtn = new QPushButton("📤 Téléverser la photo");
    uploadBtn->setFixedHeight(45);
    uploadBtn->setCursor(Qt::PointingHandCursor);
    uploadBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 15px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(uploadBtn, &QPushButton::clicked, this, &TeacherDashboardWindow::onUploadPhotoClicked);

    cLayout->addWidget(new QLabel("Sujet / Matière d'examen :"));
    cLayout->addWidget(photoSubjectCombo);
    cLayout->addWidget(new QLabel("Fichier photo de l'examen :"));
    cLayout->addLayout(fileLayout);
    cLayout->addSpacing(10);
    cLayout->addWidget(uploadBtn);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

void TeacherDashboardWindow::onUploadPhotoClicked()
{
    QString subject = photoSubjectCombo->currentText();
    QString filePath = photoFilePathEdit->text().trimmed();

    if (subject.isEmpty() || filePath.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez sélectionner une matière et choisir un fichier.");
        return;
    }

    QString dateStr = QDateTime::currentDateTime().toString("dd/MM/yyyy");

    currentProf.ajouterPhotoExamen(subject.toStdString(), filePath.toStdString(), dateStr.toStdString());
    enregistrerProfesseur(profs, currentProf);

    if (cardPhotos) {
        cardPhotos->updateValue(QString::number(currentProf.getPhotosExamens().size()));
    }

    QMessageBox::information(this, "Succès", QString("Photo d'examen ajoutée pour %1.").arg(subject));
    photoFilePathEdit->clear();
}

void TeacherDashboardWindow::refreshPhotosTable()
{
    if (!viewPhotosTable) return;

    for (const auto &p : profs) {
        if (p.getId() == currentProf.getId()) {
            currentProf = p;
            break;
        }
    }

    const auto &photos = currentProf.getPhotosExamens();
    viewPhotosTable->setRowCount((int)photos.size());
    for (int i = 0; i < (int)photos.size(); ++i) {
        viewPhotosTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(photos[i].matiere)));
        viewPhotosTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(photos[i].cheminFichier)));
        viewPhotosTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(photos[i].dateAjout)));
    }

    if (cardPhotos) {
        cardPhotos->updateValue(QString::number(photos.size()));
    }
}

// ============================================================
//  7. CONSULTER LES PHOTOS D'EXAMENS
// ============================================================
QWidget* TeacherDashboardWindow::createViewPhotosPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QFrame *card = new QFrame(page);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QVBoxLayout *cLayout = new QVBoxLayout(card);
    cLayout->setContentsMargins(25, 20, 25, 25);
    cLayout->setSpacing(15);

    QLabel *title = new QLabel("Photos d'examens téléversées");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(title);

    viewPhotosTable = new QTableWidget(card);
    viewPhotosTable->setColumnCount(3);
    viewPhotosTable->setHorizontalHeaderLabels({"Matière", "Chemin / Fichier", "Date d'ajout"});
    viewPhotosTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    viewPhotosTable->verticalHeader()->setVisible(false);
    viewPhotosTable->setStyleSheet(
        "QTableWidget { background-color: white; border: none; color: #2d3748; font-size: 13px; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; padding: 8px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; }"
    );

    refreshPhotosTable();
    viewPhotosTable->setMinimumHeight(280);
    cLayout->addWidget(viewPhotosTable);

    layout->addWidget(card);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ============================================================
//  8. STATUT DE SALAIRE
// ============================================================
QWidget* TeacherDashboardWindow::createSalaryPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    salaryWidget = new StatutSalaireProfesseur(currentProf);
    scroll->setWidget(salaryWidget);
    return scroll;
}

void TeacherDashboardWindow::refreshSalaryPage()
{
    if (salaryWidget) {
        salaryWidget->updateProf(currentProf);
    }
}

// ============================================================
//  9. PARAMÈTRES (MODULE UNIFIÉ)
// ============================================================
QWidget* TeacherDashboardWindow::createSettingsPage()
{
    ParametresCompteBase *sett = new ParametresCompteBase(&currentProf, profs, this);
    connect(sett, &ParametresCompteBase::logoutRequested, this, &TeacherDashboardWindow::logoutRequested);
    connect(sett, &ParametresCompteBase::profilUpdated, this, [this]() {
        profs = chargerProfesseurs();
        for (const auto &p : profs) {
            if (p.getId() == currentProf.getId() || (!p.getEmail().empty() && p.getEmail() == currentProf.getEmail())) {
                currentProf = p;
                break;
            }
        }
        refreshGradesTable();
    });
    return sett;
}

void TeacherDashboardWindow::onSaveSettingsClicked()
{
    QString email = settEmailEdit->text().trimmed();
    QString phone = settPhoneEdit->text().trimmed();
    QString curPwd = settCurrentPwdEdit->text();
    QString newPwd = settNewPwdEdit->text();

    if (!email.isEmpty()) currentProf.modifierEmail(email.toStdString());
    if (!phone.isEmpty()) currentProf.modifierTelephone(phone.toStdString());

    if (!curPwd.isEmpty() && !newPwd.isEmpty()) {
        if (!currentProf.verifierMotDePasse(curPwd.toStdString())) {
            QMessageBox::warning(this, "Erreur", "Le mot de passe actuel est incorrect.");
            return;
        }
        currentProf.modifierMotDePasse(newPwd.toStdString());
        settCurrentPwdEdit->clear();
        settNewPwdEdit->clear();
    }

    enregistrerProfesseur(profs, currentProf);
    QMessageBox::information(this, "Succès", "Informations du profil mises à jour avec succès.");
}

QWidget* TeacherDashboardWindow::createAnnoncesPage()
{
    annoncesRecuesWidget = new AnnoncesRecues("Professeurs", currentProf.getId(), this);
    connect(annoncesRecuesWidget, &AnnoncesRecues::lectureEffectuee, this, &TeacherDashboardWindow::updateAnnoncesBadge);
    return annoncesRecuesWidget;
}

QWidget* TeacherDashboardWindow::createEventsPage()
{
    calendrierWidget = new CalendrierScolaire(false, this);
    return calendrierWidget;
}

QWidget* TeacherDashboardWindow::createSupportPage()
{
    supportWidget = new ModuleSupport(ModuleSupport::UserMode, currentProf.getId(),
                                      currentProf.getNom() + " " + currentProf.getPrenom(),
                                      "Professeur", this);
    return supportWidget;
}

