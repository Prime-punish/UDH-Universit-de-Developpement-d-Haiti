#include "studentdashboardwindow.h"
#include "registrationdialog.h"
#include "professeur.h"
#include "annoncesrecues.h"
#include "annonce.h"
#include "calendrierscolaire.h"
#include "paiementversement.h"
#include "modulesupport.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QButtonGroup>
#include <QScrollArea>
#include <QGroupBox>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QPixmap>
#include <QTextEdit>
#include <QComboBox>

StudentDashboardWindow::StudentDashboardWindow(CompteEtudiant compte, 
                                               const std::map<std::string, ProgrammeEtudes>& progs,
                                               std::vector<CompteEtudiant>& comptesRef,
                                               QWidget *parent)
    : QWidget(parent), currentCompte(compte), programmes(progs), comptes(comptesRef),
      calendrierWidget(nullptr), paiementWidget(nullptr), supportWidget(nullptr)
{
    setupUI();
}

StudentDashboardWindow::~StudentDashboardWindow() {}

void StudentDashboardWindow::setupUI()
{
    setMinimumSize(1000, 650);
    setStyleSheet("background-color: #f5f7fa; font-family: 'Segoe UI', sans-serif;");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    QWidget *sidebar = createSidebar();
    
    // Main Content Area
    stackedWidget = new QStackedWidget();
    
    stackedWidget->addWidget(createDashboardPage());        // 0
    stackedWidget->addWidget(createProfilePage());          // 1
    stackedWidget->addWidget(createCoursesPage());          // 2
    stackedWidget->addWidget(createResultsPage());          // 3
    stackedWidget->addWidget(createTimetablePage());        // 4
    stackedWidget->addWidget(createNoticeBoardPage());      // 5
    stackedWidget->addWidget(createAcademicCalendarPage()); // 6: Événements
    stackedWidget->addWidget(createPaymentPage());          // 7: Payer un versement
    stackedWidget->addWidget(createDownloadsPage());        // 8
    stackedWidget->addWidget(createSupportPage());          // 9: Support
    stackedWidget->addWidget(createSettingsPage());         // 10

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget, 1);
}

QWidget* StudentDashboardWindow::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("background-color: #0b1e36; color: white;");

    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 15, 0, 15);
    layout->setSpacing(4);

    // University Logo in Sidebar
    QLabel *logoImg = new QLabel();
    QPixmap logoPix(":/resources/logo.png");
    if (!logoPix.isNull()) {
        logoImg->setPixmap(logoPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoImg->setAlignment(Qt::AlignCenter);
    logoImg->setStyleSheet("background: transparent; margin-top: 5px;");
    layout->addWidget(logoImg);

    QLabel *logoTitle = new QLabel("UDH");
    logoTitle->setAlignment(Qt::AlignCenter);
    logoTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #d4af37; background: transparent; letter-spacing: 2px;");
    layout->addWidget(logoTitle);

    QLabel *logoSub = new QLabel("PORTAIL ÉTUDIANT");
    logoSub->setAlignment(Qt::AlignCenter);
    logoSub->setStyleSheet("font-size: 10px; font-weight: bold; color: #90a4ae; margin-bottom: 15px; background: transparent; letter-spacing: 1px;");
    layout->addWidget(logoSub);

    QStringList menuItems = {
        "Tableau de bord", "Profil", "Cours", "Résultats", "Emploi du temps", 
        "Annonces", "Événements", "Payer un versement", "Téléchargements", "Support", "Paramètres"
    };

    sidebarBtnGroup = new QButtonGroup(this);
    sidebarBtnGroup->setExclusive(true);

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *btn = new QPushButton(menuItems[i]);
        btn->setFixedHeight(42);
        btn->setCheckable(true);
        btn->setStyleSheet("QPushButton { text-align: left; padding-left: 28px; border: none; font-size: 13px; background-color: transparent; color: #b3c2d6; }"
                           "QPushButton:hover { background-color: #1a3353; color: white; }"
                           "QPushButton:checked { background-color: #1a3353; color: #d4af37; font-weight: bold; border-left: 4px solid #d4af37; }");
        if (i == 0) btn->setChecked(true);
        
        sidebarBtnGroup->addButton(btn, i);
        layout->addWidget(btn);
    }
    
    connect(sidebarBtnGroup, &QButtonGroup::idClicked, this, &StudentDashboardWindow::onSidebarBtnClicked);

    layout->addStretch();

    updateAnnoncesBadge();

    QPushButton *logoutBtn = new QPushButton("Déconnexion");
    logoutBtn->setFixedHeight(42);
    logoutBtn->setStyleSheet("QPushButton { text-align: left; padding-left: 28px; border: none; font-size: 13px; background-color: transparent; color: #ff6b6b; }"
                             "QPushButton:hover { background-color: #3d232a; }");
    connect(logoutBtn, &QPushButton::clicked, this, &StudentDashboardWindow::logoutRequested);
    layout->addWidget(logoutBtn);

    return sidebar;
}

void StudentDashboardWindow::updateAnnoncesBadge()
{
    int nonLues = compterAnnoncesNonLues("Etudiants", currentCompte.getId());
    int annoncesBtnIndex = 5; // index of Annonces in sidebar
    if (sidebarBtnGroup && sidebarBtnGroup->button(annoncesBtnIndex)) {
        if (nonLues > 0) {
            sidebarBtnGroup->button(annoncesBtnIndex)->setText(QString("Annonces  (%1)").arg(nonLues));
        } else {
            sidebarBtnGroup->button(annoncesBtnIndex)->setText("Annonces");
        }
    }
}

void StudentDashboardWindow::onSidebarBtnClicked(int index)
{
    if (index != 0 && index != 10 && !currentCompte.estInscrit()) {
        auto reply = QMessageBox::question(
            this,
            "Inscription Requise",
            "⚠️ L'accès aux rubriques avancées nécessite une inscription administrative complétée.\n\n"
            "Souhaitez-vous finaliser votre inscription maintenant ?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
            onCompleteRegistrationClicked();
        } else {
            if (sidebarBtnGroup && sidebarBtnGroup->button(stackedWidget->currentIndex())) {
                sidebarBtnGroup->button(stackedWidget->currentIndex())->setChecked(true);
            }
        }
        return;
    }

    if (index >= 0 && index < stackedWidget->count()) {
        if (index == 0 || index == 3 || index == 5) {
            reloadAllPages();
            updateAnnoncesBadge();
        } else if (index == 6 && calendrierWidget) {
            calendrierWidget->refreshData();
        } else if (index == 7 && paiementWidget) {
            paiementWidget->refreshData();
        } else if (index == 9 && supportWidget) {
            supportWidget->refreshData();
        }
        stackedWidget->setCurrentIndex(index);
    }
}

