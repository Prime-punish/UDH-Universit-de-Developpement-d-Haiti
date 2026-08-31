#include "gestionsalaires.h"
#include "tresorerie.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QDialog>
#include <QLabel>
#include <QPushButton>

GestionSalaires::GestionSalaires(std::vector<Professeur> &profsRef, std::vector<Administrateur> &adminsRef, QWidget *parent)
    : QWidget(parent), profs(profsRef), admins(adminsRef),
      cardPaid(nullptr), cardUnpaid(nullptr), cardTotal(nullptr), cardSolde(nullptr),
      filterCombo(nullptr), salaryTable(nullptr)
{
    synchroniserJournalTresorerieSiVide();
    setupUI();
}

void GestionSalaires::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(20);

    // 4 Stat Cards Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    cardPaid   = new StatCard("✅", "0", "Payés ce mois", "#34A853", this);
    cardUnpaid = new StatCard("⏳", "0", "Non payés", "#EA4335", this);
    cardTotal  = new StatCard("💰", "100%", "Masse Traitée", "#9C27B0", this);
    cardSolde  = new StatCard("🏦", "0 HTG", "Solde Disponible", "#0D9488", this);

    statsLayout->addWidget(cardPaid);
    statsLayout->addWidget(cardUnpaid);
    statsLayout->addWidget(cardTotal);
    statsLayout->addWidget(cardSolde);
    mainLayout->addLayout(statsLayout);

    // Main Card Frame for Table & Actions
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

    // Header Controls Row: Title, Journal de caisse button, Filter ComboBox, Mark All Paid Button
    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *title = new QLabel("Gestion des Salaires du Personnel", tableCard);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #1a202c; border: none;");

    filterCombo = new QComboBox(tableCard);
    filterCombo->setFixedWidth(170);
    filterCombo->setFixedHeight(38);
    filterCombo->setStyleSheet(
        "QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding-left: 10px; font-weight: bold; font-size: 13px; }"
        "QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
    );
    filterCombo->addItems({"Tous", "Professeurs", "Administrateurs"});
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GestionSalaires::onFilterChanged);

    QPushButton *journalBtn = new QPushButton("📜 Journal de Caisse", tableCard);
    journalBtn->setFixedHeight(38);
    journalBtn->setCursor(Qt::PointingHandCursor);
    journalBtn->setStyleSheet(
        "QPushButton { background-color: #0D9488; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 14px; border: none; }"
        "QPushButton:hover { background-color: #0F766E; }"
    );
    connect(journalBtn, &QPushButton::clicked, this, &GestionSalaires::onViewJournalTresorerieClicked);

    QPushButton *markAllBtn = new QPushButton("⚡ Marquer tous comme payés", tableCard);
    markAllBtn->setFixedHeight(38);
    markAllBtn->setCursor(Qt::PointingHandCursor);
    markAllBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(markAllBtn, &QPushButton::clicked, this, &GestionSalaires::onMarkAllPaidClicked);

    headerRow->addWidget(title);
    headerRow->addStretch();
    headerRow->addWidget(journalBtn);
    headerRow->addSpacing(8);
    headerRow->addWidget(new QLabel("Filtre :", tableCard));
    headerRow->addWidget(filterCombo);
    headerRow->addSpacing(8);
    headerRow->addWidget(markAllBtn);

    tcLayout->addLayout(headerRow);

    // Table Widget
    salaryTable = new QTableWidget(tableCard);
    salaryTable->setColumnCount(7);
    salaryTable->setHorizontalHeaderLabels({"ID", "Nom / Prénom", "Poste / Matières", "Type", "Montant", "Statut", "Action"});
    salaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    salaryTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    salaryTable->verticalHeader()->setVisible(false);
    salaryTable->verticalHeader()->setDefaultSectionSize(38);
    salaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    salaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    salaryTable->setAlternatingRowColors(true);
    salaryTable->setShowGrid(false);
    salaryTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );

    tcLayout->addWidget(salaryTable);
    mainLayout->addWidget(tableCard);

    refreshData();
}

void GestionSalaires::refreshData()
{
    populateTable();
}

void GestionSalaires::onFilterChanged(int)
{
    populateTable();
}

