#include "modulevalidationacademique.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QGroupBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

static const QString FICHIER_DECISIONS = "decisions_academiques.txt";

ModuleValidationAcademique::ModuleValidationAcademique(QWidget *parent)
    : QWidget(parent), table(nullptr), sujetEdit(nullptr), descEdit(nullptr), typeCombo(nullptr)
{
    setupUI();
    loadDecisions();
}

void ModuleValidationAcademique::refreshData() { loadDecisions(); }

void ModuleValidationAcademique::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("📜  Validation des décisions académiques");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a202c; background: transparent;");
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("Soumission et validation des arrêtés académiques, notes de service et circulaires.");
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size: 13px; color: #718096; background: transparent; margin-bottom: 6px;");
    mainLayout->addWidget(subtitle);

    /* ---- Form ---- */
    QGroupBox *formBox = new QGroupBox("Nouvelle décision académique");
    formBox->setStyleSheet(
        "QGroupBox { font-size: 14px; font-weight: bold; color: #2d3748; background: white; border: 1px solid #e2e8f0; border-radius: 12px; padding: 20px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 8px; }"
    );
    QGraphicsDropShadowEffect *formShadow = new QGraphicsDropShadowEffect();
    formShadow->setBlurRadius(15); formShadow->setColor(QColor(0,0,0,25)); formShadow->setOffset(0,3);
    formBox->setGraphicsEffect(formShadow);

    QVBoxLayout *formLayout = new QVBoxLayout(formBox);
    formLayout->setSpacing(10);

    QHBoxLayout *row1 = new QHBoxLayout();
    typeCombo = new QComboBox();
    typeCombo->addItems({"Arrêté", "Note de service", "Circulaire", "Décret", "Résolution"});
    typeCombo->setFixedHeight(36);
    typeCombo->setMinimumWidth(180);
    typeCombo->setStyleSheet("QComboBox { border: 1px solid #cbd5e0; border-radius: 8px; padding: 0 12px; font-size: 13px; background: white; color: #2d3748; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: white; color: #2d3748; selection-background-color: #edf2f7; }");
    row1->addWidget(new QLabel("Type :"));
    row1->addWidget(typeCombo);

    sujetEdit = new QLineEdit();
    sujetEdit->setPlaceholderText("Sujet de la décision...");
    sujetEdit->setFixedHeight(36);
    sujetEdit->setStyleSheet("QLineEdit { border: 1px solid #cbd5e0; border-radius: 8px; padding: 0 12px; font-size: 13px; background: white; color: #2d3748; }");
    row1->addWidget(new QLabel("Sujet :"));
    row1->addWidget(sujetEdit, 1);
    formLayout->addLayout(row1);

    descEdit = new QTextEdit();
    descEdit->setPlaceholderText("Description détaillée de la décision académique...");
    descEdit->setFixedHeight(80);
    descEdit->setStyleSheet("QTextEdit { border: 1px solid #cbd5e0; border-radius: 8px; padding: 8px; font-size: 13px; background: white; color: #2d3748; }");
    formLayout->addWidget(descEdit);

    QPushButton *addBtn = new QPushButton("✅  Soumettre la décision");
    addBtn->setFixedHeight(38);
    addBtn->setFixedWidth(240);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border: none; border-radius: 8px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(addBtn, &QPushButton::clicked, this, &ModuleValidationAcademique::addDecision);
    formLayout->addWidget(addBtn, 0, Qt::AlignRight);

    mainLayout->addWidget(formBox);

    /* ---- Table ---- */
    table = new QTableWidget();
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Type", "Sujet", "Description", "Date", "Statut"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(44);
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
    QGraphicsDropShadowEffect *tblShadow = new QGraphicsDropShadowEffect();
    tblShadow->setBlurRadius(18); tblShadow->setColor(QColor(0,0,0,30)); tblShadow->setOffset(0,3);
    table->setGraphicsEffect(tblShadow);

    mainLayout->addWidget(table, 1);
}

void ModuleValidationAcademique::loadDecisions()
{
    table->setRowCount(0);
    QFile file(FICHIER_DECISIONS);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split("~");
        if (parts.size() < 5) continue;

        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(parts[0])); // Type
        table->setItem(row, 1, new QTableWidgetItem(parts[1])); // Sujet
        table->setItem(row, 2, new QTableWidgetItem(parts[2])); // Desc
        table->setItem(row, 3, new QTableWidgetItem(parts[3])); // Date

        // Statut badge
        QString statut = parts[4];
        QLabel *badge = new QLabel(statut);
        badge->setAlignment(Qt::AlignCenter);
        QString bc;
        if (statut == "Validé") bc = "background-color: #c6f6d5; color: #276749;";
        else if (statut == "En attente") bc = "background-color: #fefcbf; color: #975a16;";
        else bc = "background-color: #fed7d7; color: #9b2c2c;";
        badge->setStyleSheet(QString("font-size: 11px; font-weight: bold; border-radius: 10px; padding: 4px 12px; %1").arg(bc));
        QWidget *bw = new QWidget();
        QHBoxLayout *bl = new QHBoxLayout(bw);
        bl->setContentsMargins(4, 2, 4, 2);
        bl->addWidget(badge, 0, Qt::AlignCenter);
        table->setCellWidget(row, 4, bw);

        row++;
    }
    file.close();
}

void ModuleValidationAcademique::saveDecisions()
{
    QFile file(FICHIER_DECISIONS);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return;
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    for (int r = 0; r < table->rowCount(); ++r) {
        QString type = table->item(r, 0) ? table->item(r, 0)->text() : "";
        QString sujet = table->item(r, 1) ? table->item(r, 1)->text() : "";
        QString desc = table->item(r, 2) ? table->item(r, 2)->text() : "";
        QString date = table->item(r, 3) ? table->item(r, 3)->text() : "";
        // For statut, check the cell widget label
        QString statut = "Validé";
        QWidget *w = table->cellWidget(r, 4);
        if (w) {
            QLabel *lbl = w->findChild<QLabel*>();
            if (lbl) statut = lbl->text();
        }
        out << type << "~" << sujet << "~" << desc << "~" << date << "~" << statut << "\n";
    }
    file.close();
}

void ModuleValidationAcademique::addDecision()
{
    QString sujet = sujetEdit->text().trimmed();
    QString desc = descEdit->toPlainText().trimmed().replace("~", "-").replace("\n", " ");
    QString type = typeCombo->currentText();

    if (sujet.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez saisir un sujet.");
        return;
    }

    QString date = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");

    // Append to file
    QFile file(FICHIER_DECISIONS);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << type << "~" << sujet << "~" << desc << "~" << date << "~Validé\n";
        file.close();
    }

    sujetEdit->clear();
    descEdit->clear();
    QMessageBox::information(this, "Décision soumise", "La décision académique a été enregistrée et validée.");
    loadDecisions();
}
