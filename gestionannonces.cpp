#include "gestionannonces.h"
#include "etudiant.h"
#include "professeur.h"
#include "administrateur.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QDateTime>
#include <QScrollArea>

GestionAnnonces::GestionAnnonces(QWidget *parent)
    : QWidget(parent),
      cardTotal(nullptr), cardMonth(nullptr), cardAudience(nullptr),
      titleEdit(nullptr), contentEdit(nullptr),
      cbEtudiants(nullptr), cbProfesseurs(nullptr), cbAdmins(nullptr), cbTous(nullptr),
      dateLabel(nullptr), annoncesTable(nullptr)
{
    setupUI();
}

void GestionAnnonces::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *content = new QWidget();
    content->setStyleSheet("background-color: #f0f2f5;");
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(25, 20, 25, 25);
    mainLayout->setSpacing(20);

    // 1. 3 StatCards Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    cardTotal    = new StatCard("📢", "0", "Total annonces publiées", "#2b6cb0", content);
    cardMonth    = new StatCard("📅", "0", "Annonces ce mois-ci", "#27ae60", content);
    cardAudience = new StatCard("👥", "0", "Destinataires touchés", "#8e44ad", content);

    statsLayout->addWidget(cardTotal);
    statsLayout->addWidget(cardMonth);
    statsLayout->addWidget(cardAudience);
    mainLayout->addLayout(statsLayout);

    // 2. Form Card Frame
    QFrame *formCard = new QFrame(content);
    formCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *formShadow = new QGraphicsDropShadowEffect(formCard);
    formShadow->setBlurRadius(15);
    formShadow->setColor(QColor(0, 0, 0, 15));
    formShadow->setOffset(0, 4);
    formCard->setGraphicsEffect(formShadow);

    QVBoxLayout *formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(25, 22, 25, 25);
    formLayout->setSpacing(14);

    QLabel *formTitle = new QLabel("📢 Rédiger et Publier une Annonce Institutionnelle", formCard);
    formTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #0b1e36; border: none;");
    formLayout->addWidget(formTitle);

    // Form inputs styles
    const QString inputStyle =
        "QLineEdit, QTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus, QTextEdit:focus { border: 1.5px solid #2b6cb0; }";

    // Title input
    QLabel *lblTitle = new QLabel("Titre de l'annonce :", formCard);
    lblTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #2d3748; border: none;");
    titleEdit = new QLineEdit(formCard);
    titleEdit->setPlaceholderText("Ex: Calendrier des examens finaux, Arrêté rectoral, Rappel paiement...");
    titleEdit->setFixedHeight(40);
    titleEdit->setStyleSheet(inputStyle);
    formLayout->addWidget(lblTitle);
    formLayout->addWidget(titleEdit);

    // Destinataires Checkboxes
    QLabel *lblDest = new QLabel("Destinataires cibles :", formCard);
    lblDest->setStyleSheet("font-size: 13px; font-weight: bold; color: #2d3748; border: none;");
    formLayout->addWidget(lblDest);

    QHBoxLayout *cbLayout = new QHBoxLayout();
    cbLayout->setSpacing(20);

    const QString cbStyle =
        "QCheckBox { font-size: 13px; font-weight: bold; color: #2d3748; spacing: 8px; border: none; }"
        "QCheckBox::indicator { width: 18px; height: 18px; border-radius: 4px; border: 1.5px solid #a0aec0; background-color: white; }"
        "QCheckBox::indicator:checked { background-color: #2b6cb0; border-color: #2b6cb0; image: none; }";

    cbEtudiants = new QCheckBox("🎓 Étudiants", formCard);
    cbEtudiants->setStyleSheet(cbStyle);
    cbEtudiants->setChecked(true);

    cbProfesseurs = new QCheckBox("👨‍🏫 Professeurs", formCard);
    cbProfesseurs->setStyleSheet(cbStyle);
    cbProfesseurs->setChecked(true);

    cbAdmins = new QCheckBox("🏛️ Administrateurs (Postes)", formCard);
    cbAdmins->setStyleSheet(cbStyle);
    cbAdmins->setChecked(true);

    cbTous = new QCheckBox("🌐 Tous", formCard);
    cbTous->setStyleSheet(cbStyle);
    cbTous->setChecked(true);

    connect(cbTous, &QCheckBox::toggled, this, &GestionAnnonces::onTousToggled);

    cbLayout->addWidget(cbEtudiants);
    cbLayout->addWidget(cbProfesseurs);
    cbLayout->addWidget(cbAdmins);
    cbLayout->addWidget(cbTous);
    cbLayout->addStretch();
    formLayout->addLayout(cbLayout);

    // Content input
    QLabel *lblContent = new QLabel("Contenu du message :", formCard);
    lblContent->setStyleSheet("font-size: 13px; font-weight: bold; color: #2d3748; border: none;");
    contentEdit = new QTextEdit(formCard);
    contentEdit->setPlaceholderText("Saisissez ici le texte détaillé de l'annonce officielle...");
    contentEdit->setFixedHeight(110);
    contentEdit->setStyleSheet(inputStyle);
    formLayout->addWidget(lblContent);
    formLayout->addWidget(contentEdit);

    // Bottom action row: Date + Publish Button
    QHBoxLayout *actionRow = new QHBoxLayout();
    QString todayStr = QDate::currentDate().toString("dd/MM/yyyy");
    dateLabel = new QLabel(QString("📅 Date d'envoi automatique : <b>%1</b>").arg(todayStr), formCard);
    dateLabel->setStyleSheet("font-size: 13px; color: #4a5568; border: none;");

    QPushButton *publishBtn = new QPushButton("📢  Publier l'annonce", formCard);
    publishBtn->setFixedHeight(42);
    publishBtn->setFixedWidth(200);
    publishBtn->setCursor(Qt::PointingHandCursor);
    publishBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(publishBtn, &QPushButton::clicked, this, &GestionAnnonces::onPublishClicked);

    actionRow->addWidget(dateLabel);
    actionRow->addStretch();
    actionRow->addWidget(publishBtn);
    formLayout->addLayout(actionRow);

    mainLayout->addWidget(formCard);

    // 3. Sent Announcements Table Frame
    QFrame *tableCard = new QFrame(content);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect(tableCard);
    tableShadow->setBlurRadius(15);
    tableShadow->setColor(QColor(0, 0, 0, 15));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(22, 20, 22, 22);
    tableCardLayout->setSpacing(14);

    QLabel *tableTitle = new QLabel("📋 Historique des Annonces Publiées", tableCard);
    tableTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #0b1e36; border: none;");
    tableCardLayout->addWidget(tableTitle);

    annoncesTable = new QTableWidget(tableCard);
    annoncesTable->setColumnCount(5);
    annoncesTable->setHorizontalHeaderLabels({"Titre de l'annonce", "Destinataires", "Date d'envoi", "Lectures (Vues)", "Action"});
    annoncesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    annoncesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    annoncesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    annoncesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    annoncesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    annoncesTable->verticalHeader()->setVisible(false);
    annoncesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    annoncesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    annoncesTable->setAlternatingRowColors(true);
    annoncesTable->setShowGrid(false);
    annoncesTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; "
        "  border: none; border-bottom: 2px solid #cbd5e0; padding: 10px; font-size: 12px; }"
    );
    annoncesTable->setMinimumHeight(260);

    tableCardLayout->addWidget(annoncesTable);
    mainLayout->addWidget(tableCard);

    scroll->setWidget(content);
    rootLayout->addWidget(scroll);

    refreshData();
}

