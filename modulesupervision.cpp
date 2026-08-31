#include "modulesupervision.h"
#include "gestionreunions.h"
#include "gestioncourrier.h"
#include "paiementversement.h"
#include "modulesupport.h"
#include "annonce.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <map>

ModuleSupervision::ModuleSupervision(QWidget *parent)
    : QWidget(parent), cardStudents(nullptr), cardProfessors(nullptr), cardTotalEmployees(nullptr),
      subjectAveragesTable(nullptr), activityReportTable(nullptr)
{
    setupUI();
    refreshData();
}

void ModuleSupervision::setupUI()
{
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(25, 20, 25, 25);
    mainLayout->setSpacing(20);

    // Title Row
    QLabel *pageTitle = new QLabel("👑 Supervision Haute Direction & Performance Pédagogique", contentWidget);
    pageTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36;");
    mainLayout->addWidget(pageTitle);

    // 3 StatCards Max
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    cardStudents = new StatCard("🎓", "0", "Étudiants Inscrits / Approuvés", "#3182CE", contentWidget);
    cardProfessors = new StatCard("👨‍🏫", "0", "Professeurs Validés", "#805AD5", contentWidget);
    cardTotalEmployees = new StatCard("🏛️", "0", "Total Personnel (Admin + Profs)", "#38A169", contentWidget);

    statsLayout->addWidget(cardStudents);
    statsLayout->addWidget(cardProfessors);
    statsLayout->addWidget(cardTotalEmployees);
    mainLayout->addLayout(statsLayout);

    // Table 1: Moyenne par matière
    QFrame *cardTable1 = new QFrame(contentWidget);
    cardTable1->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow1 = new QGraphicsDropShadowEffect(cardTable1);
    shadow1->setBlurRadius(15);
    shadow1->setColor(QColor(0, 0, 0, 15));
    shadow1->setOffset(0, 4);
    cardTable1->setGraphicsEffect(shadow1);

    QVBoxLayout *t1Layout = new QVBoxLayout(cardTable1);
    t1Layout->setContentsMargins(20, 16, 20, 16);
    t1Layout->setSpacing(12);

    QLabel *t1Title = new QLabel("📚 Performance par Matière & Moyennes Générales Diffusées", cardTable1);
    t1Title->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    t1Layout->addWidget(t1Title);

    subjectAveragesTable = new QTableWidget(cardTable1);
    subjectAveragesTable->setColumnCount(4);
    subjectAveragesTable->setHorizontalHeaderLabels({"Matière / Cours", "Moyenne Générale", "Nombre d'étudiants évalués", "Indicateur Pédagogique"});
    subjectAveragesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    subjectAveragesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    subjectAveragesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    subjectAveragesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    subjectAveragesTable->verticalHeader()->setVisible(false);
    subjectAveragesTable->verticalHeader()->setDefaultSectionSize(38);
    subjectAveragesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    subjectAveragesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    subjectAveragesTable->setAlternatingRowColors(true);
    subjectAveragesTable->setShowGrid(false);
    subjectAveragesTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    t1Layout->addWidget(subjectAveragesTable);
    mainLayout->addWidget(cardTable1);

    // Table 2: Rapport de travail & Activité globale
    QFrame *cardTable2 = new QFrame(contentWidget);
    cardTable2->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow2 = new QGraphicsDropShadowEffect(cardTable2);
    shadow2->setBlurRadius(15);
    shadow2->setColor(QColor(0, 0, 0, 15));
    shadow2->setOffset(0, 4);
    cardTable2->setGraphicsEffect(shadow2);

    QVBoxLayout *t2Layout = new QVBoxLayout(cardTable2);
    t2Layout->setContentsMargins(20, 16, 20, 16);
    t2Layout->setSpacing(12);

    QLabel *t2Title = new QLabel("📋 Rapport de Travail & Synthèse de l'Activité Institutionnelle", cardTable2);
    t2Title->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    t2Layout->addWidget(t2Title);

    activityReportTable = new QTableWidget(cardTable2);
    activityReportTable->setColumnCount(3);
    activityReportTable->setHorizontalHeaderLabels({"Pôle d'activité", "Volume / Décompte Réalisé", "Statut / Évaluation"});
    activityReportTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    activityReportTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    activityReportTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    activityReportTable->verticalHeader()->setVisible(false);
    activityReportTable->verticalHeader()->setDefaultSectionSize(38);
    activityReportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    activityReportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    activityReportTable->setAlternatingRowColors(true);
    activityReportTable->setShowGrid(false);
    activityReportTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    t2Layout->addWidget(activityReportTable);
    mainLayout->addWidget(cardTable2);

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void ModuleSupervision::refreshData()
{
    std::vector<Professeur> profs = chargerProfesseurs();
    std::vector<Administrateur> admins = chargerAdministrateurs();
    std::vector<CompteEtudiant> comptes = chargerComptes();
    std::vector<ReunionComite> reunions = chargerReunionsComite();
    std::vector<Courrier> courriers = chargerCourriers();
    std::vector<VersementEtudiant> versements = chargerVersements();
    std::vector<TicketSupport> tickets = chargerTicketsSupport();
    std::vector<Annonce> annonces = chargerAnnonces();

    // 1. Update 3 Top StatCards
    int nbInscrits = 0;
    for (const auto &c : comptes) {
        if (c.estInscrit() || c.getStatutInscription() == "Approuvé") {
            nbInscrits++;
        }
    }
    if (cardStudents) cardStudents->updateValue(QString::number(nbInscrits));

    int nbProfsValides = 0;
    for (const auto &p : profs) {
        if (p.estValide()) nbProfsValides++;
    }
    if (cardProfessors) cardProfessors->updateValue(QString::number(nbProfsValides));

    int totalEmployes = admins.size() + profs.size();
    if (cardTotalEmployees) cardTotalEmployees->updateValue(QString::number(totalEmployes));

    // 2. Compute Subject Averages
    struct StatsMat {
        double sumNotes = 0.0;
        int count = 0;
    };
    std::map<std::string, StatsMat> mapMatieres;
    int totalNotesAttribuees = 0;

    for (const auto &p : profs) {
        for (const auto &n : p.getNotes()) {
            mapMatieres[n.matiere].sumNotes += n.valeur;
            mapMatieres[n.matiere].count++;
            totalNotesAttribuees++;
        }
    }

    if (subjectAveragesTable) {
        subjectAveragesTable->setRowCount(0);
        int row = 0;

        if (mapMatieres.empty()) {
            subjectAveragesTable->insertRow(0);
            subjectAveragesTable->setItem(0, 0, new QTableWidgetItem("Aucune note enregistrée"));
            subjectAveragesTable->setItem(0, 1, new QTableWidgetItem("N/A"));
            subjectAveragesTable->setItem(0, 2, new QTableWidgetItem("0"));
            subjectAveragesTable->setItem(0, 3, new QTableWidgetItem("En attente d'évaluations"));
        } else {
            for (const auto &pair : mapMatieres) {
                subjectAveragesTable->insertRow(row);

                subjectAveragesTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(pair.first)));

                double avg = (pair.second.count > 0) ? (pair.second.sumNotes / pair.second.count) : 0.0;
                QTableWidgetItem *avgItem = new QTableWidgetItem(QString::number(avg, 'f', 1) + " / 100");
                avgItem->setTextAlignment(Qt::AlignCenter);
                avgItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
                subjectAveragesTable->setItem(row, 1, avgItem);

                QTableWidgetItem *cntItem = new QTableWidgetItem(QString("%1 étudiant(s)").arg(pair.second.count));
                cntItem->setTextAlignment(Qt::AlignCenter);
                subjectAveragesTable->setItem(row, 2, cntItem);

                // Badge indicateur
                QWidget *bWidget = new QWidget();
                QHBoxLayout *bLayout = new QHBoxLayout(bWidget);
                bLayout->setContentsMargins(4, 2, 4, 2);
                bLayout->setAlignment(Qt::AlignCenter);
                QLabel *badge = new QLabel();
                badge->setAlignment(Qt::AlignCenter);
                badge->setFixedWidth(110);
                badge->setFixedHeight(24);

                if (avg >= 75.0) {
                    badge->setText("🌟 Très Satisfaisant");
                    badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 12px; font-size: 11px;");
                } else if (avg >= 60.0) {
                    badge->setText("👍 Conforme");
                    badge->setStyleSheet("background-color: #BEE3F8; color: #2B6CB0; font-weight: bold; border-radius: 12px; font-size: 11px;");
                } else {
                    badge->setText("⚠️ À Surveiller");
                    badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 12px; font-size: 11px;");
                }
                bLayout->addWidget(badge);
                subjectAveragesTable->setCellWidget(row, 3, bWidget);

                row++;
            }
        }
    }

    // 3. Activity Report Table
    if (activityReportTable) {
        activityReportTable->setRowCount(0);

        auto addActivityRow = [this](int r, const QString &pole, const QString &val, const QString &statut, const QString &badgeColor) {
            activityReportTable->insertRow(r);
            activityReportTable->setItem(r, 0, new QTableWidgetItem(pole));

            QTableWidgetItem *vItem = new QTableWidgetItem(val);
            vItem->setTextAlignment(Qt::AlignCenter);
            vItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
            activityReportTable->setItem(r, 1, vItem);

            QWidget *bWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(bWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);
            QLabel *badge = new QLabel(statut);
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(120);
            badge->setFixedHeight(22);
            badge->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 11px; font-size: 11px;").arg(badgeColor));
            bLayout->addWidget(badge);
            activityReportTable->setCellWidget(r, 2, bWidget);
        };

        int rowIdx = 0;
        addActivityRow(rowIdx++, "📝 Évaluations Pédagogiques & Notes Enregistrées", QString("%1 note(s) saisie(s)").arg(totalNotesAttribuees), "Actif", "#3182CE");
        addActivityRow(rowIdx++, "🏛️ Réunions du Comité de Direction Organisées", QString("%1 réunion(s)").arg(reunions.size()), "Suivi Régulier", "#805AD5");
        addActivityRow(rowIdx++, "📬 Courriers, Rapports & Transmissions Diplomatiques", QString("%1 courrier(s) échangé(s)").arg(courriers.size()), "Opérationnel", "#2B6CB0");
        addActivityRow(rowIdx++, "💳 Versements Financiers & Paiements de Semestre", QString("%1 versement(s)").arg(versements.size()), "En Cours", "#38A169");
        addActivityRow(rowIdx++, "🎧 Tickets de Support & Requêtes Étudiants/Profs", QString("%1 ticket(s)").arg(tickets.size()), "Sous Contrôle", "#DD6B20");
        addActivityRow(rowIdx++, "📢 Annonces & Communications Officielles Diffusées", QString("%1 annonce(s)").arg(annonces.size()), "Diffusé", "#319795");
    }
}
