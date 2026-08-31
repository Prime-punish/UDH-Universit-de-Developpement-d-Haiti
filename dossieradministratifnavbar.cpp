#include "dossieradministratifnavbar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>
#include <set>

DossierAdministratifNavBar::DossierAdministratifNavBar(std::vector<CompteEtudiant> &comptesRef,
                                                     std::vector<Professeur> &profsRef,
                                                     QWidget *parent)
    : QWidget(parent), comptes(comptesRef), profs(&profsRef),
      tabWidget(nullptr), paiementWidget(nullptr), examensWidget(nullptr),
      supportWidget(nullptr), rapportWidget(nullptr),
      lblTotalExamens(nullptr), lblTotalFacultes(nullptr), lblTotalProfs(nullptr),
      examensSearchEdit(nullptr), examensFaculteCombo(nullptr), examensTable(nullptr)
{
    setupUI();
}

DossierAdministratifNavBar::DossierAdministratifNavBar(std::vector<CompteEtudiant> &comptesRef,
                                                     QWidget *parent)
    : QWidget(parent), comptes(comptesRef), profs(nullptr),
      tabWidget(nullptr), paiementWidget(nullptr), examensWidget(nullptr),
      supportWidget(nullptr), rapportWidget(nullptr),
      lblTotalExamens(nullptr), lblTotalFacultes(nullptr), lblTotalProfs(nullptr),
      examensSearchEdit(nullptr), examensFaculteCombo(nullptr), examensTable(nullptr)
{
    setupUI();
}

void DossierAdministratifNavBar::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 15);
    mainLayout->setSpacing(10);

    // Horizontal Modern Tab Widget
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; background: #ffffff; border-radius: 10px; top: -1px; }"
        "QTabBar::tab { background: #edf2f7; color: #4a5568; padding: 12px 24px; font-weight: bold; font-size: 13px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 6px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0b1e36; border-bottom: 3px solid #0b1e36; font-size: 14px; }"
        "QTabBar::tab:hover:!selected { background: #e2e8f0; color: #1a202c; }"
    );

    paiementWidget = new PaiementVersement(PaiementVersement::SecretaireMode, "", "", &comptes, this);
    examensWidget = createExamensPage();
    supportWidget = new ModuleSupport(ModuleSupport::AdminMode, "sec_1", "Secrétaire Général", "Secrétaire", this);
    rapportWidget = new ModuleRapportGraphique(this);

    tabWidget->addTab(paiementWidget, "💳 Paiements & Versements");
    tabWidget->addTab(examensWidget, "📝 Épreuves & Dossiers d'Examens");
    tabWidget->addTab(supportWidget, "🎧 Tickets de Support");
    tabWidget->addTab(rapportWidget, "📊 Rapports & Synthèse");

    mainLayout->addWidget(tabWidget);

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 0 && paiementWidget) paiementWidget->refreshData();
        else if (index == 1) refreshExamensData();
        else if (index == 2 && supportWidget) supportWidget->refreshData();
        else if (index == 3 && rapportWidget) rapportWidget->refreshData();
    });

    refreshExamensData();
}