void GestionSalaires::populateTable()
{
    if (!salaryTable) return;

    salaryTable->setRowCount(0);

    int filter = filterCombo ? filterCombo->currentIndex() : 0; // 0: Tous, 1: Professeurs, 2: Administrateurs
    int paidCount = 0;
    int unpaidCount = 0;

    QString todayStr = QDateTime::currentDateTime().toString("dd/MM/yyyy");

    struct RowData {
        QString id;
        QString name;
        QString role;
        QString type;
        double amount;
        bool paid;
        QString date;
        bool isProf;
        int originalIndex;
    };

    std::vector<RowData> rows;

    if (filter == 0 || filter == 1) { // Profs
        for (size_t i = 0; i < profs.size(); ++i) {
            const auto &p = profs[i];
            RowData r;
            r.id = QString::fromStdString(p.getId());
            r.name = QString("Prof. %1 %2").arg(QString::fromStdString(p.getPrenom()), QString::fromStdString(p.getNom()));
            QString mStr = "";
            for (size_t k = 0; k < p.getMatieres().size(); ++k) {
                mStr += QString::fromStdString(p.getMatieres()[k]);
                if (k + 1 < p.getMatieres().size()) mStr += ", ";
            }
            if (mStr.isEmpty()) mStr = QString::fromStdString(p.getFaculte());
            r.role = mStr;
            r.type = "Professeur";
            r.amount = p.getMontantSalaire();
            r.paid = p.getEstPaye();
            r.date = QString::fromStdString(p.getDatePaiement());
            r.isProf = true;
            r.originalIndex = (int)i;
            rows.push_back(r);

            if (r.paid) paidCount++;
            else unpaidCount++;
        }
    }

    if (filter == 0 || filter == 2) { // Admins
        for (size_t i = 0; i < admins.size(); ++i) {
            const auto &a = admins[i];
            RowData r;
            r.id = QString::fromStdString(a.getId());
            r.name = QString("%1 %2").arg(QString::fromStdString(a.getPrenom()), QString::fromStdString(a.getNom()));
            r.role = QString::fromStdString(a.getPoste());
            if (!a.getFaculte().empty()) r.role += QString(" (%1)").arg(QString::fromStdString(a.getFaculte()));
            r.type = "Administrateur";
            r.amount = a.getMontantSalaire();
            r.paid = a.getEstPaye();
            r.date = QString::fromStdString(a.getDatePaiement());
            r.isProf = false;
            r.originalIndex = (int)i;
            rows.push_back(r);

            if (r.paid) paidCount++;
            else unpaidCount++;
        }
    }

    salaryTable->setRowCount((int)rows.size());

    // Calcul du solde disponible actuel
    double soldeActuel = calculerSoldeTresorerieDisponible(profs, admins);

    for (int r = 0; r < (int)rows.size(); ++r) {
        const auto &data = rows[r];

        salaryTable->setItem(r, 0, new QTableWidgetItem(data.id));
        salaryTable->setItem(r, 1, new QTableWidgetItem(data.name));
        salaryTable->setItem(r, 2, new QTableWidgetItem(data.role));
        salaryTable->setItem(r, 3, new QTableWidgetItem(data.type));

        QString amountText = (data.paid && data.amount > 0) ? QString("%L1 HTG").arg(data.amount, 0, 'f', 2) : "—";
        QTableWidgetItem *amountItem = new QTableWidgetItem(amountText);
        amountItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        amountItem->setForeground(QColor(data.paid ? "#2b6cb0" : "#718096"));
        salaryTable->setItem(r, 4, amountItem);

        QTableWidgetItem *statusItem = new QTableWidgetItem(data.paid ? "✅ Payé" : "⏳ Non payé");
        statusItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        statusItem->setForeground(QColor(data.paid ? "#27ae60" : "#e74c3c"));
        salaryTable->setItem(r, 5, statusItem);

        QPushButton *payBtn = new QPushButton(data.paid ? "Modifier le montant" : "Marquer comme payé");
        payBtn->setCursor(Qt::PointingHandCursor);
        payBtn->setStyleSheet(
            "QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; border: none; }"
            "QPushButton:hover { background-color: #1a3353; }"
        );

        bool isP = data.isProf;
        int idx = data.originalIndex;
        QString empName = data.name;
        bool wasPaid = data.paid;
        double oldAmount = data.paid ? data.amount : 0.0;
        double defaultAmount = data.amount > 0 ? data.amount : 45000.00;

        connect(payBtn, &QPushButton::clicked, [this, isP, idx, empName, wasPaid, oldAmount, defaultAmount, todayStr]() {
            bool ok;
            double montant = QInputDialog::getDouble(
                this,
                "Saisie du Montant du Salaire",
                QString("Veuillez entrer le montant du salaire pour %1 (HTG) :").arg(empName),
                defaultAmount,
                1.0,
                5000000.0,
                2,
                &ok
            );

            if (ok && montant > 0) {
                // Règle critique : contrôle du solde disponible avant validation
                double soldeDisponible = calculerSoldeTresorerieDisponible(profs, admins);
                double montantADecaisser = wasPaid ? (montant - oldAmount) : montant;

                if (montantADecaisser > 0 && montantADecaisser > soldeDisponible) {
                    QMessageBox::critical(
                        this,
                        "Solde de Trésorerie Insuffisant",
                        QString("⚠️ Paiement impossible : le montant demandé (%L1 HTG) dépasse le solde disponible de l'université (%L2 HTG).\n\n"
                                "Veuillez enregistrer de nouvelles recettes étudiantes avant d'effectuer ce règlement.")
                            .arg(montantADecaisser, 0, 'f', 2)
                            .arg(soldeDisponible, 0, 'f', 2)
                    );
                    return;
                }

                // Application du paiement
                if (isP) {
                    profs[idx].setMontantSalaire(montant);
                    profs[idx].setEstPaye(true);
                    profs[idx].setDatePaiement(todayStr.toStdString());
                    sauvegarderProfesseurs(profs);
                } else {
                    admins[idx].setMontantSalaire(montant);
                    admins[idx].setEstPaye(true);
                    admins[idx].setDatePaiement(todayStr.toStdString());
                    sauvegarderAdministrateurs(admins);
                }

                double nouveauSolde = calculerSoldeTresorerieDisponible(profs, admins);

                // Traçabilité : enregistrement dans le journal de trésorerie
                enregistrerOperationTresorerie(
                    "Dépense",
                    isP ? "Salaire Professeur" : "Salaire Administrateur",
                    empName.toStdString(),
                    montantADecaisser,
                    nouveauSolde,
                    "Virement Bancaire",
                    wasPaid ? "Ajustement du montant de salaire" : "Paiement de salaire mensuel validé"
                );

                populateTable();
                QMessageBox::information(
                    this,
                    "Paiement Validé avec Succès",
                    QString("Le salaire de %L1 HTG a été validé avec succès pour %2.\n\nNouveau solde de trésorerie : %L3 HTG.")
                        .arg(montant, 0, 'f', 2)
                        .arg(empName)
                        .arg(nouveauSolde, 0, 'f', 2)
                );
            }
        });

        salaryTable->setCellWidget(r, 6, payBtn);
    }

    // Update stat cards
    cardPaid->updateValue(QString::number(paidCount));
    cardUnpaid->updateValue(QString::number(unpaidCount));

    int total = paidCount + unpaidCount;
    int pct = total > 0 ? (paidCount * 100 / total) : 100;
    cardTotal->updateValue(QString("%1%").arg(pct));

    // Update Solde Disponible card
    cardSolde->updateValue(QString("%L1 HTG").arg(soldeActuel, 0, 'f', 2));
}