void GestionAnnonces::onTousToggled(bool checked)
{
    if (checked) {
        cbEtudiants->setChecked(true);
        cbProfesseurs->setChecked(true);
        cbAdmins->setChecked(true);
    }
}

void GestionAnnonces::onPublishClicked()
{
    QString title = titleEdit->text().trimmed();
    QString content = contentEdit->toPlainText().trimmed();

    if (title.isEmpty() || content.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez renseigner le titre et le contenu de l'annonce.");
        return;
    }

    std::vector<std::string> dests;
    if (cbTous->isChecked() || (cbEtudiants->isChecked() && cbProfesseurs->isChecked() && cbAdmins->isChecked())) {
        dests.push_back("Tous");
    } else {
        if (cbEtudiants->isChecked()) dests.push_back("Etudiants");
        if (cbProfesseurs->isChecked()) dests.push_back("Professeurs");
        if (cbAdmins->isChecked()) dests.push_back("Administrateurs");
    }

    if (dests.empty()) {
        QMessageBox::warning(this, "Destinataire requis", "Veuillez sélectionner au moins un groupe de destinataires.");
        return;
    }

    // Generate unique ID
    qint64 ts = QDateTime::currentMSecsSinceEpoch();
    std::string id = "ANN-" + std::to_string(ts);
    std::string dateEnvoi = QDate::currentDate().toString("dd/MM/yyyy").toStdString();

    Annonce a(id, title.toStdString(), content.toStdString(), dateEnvoi, dests, {});
    ajouterAnnonce(a);

    titleEdit->clear();
    contentEdit->clear();
    cbTous->setChecked(true);

    refreshData();
    emit annoncesUpdated();

    QMessageBox::information(this, "Annonce Publiée", "L'annonce institutionnelle a été publiée avec succès et transmise à tous les comptes concernés.");
}