QWidget* DossierAdministratifNavBar::createExamensPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(16);

    // Titre & description
    QLabel *title = new QLabel("📁   Dossiers & Épreuves d'Examens Téléversés");
    title->setStyleSheet("font-size: 19px; font-weight: bold; color: #0b1e36;");
    layout->addWidget(title);

    QLabel *desc = new QLabel("Ce module centralise l'ensemble des sujets d'examens et copies numériques téléversés par les professeurs de toutes les facultés.");
    desc->setStyleSheet("font-size: 13px; color: #718096;");
    layout->addWidget(desc);

    // 1. Cartes statistiques
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(15);

    auto createStatCard = [](const QString &icon, const QString &label, QLabel *&valLbl, const QString &color) -> QFrame* {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString("QFrame { background-color: #ffffff; border-radius: 10px; border: 1px solid #e2e8f0; border-left: 4px solid %1; }").arg(color));
        QHBoxLayout *l = new QHBoxLayout(card);
        l->setContentsMargins(18, 14, 18, 14);
        l->setSpacing(12);

        QLabel *ic = new QLabel(icon);
        ic->setStyleSheet("font-size: 26px;");
        l->addWidget(ic);

        QVBoxLayout *vl = new QVBoxLayout();
        vl->setSpacing(2);
        valLbl = new QLabel("0");
        valLbl->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(color));
        QLabel *lb = new QLabel(label);
        lb->setStyleSheet("font-size: 12px; color: #718096; font-weight: 500;");
        vl->addWidget(valLbl);
        vl->addWidget(lb);
        l->addLayout(vl);
        return card;
    };

    statsLayout->addWidget(createStatCard("📄", "Épreuves / Documents reçus", lblTotalExamens, "#2b6cb0"));
    statsLayout->addWidget(createStatCard("🏛️", "Facultés représentées", lblTotalFacultes, "#27ae60"));
    statsLayout->addWidget(createStatCard("👨‍🏫", "Professeurs déposants", lblTotalProfs, "#d69e2e"));
    layout->addLayout(statsLayout);

    // 2. Barre de filtres et recherche
    QFrame *filterCard = new QFrame();
    filterCard->setStyleSheet("QFrame { background-color: #f8fafc; border-radius: 8px; border: 1px solid #e2e8f0; }");
    QHBoxLayout *fLayout = new QHBoxLayout(filterCard);
    fLayout->setContentsMargins(15, 10, 15, 10);
    fLayout->setSpacing(12);

    QLabel *sIcon = new QLabel("🔍");
    fLayout->addWidget(sIcon);

    examensSearchEdit = new QLineEdit();
    examensSearchEdit->setPlaceholderText("Rechercher par enseignant, matière, ID ou nom de fichier...");
    examensSearchEdit->setFixedHeight(36);
    examensSearchEdit->setStyleSheet(
        "QLineEdit { background: #ffffff; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px 12px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; }"
    );
    connect(examensSearchEdit, &QLineEdit::textChanged, this, &DossierAdministratifNavBar::onExamenSearchChanged);
    fLayout->addWidget(examensSearchEdit, 1);

    examensFaculteCombo = new QComboBox();
    examensFaculteCombo->setFixedHeight(36);
    examensFaculteCombo->addItem("🏛️ Toutes les facultés");
    examensFaculteCombo->addItem("Génie civil");
    examensFaculteCombo->addItem("Génie informatique");
    examensFaculteCombo->addItem("Comptabilité et économie");
    examensFaculteCombo->addItem("Administration");
    examensFaculteCombo->addItem("Science juridique");
    examensFaculteCombo->addItem("Éducation");
    examensFaculteCombo->setStyleSheet(
        "QComboBox { background: #ffffff; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 4px 10px; font-size: 13px; font-weight: 500; min-width: 190px; }"
        "QComboBox::drop-down { border: none; }"
    );
    connect(examensFaculteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DossierAdministratifNavBar::onExamenFaculteChanged);
    fLayout->addWidget(examensFaculteCombo);

    QPushButton *refreshBtn = new QPushButton("🔄 Actualiser");
    refreshBtn->setFixedHeight(36);
    refreshBtn->setStyleSheet(
        "QPushButton { background: #0b1e36; color: #ffffff; border-radius: 6px; font-weight: bold; font-size: 12px; padding: 0 14px; border: none; }"
        "QPushButton:hover { background: #1a3353; }"
    );
    connect(refreshBtn, &QPushButton::clicked, this, &DossierAdministratifNavBar::refreshExamensData);
    fLayout->addWidget(refreshBtn);

    layout->addWidget(filterCard);

    // 3. Tableau des épreuves
    examensTable = new QTableWidget();
    examensTable->setColumnCount(7);
    examensTable->setHorizontalHeaderLabels({"ID Prof", "Enseignant", "Faculté", "Matière / Épreuve", "Date d'ajout", "Nom du fichier", "Actions"});
    examensTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    examensTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    examensTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    examensTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    examensTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    examensTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    examensTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    examensTable->verticalHeader()->setVisible(false);
    examensTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    examensTable->setAlternatingRowColors(true);
    examensTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; alternate-background-color: #f8fafc; border: 1px solid #e2e8f0; border-radius: 8px; font-size: 13px; }"
        "QHeaderView::section { background-color: #0b1e36; color: white; font-weight: bold; padding: 10px; border: none; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:selected { background-color: #ebf8ff; color: #2b6cb0; }"
    );

    layout->addWidget(examensTable, 1);
    return page;
}