void GestionSalaires::onMarkAllPaidClicked()
{
    bool ok;
    double montant = QInputDialog::getDouble(
        this,
        "Paiement Global du Personnel",
        "Veuillez saisir le montant mensuel du salaire à attribuer à chaque employé non payé (HTG) :",
        45000.00,
        1.0,
        5000000.0,
        2,
        &ok
    );

    if (ok && montant > 0) {
        double soldeDisponible = calculerSoldeTresorerieDisponible(profs, admins);
        double soldeRestant = soldeDisponible;

        QString todayStr = QDateTime::currentDateTime().toString("dd/MM/yyyy");
        int countPayes = 0;
        QStringList payesList;
        QStringList nonPayesList;

        // 1. Traitement ordonné des professeurs non payés
        for (auto &p : profs) {
            if (!p.getEstPaye()) {
                QString name = QString("Prof. %1 %2").arg(QString::fromStdString(p.getPrenom()), QString::fromStdString(p.getNom()));
                if (soldeRestant >= montant) {
                    p.setMontantSalaire(montant);
                    p.setEstPaye(true);
                    p.setDatePaiement(todayStr.toStdString());
                    soldeRestant -= montant;
                    countPayes++;
                    payesList << name;

                    enregistrerOperationTresorerie(
                        "Dépense",
                        "Salaire Professeur",
                        name.toStdString(),
                        montant,
                        soldeRestant,
                        "Virement Bancaire",
                        "Paiement global de masse"
                    );
                } else {
                    nonPayesList << QString("%1 (%2 HTG requis)").arg(name).arg(montant, 0, 'f', 2);
                }
            }
        }
        if (countPayes > 0) sauvegarderProfesseurs(profs);

        // 2. Traitement ordonné des administrateurs non payés
        for (auto &a : admins) {
            if (!a.getEstPaye()) {
                QString name = QString("%1 %2 (%3)").arg(QString::fromStdString(a.getPrenom()), QString::fromStdString(a.getNom()), QString::fromStdString(a.getPoste()));
                if (soldeRestant >= montant) {
                    a.setMontantSalaire(montant);
                    a.setEstPaye(true);
                    a.setDatePaiement(todayStr.toStdString());
                    soldeRestant -= montant;
                    countPayes++;
                    payesList << name;

                    enregistrerOperationTresorerie(
                        "Dépense",
                        "Salaire Administrateur",
                        name.toStdString(),
                        montant,
                        soldeRestant,
                        "Virement Bancaire",
                        "Paiement global de masse"
                    );
                } else {
                    nonPayesList << QString("%1 (%2 HTG requis)").arg(name).arg(montant, 0, 'f', 2);
                }
            }
        }
        if (countPayes > 0) sauvegarderAdministrateurs(admins);

        populateTable();

        // Synthèse du résultat
        if (nonPayesList.isEmpty() && countPayes > 0) {
            QMessageBox::information(
                this,
                "Paiement Global Effectué avec Succès",
                QString("✅ L'ensemble des salaires (%1 employé(s)) a été réglé avec succès pour un montant total de %L2 HTG.\n\n"
                        "Solde de trésorerie restant : %L3 HTG.")
                    .arg(countPayes)
                    .arg(countPayes * montant, 0, 'f', 2)
                    .arg(soldeRestant, 0, 'f', 2)
            );
        } else if (countPayes > 0 && !nonPayesList.isEmpty()) {
            double manqueAGagner = (nonPayesList.size() * montant) - soldeRestant;
            QMessageBox::warning(
                this,
                "Paiement Partiel — Fonds Insuffisants",
                QString("⚠️ Fonds de trésorerie partiellement insuffisants :\n\n"
                        "• Employés payés (%1) : %L2 HTG décaissés.\n"
                        "• Employés non payés (%3) par manque de liquidités :\n  - %4\n\n"
                        "Déficit de trésorerie à combler : %L5 HTG\n"
                        "Solde restant en caisse : %L6 HTG")
                    .arg(countPayes)
                    .arg(countPayes * montant, 0, 'f', 2)
                    .arg(nonPayesList.size())
                    .arg(nonPayesList.join("\n  - "))
                    .arg(manqueAGagner > 0 ? manqueAGagner : 0.0, 0, 'f', 2)
                    .arg(soldeRestant, 0, 'f', 2)
            );
        } else if (countPayes == 0 && !nonPayesList.isEmpty()) {
            QMessageBox::critical(
                this,
                "Paiement Global Impossible",
                QString("❌ Échec de l'opération : Le solde disponible (%L1 HTG) ne permet de régler aucun des %2 employés en attente (%L3 HTG par personne).\n\n"
                        "Manque à gagner total : %L4 HTG.")
                    .arg(soldeDisponible, 0, 'f', 2)
                    .arg(nonPayesList.size())
                    .arg(montant, 0, 'f', 2)
                    .arg((nonPayesList.size() * montant) - soldeDisponible, 0, 'f', 2)
            );
        } else {
            QMessageBox::information(this, "Information", "Tous les employés sont déjà marqués comme payés.");
        }
    }
}

