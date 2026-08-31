#include "modulegestionfacultes.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <set>
#include <map>

ModuleGestionFacultes::ModuleGestionFacultes(std::vector<Administrateur> &adminsRef,
                                             std::vector<Professeur> &profsRef,
                                             std::vector<CompteEtudiant> &comptesRef,
                                             QWidget *parent)
    : QWidget(parent), admins(adminsRef), profs(profsRef), comptes(comptesRef), table(nullptr)
{
    setupUI();
    populateTable();
}

void ModuleGestionFacultes::refreshData() { populateTable(); }

void ModuleGestionFacultes::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("🏫  Gestion des facultés");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a202c; background: transparent;");
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("Supervision des facultés, doyens, départements et effectifs académiques.");
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size: 13px; color: #718096; background: transparent; margin-bottom: 6px;");
    mainLayout->addWidget(subtitle);

    /* ---- Table ---- */
    table = new QTableWidget();
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Faculté", "Coordonnateur", "Nb Professeurs", "Nb Étudiants", "Statut"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(48);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->setStyleSheet(
        "QTableWidget { background-color: white; border: 1px solid #e2e8f0; border-radius: 12px; font-size: 13px; }"
        "QHeaderView::section { background-color: #f7fafc; color: #4a5568; font-weight: bold; font-size: 12px; padding: 10px; border: none; border-bottom: 2px solid #e2e8f0; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f9fafb; }"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(18);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 3);
    table->setGraphicsEffect(shadow);

    mainLayout->addWidget(table, 1);
}

void ModuleGestionFacultes::populateTable()
{
    table->setRowCount(0);

    // Collect all faculties
    std::set<std::string> facSet;
    QStringList defaultFacs = {"Sciences Informatiques", "Médecine", "Droit", "Gestion", "Sciences de l'Éducation", "Agronomie"};
    for (const auto &f : defaultFacs) facSet.insert(f.toStdString());
    for (const auto &p : profs) {
        if (!p.getFaculte().empty()) facSet.insert(p.getFaculte());
    }
    for (const auto &a : admins) {
        if (!a.getFaculte().empty()) facSet.insert(a.getFaculte());
    }

    // Count profs per faculty
    std::map<std::string, int> profsPerFac;
    for (const auto &p : profs) {
        if (!p.getFaculte().empty()) profsPerFac[p.getFaculte()]++;
    }

    // Count students per faculty
    std::map<std::string, int> studentsPerFac;
    for (const auto &c : comptes) {
        if (!c.getFaculte().empty()) studentsPerFac[c.getFaculte()]++;
    }

    // Find coordinators
    std::map<std::string, std::string> coordPerFac;
    for (const auto &a : admins) {
        if (a.getPoste().find("Coordonnateur") != std::string::npos && !a.getFaculte().empty()) {
            coordPerFac[a.getFaculte()] = a.getNom() + " " + a.getPrenom();
        }
    }

    int row = 0;
    for (const auto &fac : facSet) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(fac)));

        QString coord = "— Non assigné —";
        if (coordPerFac.count(fac)) coord = QString::fromStdString(coordPerFac[fac]);
        table->setItem(row, 1, new QTableWidgetItem(coord));

        int nbProfs = profsPerFac.count(fac) ? profsPerFac[fac] : 0;
        int nbEtudiants = studentsPerFac.count(fac) ? studentsPerFac[fac] : 0;
        table->setItem(row, 2, new QTableWidgetItem(QString::number(nbProfs)));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(nbEtudiants)));

        // Statut badge
        bool hasCoord = coordPerFac.count(fac) > 0;
        QString statut = hasCoord ? "Opérationnel" : "Sans coordonnateur";
        QLabel *badge = new QLabel(statut);
        badge->setAlignment(Qt::AlignCenter);
        QString bc = hasCoord ? "background-color: #c6f6d5; color: #276749;" : "background-color: #fefcbf; color: #975a16;";
        badge->setStyleSheet(QString("font-size: 11px; font-weight: bold; border-radius: 10px; padding: 4px 12px; %1").arg(bc));
        QWidget *bw = new QWidget();
        QHBoxLayout *bl = new QHBoxLayout(bw);
        bl->setContentsMargins(4, 2, 4, 2);
        bl->addWidget(badge, 0, Qt::AlignCenter);
        table->setCellWidget(row, 4, bw);

        row++;
    }
}