void GestionAnnonces::refreshData()
{
    auto annonces = chargerAnnonces();

    // Sort descending by ID / date
    std::reverse(annonces.begin(), annonces.end());

    int totalCount = (int)annonces.size();
    int monthCount = 0;
    QString currentMonthYear = QDate::currentDate().toString("MM/yyyy");

    for (const auto &a : annonces) {
        if (QString::fromStdString(a.dateEnvoi).endsWith(currentMonthYear)) {
            monthCount++;
        }
    }

    // Active audience calculation
    int audience = 0;
    auto comptes = chargerComptes();
    auto profs = chargerProfesseurs();
    auto admins = chargerAdministrateurs();
    audience = (int)(comptes.size() + profs.size() + admins.size());

    if (cardTotal) cardTotal->updateValue(QString::number(totalCount));
    if (cardMonth) cardMonth->updateValue(QString::number(monthCount));
    if (cardAudience) cardAudience->updateValue(QString("%1 comptes").arg(audience));

    if (!annoncesTable) return;

    annoncesTable->setRowCount((int)annonces.size());

    for (int r = 0; r < (int)annonces.size(); ++r) {
        const auto &a = annonces[r];

        auto *itemTitle = new QTableWidgetItem(QString::fromStdString(a.titre));
        itemTitle->setFont(QFont("Segoe UI", 10, QFont::Bold));
        itemTitle->setForeground(QColor("#1a202c"));

        QString destStr = QString::fromStdString(a.destinatairesChaine());
        auto *itemDest = new QTableWidgetItem(destStr);
        itemDest->setForeground(QColor("#2b6cb0"));
        itemDest->setFont(QFont("Segoe UI", 9, QFont::Bold));

        auto *itemDate = new QTableWidgetItem(QString::fromStdString(a.dateEnvoi));
        itemDate->setForeground(QColor("#718096"));

        auto *itemViews = new QTableWidgetItem(QString("👁️ %1 vue(s)").arg(a.vuPar.size()));
        itemViews->setForeground(QColor("#27ae60"));
        itemViews->setFont(QFont("Segoe UI", 9, QFont::Bold));

        annoncesTable->setItem(r, 0, itemTitle);
        annoncesTable->setItem(r, 1, itemDest);
        annoncesTable->setItem(r, 2, itemDate);
        annoncesTable->setItem(r, 3, itemViews);

        QPushButton *delBtn = new QPushButton("🗑️ Supprimer");
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setStyleSheet(
            "QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 12px; border: none; }"
            "QPushButton:hover { background-color: #1a3353; }"
        );

        std::string annId = a.id;
        QString annTitleStr = QString::fromStdString(a.titre);
        connect(delBtn, &QPushButton::clicked, [this, annId, annTitleStr]() {
            auto reply = QMessageBox::question(
                this,
                "Confirmation de suppression",
                QString("Êtes-vous sûr de vouloir supprimer définitivement l'annonce :\n« %1 » ?\n\nElle sera retirée de tous les comptes destinataires.").arg(annTitleStr),
                QMessageBox::Yes | QMessageBox::No
            );
            if (reply == QMessageBox::Yes) {
                supprimerAnnonce(annId);
                refreshData();
                emit annoncesUpdated();
                QMessageBox::information(this, "Suppression effectuée", "L'annonce a été retirée avec succès.");
            }
        });

        annoncesTable->setCellWidget(r, 4, delBtn);
    }
}