void GestionSalaires::onViewJournalTresorerieClicked()
{
    std::vector<TransactionTresorerie> journal = chargerJournalTresorerie();

    QDialog dlg(this);
    dlg.setWindowTitle("Journal de Caisse & Traçabilité Financière (UDH)");
    dlg.setMinimumSize(880, 520);
    dlg.setStyleSheet("QDialog { background-color: #f8fafc; }");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);

    // Header Summary
    double totalRecettes = calculerRecettesEtudiantesTotales();
    double totalSalaires = calculerSalairesVersesTotaux(profs, admins);
    double soldeActuel = totalRecettes - totalSalaires;

    QFrame *summaryCard = new QFrame(&dlg);
    summaryCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 8px; border: 1px solid #cbd5e0; }");
    QHBoxLayout *sumLayout = new QHBoxLayout(summaryCard);
    sumLayout->setContentsMargins(15, 10, 15, 10);

    QLabel *recLbl = new QLabel(QString("<b>Entrées (Recettes) :</b> <span style='color:#27ae60;'>+%L1 HTG</span>").arg(totalRecettes, 0, 'f', 2), summaryCard);
    QLabel *depLbl = new QLabel(QString("<b>Sorties (Salaires) :</b> <span style='color:#e74c3c;'>-%L1 HTG</span>").arg(totalSalaires, 0, 'f', 2), summaryCard);
    QLabel *soldeLbl = new QLabel(QString("<b>Solde Disponible :</b> <span style='color:#0D9488; font-size:14px;'>%L1 HTG</span>").arg(soldeActuel, 0, 'f', 2), summaryCard);

    sumLayout->addWidget(recLbl);
    sumLayout->addStretch();
    sumLayout->addWidget(depLbl);
    sumLayout->addStretch();
    sumLayout->addWidget(soldeLbl);
    layout->addWidget(summaryCard);

    // Table
    QTableWidget *table = new QTableWidget(&dlg);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"Date / Heure", "Type", "Catégorie", "Bénéficiaire / Émetteur", "Montant", "Solde après", "Méthode"});
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(34);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: 1px solid #cbd5e0; border-radius: 6px; color: #2d3748; font-size: 12px; }"
        "QTableWidget::item { padding: 6px; border-bottom: 1px solid #edf2f7; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; }"
    );

    table->setRowCount((int)journal.size());
    int r = 0;
    for (auto it = journal.rbegin(); it != journal.rend(); ++it) {
        table->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(it->date)));

        QTableWidgetItem *typeItem = new QTableWidgetItem(QString::fromStdString(it->type));
        typeItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        typeItem->setForeground(QColor(it->type == "Recette" ? "#27ae60" : "#e74c3c"));
        table->setItem(r, 1, typeItem);

        table->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(it->categorie)));
        table->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(it->beneficiaireOuPayeur)));

        QString mSign = (it->type == "Recette") ? "+" : "-";
        QTableWidgetItem *mItem = new QTableWidgetItem(QString("%1%L2 HTG").arg(mSign).arg(it->montant, 0, 'f', 2));
        mItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        mItem->setForeground(QColor(it->type == "Recette" ? "#27ae60" : "#e74c3c"));
        table->setItem(r, 4, mItem);

        QTableWidgetItem *sItem = new QTableWidgetItem(QString("%L1 HTG").arg(it->soldeApres, 0, 'f', 2));
        sItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        sItem->setForeground(QColor("#2b6cb0"));
        table->setItem(r, 5, sItem);

        table->setItem(r, 6, new QTableWidgetItem(QString::fromStdString(it->methode)));
        r++;
    }

    layout->addWidget(table);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    QPushButton *closeBtn = new QPushButton("Fermer", &dlg);
    closeBtn->setFixedHeight(36);
    closeBtn->setFixedWidth(120);
    closeBtn->setStyleSheet("QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; font-weight: bold; border: none; outline: none; }"
                           "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    layout->addLayout(btnRow);

    dlg.exec();
}