void DossierAdministratifNavBar::refreshExamensData()
{
    if (!examensTable) return;

    std::vector<Professeur> allProfs;
    if (profs && !profs->empty()) {
        allProfs = *profs;
    } else {
        allProfs = chargerProfesseurs();
    }

    QString search = examensSearchEdit ? examensSearchEdit->text().trimmed().toLower() : "";
    QString selectedFac = examensFaculteCombo ? examensFaculteCombo->currentText() : "🏛️ Toutes les facultés";

    struct ExamRecord {
        std::string profId;
        std::string profNom;
        std::string profFaculte;
        std::string matiere;
        std::string dateAjout;
        std::string chemin;
    };

    std::vector<ExamRecord> list;
    std::set<std::string> distinctFacultes;
    std::set<std::string> distinctProfs;
    int totalDocs = 0;

    for (const auto &p : allProfs) {
        for (const auto &ex : p.getPhotosExamens()) {
            totalDocs++;
            distinctFacultes.insert(p.getFaculte());
            distinctProfs.insert(p.getId());

            // Filtre par faculté
            if (selectedFac != "🏛️ Toutes les facultés" && !selectedFac.isEmpty()) {
                if (QString::fromStdString(p.getFaculte()).trimmed().toLower() != selectedFac.trimmed().toLower()) {
                    continue;
                }
            }

            // Filtre par recherche
            if (!search.isEmpty()) {
                QString fullStr = QString("%1 %2 %3 %4 %5 %6")
                                  .arg(QString::fromStdString(p.getId()))
                                  .arg(QString::fromStdString(p.getNom()))
                                  .arg(QString::fromStdString(p.getPrenom()))
                                  .arg(QString::fromStdString(p.getFaculte()))
                                  .arg(QString::fromStdString(ex.matiere))
                                  .arg(QString::fromStdString(ex.cheminFichier)).toLower();
                if (!fullStr.contains(search)) {
                    continue;
                }
            }

            list.push_back({
                p.getId(),
                p.getNom() + " " + p.getPrenom(),
                p.getFaculte(),
                ex.matiere,
                ex.dateAjout,
                ex.cheminFichier
            });
        }
    }

    // Mise à jour des cartes statistiques
    if (lblTotalExamens) lblTotalExamens->setText(QString::number(totalDocs));
    if (lblTotalFacultes) lblTotalFacultes->setText(QString::number(distinctFacultes.size()));
    if (lblTotalProfs) lblTotalProfs->setText(QString::number(distinctProfs.size()));

    // Remplissage du tableau
    examensTable->setRowCount((int)list.size());
    for (int i = 0; i < (int)list.size(); ++i) {
        const auto &item = list[i];

        QTableWidgetItem *itId = new QTableWidgetItem(QString::fromStdString(item.profId));
        itId->setTextAlignment(Qt::AlignCenter);
        itId->setFlags(itId->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 0, itId);

        QTableWidgetItem *itNom = new QTableWidgetItem(QString::fromStdString(item.profNom));
        itNom->setFlags(itNom->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 1, itNom);

        QTableWidgetItem *itFac = new QTableWidgetItem(QString::fromStdString(item.profFaculte));
        itFac->setFlags(itFac->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 2, itFac);

        QTableWidgetItem *itMat = new QTableWidgetItem(QString::fromStdString(item.matiere));
        itMat->setFlags(itMat->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 3, itMat);

        QTableWidgetItem *itDate = new QTableWidgetItem(QString::fromStdString(item.dateAjout.empty() ? "—" : item.dateAjout));
        itDate->setTextAlignment(Qt::AlignCenter);
        itDate->setFlags(itDate->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 4, itDate);

        QFileInfo fi(QString::fromStdString(item.chemin));
        QString fileName = fi.fileName().isEmpty() ? QString::fromStdString(item.chemin) : fi.fileName();
        QTableWidgetItem *itFile = new QTableWidgetItem("📎 " + fileName);
        itFile->setToolTip(QString::fromStdString(item.chemin));
        itFile->setFlags(itFile->flags() ^ Qt::ItemIsEditable);
        examensTable->setItem(i, 5, itFile);

        // Actions Widget
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *aLayout = new QHBoxLayout(actionWidget);
        aLayout->setContentsMargins(4, 2, 4, 2);
        aLayout->setSpacing(6);

        QPushButton *openBtn = new QPushButton("👁️ Consulter");
        openBtn->setStyleSheet(
            "QPushButton { background-color: #2b6cb0; color: white; border-radius: 5px; font-weight: bold; font-size: 11px; padding: 4px 10px; border: none; }"
            "QPushButton:hover { background-color: #1a4971; }"
        );
        QString cheminStr = QString::fromStdString(item.chemin);
        connect(openBtn, &QPushButton::clicked, this, [this, cheminStr]() {
            onOuvrirFichierExamen(cheminStr);
        });
        aLayout->addWidget(openBtn);

        QPushButton *folderBtn = new QPushButton("📂");
        folderBtn->setToolTip("Ouvrir l'emplacement du dossier");
        folderBtn->setStyleSheet(
            "QPushButton { background-color: #edf2f7; color: #4a5568; border: 1px solid #cbd5e0; border-radius: 5px; font-size: 11px; padding: 4px 8px; }"
            "QPushButton:hover { background-color: #e2e8f0; }"
        );
        connect(folderBtn, &QPushButton::clicked, this, [cheminStr]() {
            QFileInfo info(cheminStr);
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
        });
        aLayout->addWidget(folderBtn);

        examensTable->setCellWidget(i, 6, actionWidget);
        examensTable->setRowHeight(i, 44);
    }
}

void DossierAdministratifNavBar::onExamenSearchChanged()
{
    refreshExamensData();
}

void DossierAdministratifNavBar::onExamenFaculteChanged()
{
    refreshExamensData();
}

void DossierAdministratifNavBar::onOuvrirFichierExamen(const QString &chemin)
{
    if (chemin.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Document introuvable", "Aucun chemin de fichier n'est renseigné pour cette épreuve.");
        return;
    }

    QFileInfo info(chemin);
    if (!info.exists()) {
        QMessageBox::information(this, "Information du document",
            QString("Chemin enregistré : %1\n\n(Le fichier physique n'a pas été trouvé à cet emplacement spécifique sur votre disque).").arg(chemin));
        return;
    }

    bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
    if (!success) {
        QMessageBox::warning(this, "Erreur d'ouverture",
            QString("Impossible d'ouvrir le fichier : %1").arg(chemin));
    }
}

void DossierAdministratifNavBar::refreshData()
{
    if (paiementWidget) paiementWidget->refreshData();
    refreshExamensData();
    if (supportWidget) supportWidget->refreshData();
    if (rapportWidget) rapportWidget->refreshData();
}