void StudentDashboardWindow::reloadAllPages()
{
    int currentIdx = stackedWidget->currentIndex();
    while (stackedWidget->count() > 0) {
        QWidget *w = stackedWidget->widget(0);
        stackedWidget->removeWidget(w);
        delete w;
    }
    stackedWidget->addWidget(createDashboardPage());        // 0
    stackedWidget->addWidget(createProfilePage());          // 1
    stackedWidget->addWidget(createCoursesPage());          // 2
    stackedWidget->addWidget(createResultsPage());          // 3
    stackedWidget->addWidget(createTimetablePage());        // 4
    stackedWidget->addWidget(createNoticeBoardPage());      // 5
    stackedWidget->addWidget(createAcademicCalendarPage()); // 6
    stackedWidget->addWidget(createPaymentPage());          // 7
    stackedWidget->addWidget(createDownloadsPage());        // 8
    stackedWidget->addWidget(createSupportPage());          // 9
    stackedWidget->addWidget(createSettingsPage());         // 10
    
    if (currentIdx >= 0 && currentIdx < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(currentIdx);
    }
}

QWidget* StudentDashboardWindow::createUnregisteredNoticeCard()
{
    QWidget *card = new QWidget();
    card->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e0e0e0;");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(40, 50, 40, 50);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);

    QLabel *lockIcon = new QLabel("🔒");
    lockIcon->setStyleSheet("font-size: 50px; background: transparent; border: none;");
    lockIcon->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Accès Réservé aux Étudiants Inscrits");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
    title->setAlignment(Qt::AlignCenter);

    QLabel *desc = new QLabel("Cette section est verrouillée tant que votre inscription administrative n'a pas été complétée.\nCliquez ci-dessous pour renseigner vos informations et activer vos accès.");
    desc->setStyleSheet("font-size: 14px; color: #7f8c8d; background: transparent; border: none;");
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);

    QPushButton *regBtn = new QPushButton("⚡ Compléter mon inscription maintenant");
    regBtn->setFixedSize(300, 45);
    regBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(regBtn, &QPushButton::clicked, this, &StudentDashboardWindow::onCompleteRegistrationClicked);

    layout->addWidget(lockIcon);
    layout->addWidget(title);
    layout->addWidget(desc);
    layout->addSpacing(10);
    layout->addWidget(regBtn, 0, Qt::AlignHCenter);

    return card;
}

