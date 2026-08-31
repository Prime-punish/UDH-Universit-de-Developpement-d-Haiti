#include "validationinscriptions.h"
#include "fichedetailinscription.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>

ValidationInscriptions::ValidationInscriptions(std::vector<CompteEtudiant> &comptesRef, QWidget *parent)
    : QWidget(parent), comptes(comptesRef)
{
    setupUI();
}

void ValidationInscriptions::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(20);

    // 3 Stat Cards Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    cardPending  = new StatCard("⏳", "0", "Inscriptions en attente", "#E67E22", this);
    cardApproved = new StatCard("✅", "0", "Inscriptions approuvées", "#27AE60", this);
    cardRejected = new StatCard("❌", "0", "Inscriptions rejetées", "#E74C3C", this);

    statsLayout->addWidget(cardPending);
    statsLayout->addWidget(cardApproved);
    statsLayout->addWidget(cardRejected);
    mainLayout->addLayout(statsLayout);

    // Card Frame for Table
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(15);

    // Header Controls: Title & Filter QComboBox
    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *title = new QLabel("Validation des Inscriptions Étudiantes", tableCard);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #1a202c; border: none;");

    filterCombo = new QComboBox(tableCard);
    filterCombo->setFixedWidth(180);
    filterCombo->setFixedHeight(38);
    filterCombo->setStyleSheet(
        "QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding-left: 10px; font-weight: bold; font-size: 13px; }"
        "QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
    );
    filterCombo->addItems({"En attente", "Approuvées", "Rejetées", "Toutes"});
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ValidationInscriptions::onFilterChanged);

    headerRow->addWidget(title);
    headerRow->addStretch();
    headerRow->addWidget(new QLabel("Filtre :", tableCard));
    headerRow->addWidget(filterCombo);
    tcLayout->addLayout(headerRow);

    // Table Widget
    requestsTable = new QTableWidget(tableCard);
    requestsTable->setColumnCount(6);
    requestsTable->setHorizontalHeaderLabels({"ID Étudiant", "Nom / Prénom", "Faculté / Filière", "Date de soumission", "Statut", "Action"});
    requestsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    requestsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    requestsTable->verticalHeader()->setVisible(false);
    requestsTable->verticalHeader()->setDefaultSectionSize(38);
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestsTable->setAlternatingRowColors(true);
    requestsTable->setShowGrid(false);
    requestsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );

    tcLayout->addWidget(requestsTable);
    mainLayout->addWidget(tableCard);

    refreshData();
}

void ValidationInscriptions::refreshData()
{
    populateTable();
}

void ValidationInscriptions::onFilterChanged(int)
{
    populateTable();
}

void ValidationInscriptions::populateTable()
{
    if (!requestsTable) return;

    requestsTable->setRowCount(0);
    rowMappings.clear();

    int filter = filterCombo->currentIndex(); // 0: En attente, 1: Approuvées, 2: Rejetées, 3: Toutes

    int pendingCount = 0;
    int approvedCount = 0;
    int rejectedCount = 0;

    for (size_t i = 0; i < comptes.size(); ++i) {
        const auto &c = comptes[i];
        std::string st = c.getStatutInscription();

        if (st == "Approuvé") approvedCount++;
        else if (st == "Rejeté") rejectedCount++;
        else pendingCount++;

        bool matchesFilter = false;
        if (filter == 0 && (st == "En attente" || st.empty())) matchesFilter = true;
        else if (filter == 1 && st == "Approuvé") matchesFilter = true;
        else if (filter == 2 && st == "Rejeté") matchesFilter = true;
        else if (filter == 3) matchesFilter = true;

        if (matchesFilter) {
            int r = requestsTable->rowCount();
            requestsTable->insertRow(r);

            QString idStr = QString::fromStdString(c.getId());
            QString nameStr = QString("%1 %2").arg(QString::fromStdString(c.getPrenom()), QString::fromStdString(c.getNom()));
            QString facStr = QString::fromStdString(c.getFaculte().empty() ? "Non spécifiée" : c.getFaculte());
            QString dateStr = QString::fromStdString(c.getDateSoumission().empty() ? "N/A" : c.getDateSoumission());

            requestsTable->setItem(r, 0, new QTableWidgetItem(idStr));
            requestsTable->setItem(r, 1, new QTableWidgetItem(nameStr));
            requestsTable->setItem(r, 2, new QTableWidgetItem(facStr));
            requestsTable->setItem(r, 3, new QTableWidgetItem(dateStr));

            QString badgeText = st == "Approuvé" ? "✅ Approuvé" : (st == "Rejeté" ? "❌ Rejeté" : "⏳ En attente");
            QString badgeColor = st == "Approuvé" ? "#27ae60" : (st == "Rejeté" ? "#e74c3c" : "#e67e22");

            QTableWidgetItem *stItem = new QTableWidgetItem(badgeText);
            stItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
            stItem->setForeground(QColor(badgeColor));
            requestsTable->setItem(r, 4, stItem);

            QPushButton *viewBtn = new QPushButton("👁️ Voir le dossier");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet(
                "QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; border: none; }"
                "QPushButton:hover { background-color: #1a3353; }"
            );

            int origIdx = (int)i;
            connect(viewBtn, &QPushButton::clicked, [this, origIdx]() {
                onViewDossier(origIdx);
            });

            requestsTable->setCellWidget(r, 5, viewBtn);

            TableRowMapping mapItem;
            mapItem.originalIndex = origIdx;
            rowMappings.push_back(mapItem);
        }
    }

    cardPending->updateValue(QString::number(pendingCount));
    cardApproved->updateValue(QString::number(approvedCount));
    cardRejected->updateValue(QString::number(rejectedCount));
}

void ValidationInscriptions::onViewDossier(int originalIndex)
{
    if (originalIndex < 0 || originalIndex >= (int)comptes.size()) return;

    FicheDetailInscription dialog(comptes[originalIndex], comptes, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshData();
    }
}