QWidget* StudentDashboardWindow::createDashboardPage()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #333;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    // ========== WELCOME BANNER (BLUE GRADIENT) ==========
    QWidget *banner = new QWidget();
    banner->setMinimumHeight(130);
    banner->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #0b1e36, stop:0.5 #1a365d, stop:1 #2b6cb0);"
        "border-radius: 15px; color: white;"
    );
    QHBoxLayout *bannerLayout = new QHBoxLayout(banner);
    bannerLayout->setContentsMargins(30, 20, 30, 20);

    QVBoxLayout *bannerTextLayout = new QVBoxLayout();
    bannerTextLayout->setSpacing(5);

    greetingLabel = new QLabel(QString("Bonjour, 👋\n%1 %2 !")
        .arg(QString::fromStdString(currentCompte.getPrenom()))
        .arg(QString::fromStdString(currentCompte.getNom())));
    greetingLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white; background: transparent;");

    QLabel *subtitleLabel = new QLabel("Bienvenue sur le portail de l'Université de Développement d'Haïti (UDH)");
    subtitleLabel->setStyleSheet("font-size: 13px; color: rgba(255,255,255,0.9); background: transparent;");

    bannerTextLayout->addWidget(greetingLabel);
    bannerTextLayout->addWidget(subtitleLabel);

    // University Logo in Banner with transparent background
    QLabel *bannerLogo = new QLabel();
    QPixmap bannerLogoPix(":/resources/logo.png");
    if (!bannerLogoPix.isNull()) {
        bannerLogo->setPixmap(bannerLogoPix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    bannerLogo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bannerLogo->setStyleSheet("background: transparent;");

    bannerLayout->addLayout(bannerTextLayout, 1);
    bannerLayout->addWidget(bannerLogo);
    layout->addWidget(banner);

    // ========== STAT CARDS ROW (3 CARDS DYNAMIQUES) ==========
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(15);

    // 1. Calcul de la moyenne réelle (notes avec statut Transférée)
    std::vector<Professeur> profsList = chargerProfesseurs();
    double totalNotes = 0.0;
    int countNotes = 0;
    for (const auto &p : profsList) {
        for (const auto &n : p.getNotes()) {
            if (n.idEtudiant == currentCompte.getId() && n.statut == "Transférée") {
                totalNotes += n.valeur;
                countNotes++;
            }
        }
    }
    QString moyenneStr = (countNotes > 0) ? QString::number(totalNotes / countNotes, 'f', 1) : "—";
    QString moyenneSub = (countNotes > 0) ? "Sur 100" : "Aucune note";

    // 2. Calcul des annonces non lues
    int unreadAnnCount = compterAnnoncesNonLues("Etudiants", currentCompte.getId());
    QString annValStr = QString::number(unreadAnnCount);

    // 3. Calcul du reste à payer
    const double MONTANT_ANNUEL = 120000.0;
    std::vector<VersementEtudiant> versementsList = chargerVersements();
    double totalPaye = 0.0;
    for (const auto &v : versementsList) {
        if (v.idEtudiant == currentCompte.getId() && (v.statut == "Confirmé" || v.statut == "Validé")) {
            totalPaye += v.montant;
        }
    }
    double resteAPayer = MONTANT_ANNUEL - totalPaye;
    QString resteValStr;
    QString resteSubStr;
    QString resteColor;
    if (resteAPayer <= 0.0) {
        resteValStr = "Payé intégralement";
        resteSubStr = "Scolarité soldée";
        resteColor = "#27ae60";
    } else {
        resteValStr = QString("%1 Gdes").arg(resteAPayer, 0, 'f', 0);
        resteSubStr = "Sur 120 000 Gdes (année)";
        resteColor = "#27ae60";
    }

    statsLayout->addWidget(createStatCard("📊", moyenneStr, "Moyenne", moyenneSub, "#4285F4"), 1);
    statsLayout->addWidget(createStatCard("🔔", annValStr, "Annonces non lues", "À consulter", "#ea580c"), 1);
    statsLayout->addWidget(createStatCard("💰", resteValStr, "Reste à payer", resteSubStr, resteColor), 1);
    
    layout->addLayout(statsLayout);

    // ========== RESULTS + NOTICE BOARD ROW ==========
    QHBoxLayout *midRow = new QHBoxLayout();
    midRow->setSpacing(20);
    midRow->addWidget(createRecentResultsSection(), 3);
    midRow->addWidget(createNoticeBoardSection(), 2);
    layout->addLayout(midRow);

    // ========== CALENDAR + QUICK ACCESS ROW ==========
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(20);
    bottomRow->addWidget(createCalendarSection(), 3);
    bottomRow->addWidget(createQuickAccessSection(), 2);
    layout->addLayout(bottomRow);

    // ========== FOOTER ==========
    QLabel *footer = new QLabel("© 2024 Système de Gestion Étudiante UDH. Tous droits réservés.        Fait avec ❤ pour les étudiants");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("font-size: 11px; color: #999; background: transparent; padding: 5px;");
    layout->addWidget(footer);

    // Registration button if not registered
    if (!currentCompte.estInscrit()) {
        QPushButton *completeRegBtn = new QPushButton("⚡ Compléter l'Inscription Maintenant");
        completeRegBtn->setFixedHeight(50);
        completeRegBtn->setStyleSheet(
            "QPushButton { background-color: #0b1e36; color: white; border-radius: 10px; "
            "font-weight: bold; font-size: 16px; border: none; }"
            "QPushButton:hover { background-color: #1a3353; }"
        );
        connect(completeRegBtn, &QPushButton::clicked, this, &StudentDashboardWindow::onCompleteRegistrationClicked);
        layout->insertWidget(1, completeRegBtn); // Insert right after banner
    }

    statusLabel = new QLabel(currentCompte.estInscrit() ? "Inscrit" : "En attente");
    statusLabel->setVisible(false); // Hidden, used internally

    scrollArea->setWidget(page);
    return scrollArea;
}

// ========== STAT CARD HELPER ==========
QWidget* StudentDashboardWindow::createStatCard(const QString& icon, const QString& value, 
                                                 const QString& label, const QString& sublabel, 
                                                 const QString& accentColor)
{
    QWidget *card = new QWidget();
    card->setMinimumHeight(110);
    card->setStyleSheet(
        QString("QWidget { background-color: white; border-radius: 12px; "
                "border: 1px solid #e8e8e8; }"
        )
    );

    QHBoxLayout *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    cardLayout->setSpacing(12);

    // Icon circle
    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setFixedSize(45, 45);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(
        QString("background-color: %1; border-radius: 22px; font-size: 20px; color: white;").arg(accentColor)
    );

    // Text section
    QWidget *textWidget = new QWidget();
    textWidget->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(2);

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");

    QLabel *labelLabel = new QLabel(label);
    labelLabel->setStyleSheet("font-size: 11px; font-weight: bold; color: #7f8c8d; background: transparent; border: none;");

    QLabel *subLabel = new QLabel(sublabel);
    subLabel->setStyleSheet(QString("font-size: 10px; color: %1; background: transparent; border: none;").arg(accentColor));

    textLayout->addWidget(valueLabel);
    textLayout->addWidget(labelLabel);
    textLayout->addWidget(subLabel);

    cardLayout->addWidget(iconLabel);
    cardLayout->addWidget(textWidget, 1);

    return card;
}

// ========== RECENT RESULTS SECTION ==========
QWidget* StudentDashboardWindow::createRecentResultsSection()
{
    QWidget *section = new QWidget();
    section->setStyleSheet("QWidget { background-color: white; border-radius: 12px; border: 1px solid #e8e8e8; }");
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(10);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("Résultats Récents");
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
    QLabel *viewAll = new QLabel("<a style='color: #4285F4; text-decoration: none;' href='#'>Voir tous les résultats ›</a>");
    viewAll->setStyleSheet("font-size: 12px; background: transparent; border: none; cursor: pointer;");
    viewAll->setTextFormat(Qt::RichText);
    connect(viewAll, &QLabel::linkActivated, [this]() {
        if (sidebarBtnGroup && sidebarBtnGroup->button(3)) {
            sidebarBtnGroup->button(3)->setChecked(true);
            onSidebarBtnClicked(3);
        }
    });

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(viewAll);
    layout->addLayout(headerLayout);

    // Load real transferred notes for this student
    std::vector<Professeur> profsList = chargerProfesseurs();
    struct NoteAffichee {
        QString matiere;
        QString prof;
        double note;
    };
    std::vector<NoteAffichee> notesTransferees;

    for (const auto &p : profsList) {
        for (const auto &n : p.getNotes()) {
            if (n.idEtudiant == currentCompte.getId() && n.statut == "Transférée") {
                NoteAffichee na;
                na.matiere = QString::fromStdString(n.matiere);
                na.prof = QString("Prof. %1 %2").arg(QString::fromStdString(p.getPrenom()), QString::fromStdString(p.getNom()));
                na.note = n.valeur;
                notesTransferees.push_back(na);
            }
        }
    }

    if (notesTransferees.empty()) {
        QLabel *emptyLbl = new QLabel("📋 Aucun résultat disponible pour le moment.\nLes notes publiées par vos professeurs apparaîtront ici dès leur validation administrative.");
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet("font-size: 13px; color: #718096; padding: 30px 15px; background-color: #f8fafc; border-radius: 8px; border: 1px dashed #cbd5e0;");
        layout->addWidget(emptyLbl);
    } else {
        int count = std::min((int)notesTransferees.size(), 5);
        QTableWidget *table = new QTableWidget(count, 4);
        table->setHorizontalHeaderLabels({"Matière", "Enseignant", "Score", "Mention"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setShowGrid(false);
        table->setAlternatingRowColors(true);
        table->setStyleSheet(
            "QTableWidget { background-color: white; border: none; color: #333; font-size: 12px; }"
            "QTableWidget::item { padding: 8px; border-bottom: 1px solid #f0f0f0; color: #333; }"
            "QTableWidget::item:alternate { background-color: #fafbfc; }"
            "QHeaderView::section { background-color: #f8f9fa; color: #666; font-weight: bold; "
            "  border: none; border-bottom: 2px solid #e0e0e0; padding: 8px; font-size: 11px; }"
        );

        for (int r = 0; r < count; ++r) {
            const auto &item = notesTransferees[r];
            auto *subItem = new QTableWidgetItem(item.matiere);
            subItem->setForeground(QColor("#2c3e50"));
            subItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
            table->setItem(r, 0, subItem);

            auto *profItem = new QTableWidgetItem(item.prof);
            profItem->setForeground(QColor("#4a5568"));
            table->setItem(r, 1, profItem);

            auto *scoreItem = new QTableWidgetItem(QString::number(item.note, 'f', 1) + " / 100");
            scoreItem->setTextAlignment(Qt::AlignCenter);
            scoreItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
            scoreItem->setForeground(QColor(item.note >= 70 ? "#27ae60" : (item.note >= 50 ? "#f39c12" : "#e74c3c")));
            table->setItem(r, 2, scoreItem);

            QString mention = item.note >= 90 ? "A+ (Très Bien)" : (item.note >= 80 ? "A (Bien)" : (item.note >= 70 ? "B (Assez Bien)" : (item.note >= 60 ? "C (Passable)" : (item.note >= 50 ? "D (Admis)" : "E (Ajourné)"))));
            QString mentionColor = item.note >= 80 ? "#27ae60" : (item.note >= 50 ? "#3182ce" : "#e74c3c");
            auto *mentionItem = new QTableWidgetItem(mention);
            mentionItem->setTextAlignment(Qt::AlignCenter);
            mentionItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
            mentionItem->setForeground(QColor(mentionColor));
            table->setItem(r, 3, mentionItem);
        }

        table->setFixedHeight(std::min(220, count * 40 + 35));
        layout->addWidget(table);
    }

    return section;
}

// ========== NOTICE BOARD SECTION ==========
QWidget* StudentDashboardWindow::createNoticeBoardSection()
{
    QWidget *section = new QWidget();
    section->setStyleSheet("QWidget { background-color: white; border-radius: 12px; border: 1px solid #e8e8e8; }");
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(12);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("📢 Tableau d'Affichage & Communications");
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
    QPushButton *viewAllBtn = new QPushButton("Voir toutes les annonces ›");
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    viewAllBtn->setStyleSheet("QPushButton { color: #4285F4; font-size: 12px; font-weight: bold; background: transparent; border: none; } QPushButton:hover { text-decoration: underline; }");
    connect(viewAllBtn, &QPushButton::clicked, [this]() {
        if (sidebarBtnGroup && sidebarBtnGroup->button(5)) {
            sidebarBtnGroup->button(5)->setChecked(true);
        }
        stackedWidget->setCurrentIndex(5);
    });

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(viewAllBtn);
    layout->addLayout(headerLayout);

    auto annonces = chargerAnnonces();
    std::reverse(annonces.begin(), annonces.end());

    int count = 0;
    for (const auto &a : annonces) {
        if (a.estConcerne("Etudiants")) {
            count++;
            bool nonLue = !a.estVuPar(currentCompte.getId());

            QWidget *noticeWidget = new QWidget();
            noticeWidget->setStyleSheet("background: transparent; border: none; border-bottom: 1px solid #f0f0f0; padding-bottom: 8px;");
            QVBoxLayout *nLayout = new QVBoxLayout(noticeWidget);
            nLayout->setContentsMargins(0, 5, 0, 5);
            nLayout->setSpacing(3);

            QHBoxLayout *nTitleRow = new QHBoxLayout();
            QLabel *dot = new QLabel("●");
            dot->setStyleSheet(QString("color: %1; font-size: 10px; background: transparent; border: none;").arg(nonLue ? "#e53e3e" : "#3182ce"));
            dot->setFixedWidth(15);
            QLabel *nTitle = new QLabel(QString::fromStdString(a.titre));
            nTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
            nTitleRow->addWidget(dot);
            nTitleRow->addWidget(nTitle, 1);

            QLabel *nDesc = new QLabel(QString::fromStdString(a.contenu));
            nDesc->setStyleSheet("font-size: 11px; color: #7f8c8d; margin-left: 15px; background: transparent; border: none;");
            nDesc->setWordWrap(true);

            QLabel *nDate = new QLabel(QString("📅 %1 • 🏛️ Secrétariat Général").arg(QString::fromStdString(a.dateEnvoi)));
            nDate->setStyleSheet("font-size: 10px; color: #718096; margin-left: 15px; background: transparent; border: none;");

            nLayout->addLayout(nTitleRow);
            nLayout->addWidget(nDesc);
            nLayout->addWidget(nDate);

            layout->addWidget(noticeWidget);
            if (count >= 3) break;
        }
    }

    if (count == 0) {
        QLabel *empty = new QLabel("📭 Aucune annonce pour le moment.");
        empty->setStyleSheet("font-size: 12px; color: #a0aec0; font-style: italic; padding: 15px 0; background: transparent; border: none;");
        empty->setAlignment(Qt::AlignCenter);
        layout->addWidget(empty);
    }

    layout->addStretch();
    return section;
}

// ========== CALENDAR SECTION ==========
QWidget* StudentDashboardWindow::createCalendarSection()
{
    QWidget *section = new QWidget();
    section->setStyleSheet("QWidget { background-color: white; border-radius: 12px; border: 1px solid #e8e8e8; }");
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("Calendrier Académique");
    title->setStyleSheet("font-size: 15px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
    QLabel *viewCal = new QLabel("<a style='color: #4285F4; text-decoration: none;' href='#'>Voir le calendrier ›</a>");
    viewCal->setStyleSheet("font-size: 11px; background: transparent; border: none;");
    viewCal->setTextFormat(Qt::RichText);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(viewCal);
    layout->addLayout(headerLayout);

    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setStyleSheet(
        "QCalendarWidget { background-color: white; border: none; }"
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #4285F4; border-radius: 5px; }"
        "QCalendarWidget QToolButton { color: white; font-weight: bold; font-size: 12px; background: transparent; }"
        "QCalendarWidget QToolButton:hover { background-color: rgba(255,255,255,0.2); border-radius: 3px; }"
        "QCalendarWidget QAbstractItemView { color: #333; selection-background-color: #4285F4; "
        "  selection-color: white; font-size: 11px; background-color: white; }"
        "QCalendarWidget QAbstractItemView:enabled { color: #333; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #ccc; }"
    );
    calendar->setFixedHeight(200);
    layout->addWidget(calendar);

    return section;
}

// ========== QUICK ACCESS SECTION ==========
QWidget* StudentDashboardWindow::createQuickAccessSection()
{
    QWidget *section = new QWidget();
    section->setStyleSheet("QWidget { background-color: white; border-radius: 12px; border: 1px solid #e8e8e8; }");
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QLabel *title = new QLabel("Accès Rapide");
    title->setStyleSheet("font-size: 15px; font-weight: bold; color: #2c3e50; background: transparent; border: none;");
    layout->addWidget(title);

    struct QuickItem { QString icon; QString label; QString color; int pageIndex; };
    QList<QuickItem> items = {
        {"📊", "Résultats", "#E53935", 3},
        {"📚", "Mes Cours", "#43A047", 2},
        {"🗓️", "Emploi du temps", "#FB8C00", 4},
        {"📋", "Présence", "#4285F4", 3},
        {"📥", "Téléchargements", "#9C27B0", 7},
        {"💬", "Support", "#00BCD4", 8}
    };

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(10);
    int row = 0, col = 0;
    for (const auto& item : items) {
        QPushButton *btn = new QPushButton(item.icon + "\n" + item.label);
        btn->setFixedSize(90, 75);
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; border-radius: 10px; "
                    "font-size: 11px; font-weight: bold; border: none; }"
                    "QPushButton:hover { background-color: %1; opacity: 0.9; }").arg(item.color)
        );
        int idx = item.pageIndex;
        connect(btn, &QPushButton::clicked, [this, idx]() {
            stackedWidget->setCurrentIndex(idx);
        });
        grid->addWidget(btn, row, col);
        col++;
        if (col >= 2) { col = 0; row++; }
    }
    layout->addLayout(grid);
    layout->addStretch();

    return section;
}



// ==========================================
// 1. PROFILE PAGE
// ==========================================
QWidget* StudentDashboardWindow::createProfilePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    // Page Title
    QLabel *title = new QLabel("👤 Mon Profil Étudiant");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
    layout->addWidget(title);

    if (!currentCompte.estInscrit()) {
        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    // Header Card
    QWidget *headerCard = new QWidget();
    headerCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
    QHBoxLayout *hLayout = new QHBoxLayout(headerCard);
    hLayout->setContentsMargins(25, 20, 25, 20);
    hLayout->setSpacing(20);

    QLabel *avatar = new QLabel("🎓");
    avatar->setFixedSize(65, 65);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet("background-color: #ebf3fe; border-radius: 32px; font-size: 30px;");
    hLayout->addWidget(avatar);

    QVBoxLayout *nameCol = new QVBoxLayout();
    QLabel *nameLbl = new QLabel(QString::fromStdString(currentCompte.getPrenom() + " " + currentCompte.getNom()));
    nameLbl->setStyleSheet("font-size: 20px; font-weight: bold; color: #1a2b4c; background: transparent;");
    QLabel *facLbl = new QLabel(QString("Faculté : %1").arg(QString::fromStdString(currentCompte.getFaculte())));
    facLbl->setStyleSheet("font-size: 13px; color: #718096; background: transparent;");
    QLabel *idBadge = new QLabel(QString("Matricule : %1").arg(QString::fromStdString(currentCompte.getId())));
    idBadge->setStyleSheet("font-size: 12px; font-weight: bold; color: #2b6cb0; background: transparent;");
    nameCol->addWidget(nameLbl);
    nameCol->addWidget(facLbl);
    nameCol->addWidget(idBadge);
    hLayout->addLayout(nameCol, 1);

    QLabel *statusBadge = new QLabel(currentCompte.estEnPause() ? "⏸️ Études en Pause" : "✅ Inscription Active");
    statusBadge->setStyleSheet(
        QString("background-color: %1; color: white; border-radius: 15px; font-weight: bold; font-size: 12px; padding: 6px 14px;")
        .arg(currentCompte.estEnPause() ? "#d69e2e" : "#38a169")
    );
    hLayout->addWidget(statusBadge);
    layout->addWidget(headerCard);

    // Personal Information Card
    QWidget *infoCard = new QWidget();
    infoCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
    QVBoxLayout *infoCardLayout = new QVBoxLayout(infoCard);
    infoCardLayout->setContentsMargins(25, 20, 25, 20);

    QLabel *infoTitle = new QLabel("📋 Informations Personnelles");
    infoTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a2b4c; background: transparent; margin-bottom: 10px;");
    infoCardLayout->addWidget(infoTitle);

    QGridLayout *infoGrid = new QGridLayout();
    infoGrid->setHorizontalSpacing(30);
    infoGrid->setVerticalSpacing(12);

    auto addInfoRow = [&infoGrid](int row, const QString& label, const QString& val) {
        QLabel *l = new QLabel(label);
        l->setStyleSheet("font-size: 13px; font-weight: bold; color: #718096; background: transparent;");
        QLabel *v = new QLabel(val);
        v->setStyleSheet("font-size: 14px; font-weight: 500; color: #2d3748; background: transparent;");
        infoGrid->addWidget(l, row, 0);
        infoGrid->addWidget(v, row, 1);
    };

    addInfoRow(0, "Matricule Étudiant :", QString::fromStdString(currentCompte.getId()));
    addInfoRow(1, "Nom complet :", QString::fromStdString(currentCompte.getNom() + " " + currentCompte.getPrenom()));
    addInfoRow(2, "Adresse Email :", QString::fromStdString(currentCompte.getEmail()));
    addInfoRow(3, "Numéro de Téléphone :", QString::fromStdString(currentCompte.getTelephone()));
    addInfoRow(4, "Faculté inscrite :", QString::fromStdString(currentCompte.getFaculte()));

    infoCardLayout->addLayout(infoGrid);
    layout->addWidget(infoCard);

    // Registration & Emergency Contact Card
    QWidget *emergencyCard = new QWidget();
    emergencyCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
    QVBoxLayout *emCardLayout = new QVBoxLayout(emergencyCard);
    emCardLayout->setContentsMargins(25, 20, 25, 20);

    QLabel *emTitle = new QLabel("🚨 Contact d'Urgence & Justificatifs");
    emTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a2b4c; background: transparent; margin-bottom: 10px;");
    emCardLayout->addWidget(emTitle);

    QGridLayout *emGrid = new QGridLayout();
    emGrid->setHorizontalSpacing(30);
    emGrid->setVerticalSpacing(12);

    auto addEmRow = [&emGrid](int row, const QString& label, const QString& val, const QString& color = "#2d3748") {
        QLabel *l = new QLabel(label);
        l->setStyleSheet("font-size: 13px; font-weight: bold; color: #718096; background: transparent;");
        QLabel *v = new QLabel(val);
        v->setStyleSheet(QString("font-size: 14px; font-weight: 500; color: %1; background: transparent;").arg(color));
        emGrid->addWidget(l, row, 0);
        emGrid->addWidget(v, row, 1);
    };

    addEmRow(0, "Nom du Contact :", QString::fromStdString(currentCompte.getInscription().prenomProche + " " + currentCompte.getInscription().nomProche));
    addEmRow(1, "Téléphone d'Urgence :", QString::fromStdString(currentCompte.getInscription().telProche));
    addEmRow(2, "Frais d'Inscription :", currentCompte.getInscription().aPaye ? "✅ Payé & Validé" : "⚠️ En attente de paiement", currentCompte.getInscription().aPaye ? "#38a169" : "#e53e3e");
    addEmRow(3, "Pièce d'Identité :", QString::fromStdString(currentCompte.getInscription().pieceIdentitePath.empty() ? "Non fournie" : "Fournie"));

    emCardLayout->addLayout(emGrid);
    layout->addWidget(emergencyCard);

    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ==========================================
// 2. COURSES PAGE
// ==========================================
QWidget* StudentDashboardWindow::createCoursesPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    QLabel *title = new QLabel("📚 Mes Cours & Matières");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
    layout->addWidget(title);

    if (!currentCompte.estInscrit()) {
        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    std::string fac = currentCompte.getFaculte();
    auto it = programmes.find(fac);
    if (it == programmes.end() || it->second.annees.empty()) {
        QLabel *emptyMsg = new QLabel("Aucun programme d'études trouvé pour votre faculté.");
        emptyMsg->setStyleSheet("font-size: 15px; color: #718096; background: transparent;");
        layout->addWidget(emptyMsg);
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    QLabel *facSub = new QLabel(QString("Programme officiel : %1").arg(QString::fromStdString(fac)));
    facSub->setStyleSheet("font-size: 14px; color: #4a5568; background: transparent; font-weight: 500;");
    layout->addWidget(facSub);

    for (const auto& an : it->second.annees) {
        QWidget *card = new QWidget();
        card->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(25, 20, 25, 20);
        cardLayout->setSpacing(12);

        QLabel *yearTitle = new QLabel(QString("📖 Année Académique %1").arg(an.numero));
        yearTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #1a2b4c; background: transparent; border-bottom: 2px solid #edf2f7; padding-bottom: 8px;");
        cardLayout->addWidget(yearTitle);

        QGridLayout *matGrid = new QGridLayout();
        matGrid->setHorizontalSpacing(20);
        matGrid->setVerticalSpacing(10);

        int row = 0;
        for (const auto& mat : an.matieres) {
            QLabel *bullet = new QLabel("🔹");
            bullet->setFixedWidth(20);
            bullet->setStyleSheet("font-size: 12px; background: transparent;");

            QLabel *matLabel = new QLabel(QString::fromStdString(mat));
            matLabel->setStyleSheet("font-size: 14px; font-weight: 500; color: #2d3748; background: transparent;");

            QLabel *creditBadge = new QLabel("3 Crédits");
            creditBadge->setStyleSheet("font-size: 11px; background-color: #ebf8ff; color: #2b6cb0; border-radius: 10px; padding: 3px 8px; font-weight: bold;");
            creditBadge->setAlignment(Qt::AlignCenter);

            matGrid->addWidget(bullet, row, 0);
            matGrid->addWidget(matLabel, row, 1);
            matGrid->addWidget(creditBadge, row, 2, Qt::AlignRight);
            row++;
        }
        cardLayout->addLayout(matGrid);
        layout->addWidget(card);
    }

    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ==========================================
// 3. RESULTS PAGE
// ==========================================
QWidget* StudentDashboardWindow::createResultsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    QLabel *title = new QLabel("📊 Mes Résultats & Relevé de Notes");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
    layout->addWidget(title);

    if (!currentCompte.estInscrit()) {
        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    // Load real transferred notes
    std::vector<Professeur> profsList = chargerProfesseurs();
    struct NoteEtudiant {
        QString matiere;
        QString prof;
        double note;
    };
    std::vector<NoteEtudiant> notesEtudiant;
    double somme = 0.0;
    int countValides = 0;

    for (const auto &p : profsList) {
        for (const auto &n : p.getNotes()) {
            if (n.idEtudiant == currentCompte.getId() && n.statut == "Transférée") {
                NoteEtudiant ne;
                ne.matiere = QString::fromStdString(n.matiere);
                ne.prof = QString("Prof. %1 %2").arg(QString::fromStdString(p.getPrenom()), QString::fromStdString(p.getNom()));
                ne.note = n.valeur;
                notesEtudiant.push_back(ne);
                somme += n.valeur;
                if (n.valeur >= 50.0) countValides++;
            }
        }
    }

    // Real Summary Cards
    QHBoxLayout *statRow = new QHBoxLayout();
    statRow->setSpacing(15);

    QString moyenneStr = notesEtudiant.empty() ? "—" : QString::number(somme / notesEtudiant.size(), 'f', 1) + " / 100";
    QString validesStr = QString("%1 / %2").arg(countValides).arg(notesEtudiant.size());
    QString tauxStr = notesEtudiant.empty() ? "0%" : QString("%1%").arg((int)((countValides * 100.0) / notesEtudiant.size()));

    statRow->addWidget(createStatCard("📈", moyenneStr, "Moyenne Générale", "Notes transférées", "#4285F4"));
    statRow->addWidget(createStatCard("🎖️", validesStr, "Matières Validées", "Seuil de passage: 50/100", "#34A853"));
    statRow->addWidget(createStatCard("🏅", tauxStr, "Taux de Réussite", "Moyenne des matières", "#9C27B0"));
    layout->addLayout(statRow);

    // Full results table card
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
    QVBoxLayout *tCardLayout = new QVBoxLayout(tableCard);
    tCardLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *tTitle = new QLabel("Relevé Détaillé des Notes Transférées");
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a2b4c; background: transparent; margin-bottom: 10px;");
    tCardLayout->addWidget(tTitle);

    if (notesEtudiant.empty()) {
        QLabel *emptyLabel = new QLabel("📋 Aucun résultat disponible pour le moment.\nLes notes saisies par vos enseignants apparaîtront ici dès leur validation et transfert par le secrétariat.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("font-size: 14px; color: #718096; padding: 40px; background-color: #f8fafc; border-radius: 8px; border: 1.5px dashed #cbd5e0;");
        tCardLayout->addWidget(emptyLabel);
    } else {
        QTableWidget *table = new QTableWidget((int)notesEtudiant.size(), 5);
        table->setHorizontalHeaderLabels({"Matière", "Enseignant", "Note (/100)", "Mention", "Statut"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setShowGrid(false);
        table->setAlternatingRowColors(true);
        table->setStyleSheet(
            "QTableWidget { background-color: white; border: none; color: #2d3748; font-size: 13px; }"
            "QTableWidget::item { padding: 10px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
            "QTableWidget::item:alternate { background-color: #f7fafc; }"
            "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
            "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
        );

        for (int r = 0; r < (int)notesEtudiant.size(); ++r) {
            const auto &n = notesEtudiant[r];

            auto *c0 = new QTableWidgetItem(n.matiere);
            c0->setFont(QFont("Segoe UI", 10, QFont::Bold));
            c0->setForeground(QColor("#2d3748"));

            auto *c1 = new QTableWidgetItem(n.prof);
            c1->setForeground(QColor("#4a5568"));

            auto *c2 = new QTableWidgetItem(QString::number(n.note, 'f', 1) + " / 100");
            c2->setTextAlignment(Qt::AlignCenter);
            c2->setFont(QFont("Segoe UI", 10, QFont::Bold));
            c2->setForeground(QColor(n.note >= 70 ? "#27ae60" : (n.note >= 50 ? "#f39c12" : "#e74c3c")));

            QString mention = n.note >= 90 ? "A+ (Très Bien)" : (n.note >= 80 ? "A (Bien)" : (n.note >= 70 ? "B (Assez Bien)" : (n.note >= 60 ? "C (Passable)" : (n.note >= 50 ? "D (Admis)" : "E (Ajourné)"))));
            QString mentionColor = n.note >= 80 ? "#27ae60" : (n.note >= 50 ? "#3182ce" : "#e74c3c");
            auto *c3 = new QTableWidgetItem(mention);
            c3->setTextAlignment(Qt::AlignCenter);
            c3->setFont(QFont("Segoe UI", 10, QFont::Bold));
            c3->setForeground(QColor(mentionColor));

            auto *c4 = new QTableWidgetItem("✅ Transférée");
            c4->setTextAlignment(Qt::AlignCenter);
            c4->setFont(QFont("Segoe UI", 10, QFont::Bold));
            c4->setForeground(QColor("#27ae60"));

            table->setItem(r, 0, c0);
            table->setItem(r, 1, c1);
            table->setItem(r, 2, c2);
            table->setItem(r, 3, c3);
            table->setItem(r, 4, c4);
        }

        table->setFixedHeight(std::min(350, (int)notesEtudiant.size() * 45 + 40));
        tCardLayout->addWidget(table);
    }

    layout->addWidget(tableCard);
    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ==========================================
// 4. TIMETABLE PAGE
// ==========================================
QWidget* StudentDashboardWindow::createTimetablePage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(20);

    QLabel *title = new QLabel("🗓️ Emploi du Temps Hebdomadaire");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
    layout->addWidget(title);

    if (!currentCompte.estInscrit()) {
        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
    QVBoxLayout *tCardLayout = new QVBoxLayout(tableCard);
    tCardLayout->setContentsMargins(20, 20, 20, 20);

    QTableWidget *table = new QTableWidget(4, 5);
    table->setHorizontalHeaderLabels({"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi"});
    table->setVerticalHeaderLabels({"08:00 - 10:00", "10:15 - 12:15", "13:30 - 15:30", "15:45 - 17:45"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setStyleSheet(
        "QTableWidget { background-color: white; border: none; color: #2d3748; font-size: 12px; }"
        "QTableWidget::item { padding: 8px; border: 1px solid #edf2f7; color: #2d3748; border-radius: 6px; }"
        "QHeaderView::section { background-color: #0b1e36; color: white; font-weight: bold; "
        "  padding: 10px; font-size: 12px; border: 1px solid #1a3353; }"
    );

    std::string fac = currentCompte.getFaculte();
    auto it = programmes.find(fac);
    std::vector<std::string> matieres;
    if (it != programmes.end() && !it->second.annees.empty()) {
        matieres = it->second.annees[0].matieres;
    }
    if (matieres.empty()) {
        matieres = {"Mathématiques", "Algorithmique", "Physique", "Architecture Ordinateurs", "Anglais"};
    }

    int idx = 0;
    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row < 4; ++row) {
            QString m = QString::fromStdString(matieres[idx % matieres.size()]);
            QString text = QString("%1\n(Salle %2)").arg(m).arg(101 + (idx % 8));
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(QFont("Segoe UI", 9, QFont::Bold));
            item->setForeground(QColor("#1a365d"));
            item->setBackground(QColor(row % 2 == 0 ? "#ebf8ff" : "#f0fff4"));
            table->setItem(row, col, item);
            idx++;
        }
    }
    table->setFixedHeight(320);
    tCardLayout->addWidget(table);
    layout->addWidget(tableCard);

    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ==========================================
// 5. NOTICE BOARD PAGE
// ==========================================
QWidget* StudentDashboardWindow::createNoticeBoardPage()
{
    if (!currentCompte.estInscrit()) {
        QScrollArea *scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

        QWidget *page = new QWidget();
        page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(35, 30, 35, 30);
        layout->setSpacing(15);

        QLabel *title = new QLabel("📢 Annonces Institutionnelles & Tableau d'Affichage");
        title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
        layout->addWidget(title);

        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    AnnoncesRecues *annoncesWidget = new AnnoncesRecues("Etudiants", currentCompte.getId(), this);
    connect(annoncesWidget, &AnnoncesRecues::lectureEffectuee, this, &StudentDashboardWindow::updateAnnoncesBadge);
    return annoncesWidget;
}

// ==========================================
// 6. ACADEMIC CALENDAR (EVENEMENTS) PAGE
// ==========================================
QWidget* StudentDashboardWindow::createAcademicCalendarPage()
{
    calendrierWidget = new CalendrierScolaire(false, this);
    return calendrierWidget;
}

// ==========================================
// 7. PAYMENT PAGE (PAYER UN VERSEMENT)
// ==========================================
QWidget* StudentDashboardWindow::createPaymentPage()
{
    paiementWidget = new PaiementVersement(PaiementVersement::EtudiantMode,
                                           currentCompte.getId(),
                                           currentCompte.getNom() + " " + currentCompte.getPrenom(),
                                           &comptes, this);
    return paiementWidget;
}

// ==========================================
// 8. DOWNLOADS PAGE
// ==========================================
QWidget* StudentDashboardWindow::createDownloadsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f5f7fa; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #f5f7fa; color: #2c3e50;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(15);

    QLabel *title = new QLabel("📥 Centre de Téléchargements & Documents");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a2b4c; background: transparent;");
    layout->addWidget(title);

    if (!currentCompte.estInscrit()) {
        layout->addWidget(createUnregisteredNoticeCard());
        layout->addStretch();
        scroll->setWidget(page);
        return scroll;
    }

    struct Doc { QString icon; QString name; QString size; QString type; };
    QList<Doc> docs = {
        {"📄", "Règlement Intérieur & Charte de l'Étudiant UDH", "1.2 MB", "PDF"},
        {"📘", "Guide de l'Étudiant & Services Universitaires", "3.4 MB", "PDF"},
        {"📋", "Formulaire de Demande d'Attestation d'Études", "450 KB", "PDF"},
        {"📊", "Programme Complet & Syllabus de la Faculté", "2.1 MB", "PDF"},
        {"📅", "Calendrier Universitaire Officiel 2024-2025", "850 KB", "PDF"}
    };

    for (const auto& d : docs) {
        QWidget *card = new QWidget();
        card->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #e2e8f0;");
        QHBoxLayout *cLayout = new QHBoxLayout(card);
        cLayout->setContentsMargins(20, 15, 20, 15);

        QLabel *ic = new QLabel(d.icon);
        ic->setStyleSheet("font-size: 24px; background: transparent;");
        ic->setFixedWidth(35);

        QVBoxLayout *txtCol = new QVBoxLayout();
        QLabel *n = new QLabel(d.name);
        n->setStyleSheet("font-size: 14px; font-weight: bold; color: #1a2b4c; background: transparent;");
        QLabel *meta = new QLabel(QString("Format : %1 • Taille : %2").arg(d.type).arg(d.size));
        meta->setStyleSheet("font-size: 12px; color: #718096; background: transparent;");
        txtCol->addWidget(n);
        txtCol->addWidget(meta);

        QPushButton *dlBtn = new QPushButton("⬇ Télécharger");
        dlBtn->setFixedSize(130, 36);
        dlBtn->setStyleSheet(
            "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 12px; border: none; }"
            "QPushButton:hover { background-color: #1a3353; }"
        );
        connect(dlBtn, &QPushButton::clicked, [this, d]() {
            QMessageBox::information(this, "Téléchargement", QString("Téléchargement de '%1' réussi.").arg(d.name));
        });

        cLayout->addWidget(ic);
        cLayout->addLayout(txtCol, 1);
        cLayout->addWidget(dlBtn);
        layout->addWidget(card);
    }

    layout->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ==========================================
// 9. SUPPORT (TICKETS) PAGE
// ==========================================
QWidget* StudentDashboardWindow::createSupportPage()
{
    supportWidget = new ModuleSupport(ModuleSupport::UserMode,
                                      currentCompte.getId(),
                                      currentCompte.getNom() + " " + currentCompte.getPrenom(),
                                      "Étudiant", this);
    return supportWidget;
}

// ==========================================
// 9. SETTINGS PAGE (MODULE PARAMÈTRES UNIFIÉ)
// ==========================================
QWidget* StudentDashboardWindow::createSettingsPage()
{
    ParametresCompteBase *sett = new ParametresCompteBase(&currentCompte, comptes, this);
    connect(sett, &ParametresCompteBase::logoutRequested, this, &StudentDashboardWindow::logoutRequested);
    connect(sett, &ParametresCompteBase::profilUpdated, this, &StudentDashboardWindow::updateProfileInfo);
    return sett;
}

// ==========================================
// REGISTRATION COMPLETION HANDLER
// ==========================================
void StudentDashboardWindow::onCompleteRegistrationClicked()
{
    RegistrationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        currentCompte.setInscription(dialog.getInscriptionData());
        currentCompte.setInscriptionFaite(true);
        currentCompte.setActif(true);
        
        enregistrerCompte(comptes, currentCompte);
        QMessageBox::information(this, "Inscription Validée", "🎉 Félicitations ! Votre inscription a été enregistrée avec succès. Tous vos accès sont maintenant débloqués.");
        
        // Reload all pages with fresh registered state
        reloadAllPages();
    }
}

void StudentDashboardWindow::updateProfileInfo()
{
    comptes = chargerComptes();
    for (const auto &c : comptes) {
        if (c.getId() == currentCompte.getId() || (!c.getEmail().empty() && c.getEmail() == currentCompte.getEmail())) {
            currentCompte = c;
            break;
        }
    }
    updateAnnoncesBadge();
}
