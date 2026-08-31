#include "modulesupport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <fstream>
#include <sstream>
#include <algorithm>

static const std::string FICHIER_SUPPORT = "support.txt";
static const char SEPARATEUR_SUPPORT = '~';

static std::vector<std::string> decouperSupport(const std::string &texte, char sep) {
    std::vector<std::string> champs;
    std::stringstream ss(texte);
    std::string champ;
    while (getline(ss, champ, sep)) {
        champs.push_back(champ);
    }
    if (!texte.empty() && texte.back() == sep) {
        champs.push_back("");
    }
    return champs;
}

static std::string echapperTexte(std::string texte) {
    std::string res;
    for (char c : texte) {
        if (c == '\n') res += "\\n";
        else if (c == '\r') continue;
        else if (c == SEPARATEUR_SUPPORT) res += "-";
        else res += c;
    }
    return res;
}

static std::string restaurerTexte(const std::string &texte) {
    std::string res;
    for (size_t i = 0; i < texte.size(); ++i) {
        if (texte[i] == '\\' && i + 1 < texte.size() && texte[i + 1] == 'n') {
            res += '\n';
            i++;
        } else {
            res += texte[i];
        }
    }
    return res;
}

TicketSupport::TicketSupport(const std::string &id, const std::string &expId, const std::string &nomExp,
                             const std::string &typeC, const std::string &suj, const std::string &msg,
                             const std::string &dt, const std::string &st)
    : id(id), expediteurId(expId), nomExpediteur(nomExp), typeCompte(typeC),
      sujet(suj), message(msg), date(dt), statut(st) {}

std::string TicketSupport::toLigne() const {
    std::ostringstream oss;
    oss << id << SEPARATEUR_SUPPORT
        << expediteurId << SEPARATEUR_SUPPORT
        << echapperTexte(nomExpediteur) << SEPARATEUR_SUPPORT
        << typeCompte << SEPARATEUR_SUPPORT
        << echapperTexte(sujet) << SEPARATEUR_SUPPORT
        << echapperTexte(message) << SEPARATEUR_SUPPORT
        << date << SEPARATEUR_SUPPORT
        << statut;
    return oss.str();
}

TicketSupport TicketSupport::fromLigne(const std::string &ligne) {
    auto c = decouperSupport(ligne, SEPARATEUR_SUPPORT);
    if (c.size() < 7) return TicketSupport();

    std::string id = c[0];
    std::string expId = c[1];
    std::string nomExp = restaurerTexte(c[2]);
    std::string typeC = c[3];
    std::string suj = restaurerTexte(c[4]);
    std::string msg = restaurerTexte(c[5]);
    std::string dt = c[6];
    std::string st = (c.size() >= 8) ? c[7] : "Non traité";

    return TicketSupport(id, expId, nomExp, typeC, suj, msg, dt, st);
}

std::vector<TicketSupport> chargerTicketsSupport() {
    std::vector<TicketSupport> liste;
    std::ifstream f(FICHIER_SUPPORT);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        TicketSupport t = TicketSupport::fromLigne(ligne);
        if (!t.id.empty()) {
            liste.push_back(t);
        }
    }
    return liste;
}

void sauvegarderTicketsSupport(const std::vector<TicketSupport> &tickets) {
    std::ofstream f(FICHIER_SUPPORT, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &t : tickets) {
        f << t.toLigne() << "\n";
    }
}

void ajouterTicketSupport(const TicketSupport &ticket) {
    std::vector<TicketSupport> liste = chargerTicketsSupport();
    liste.push_back(ticket);
    sauvegarderTicketsSupport(liste);
}

bool mettreAJourStatutTicket(const std::string &idTicket, const std::string &nouveauStatut) {
    std::vector<TicketSupport> liste = chargerTicketsSupport();
    bool trouve = false;
    for (auto &t : liste) {
        if (t.id == idTicket) {
            t.statut = nouveauStatut;
            trouve = true;
            break;
        }
    }
    if (trouve) {
        sauvegarderTicketsSupport(liste);
    }
    return trouve;
}

// ============================================================
//  ModuleSupport Widget Implementation
// ============================================================
ModuleSupport::ModuleSupport(Mode mode, const std::string &userId, const std::string &userName,
                             const std::string &userType, QWidget *parent)
    : QWidget(parent), currentMode(mode), currentUserId(userId),
      currentUserName(userName), currentUserType(userType),
      subjectEdit(nullptr), messageEdit(nullptr), myTicketsTable(nullptr),
      adminTicketsTable(nullptr), statusFilterCombo(nullptr), roleFilterCombo(nullptr),
      totalTicketsLabel(nullptr), pendingTicketsLabel(nullptr), resolvedTicketsLabel(nullptr)
{
    if (currentMode == UserMode) {
        setupUserUI();
    } else {
        setupAdminUI();
    }
    refreshData();
}

void ModuleSupport::setupUserUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(20);

    // Form Card
    QFrame *formCard = new QFrame(this);
    formCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(formCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    formCard->setGraphicsEffect(shadow);

    QVBoxLayout *fLayout = new QVBoxLayout(formCard);
    fLayout->setContentsMargins(25, 22, 25, 22);
    fLayout->setSpacing(14);

    QLabel *formTitle = new QLabel("🎧 Contacter le Support / Administration", formCard);
    formTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #0b1e36; border: none;");

    QLabel *infoLbl = new QLabel("Une question, un souci administratif ou pédagogique ? Envoyez un message directement à l'administration.", formCard);
    infoLbl->setStyleSheet("color: #718096; font-size: 13px; border: none;");

    QLabel *subLbl = new QLabel("Sujet de votre demande :", formCard);
    subLbl->setStyleSheet("font-weight: bold; color: #2d3748; font-size: 13px; border: none;");

    subjectEdit = new QLineEdit(formCard);
    subjectEdit->setPlaceholderText("Ex: Problème d'accès aux notes, Demande d'attestation...");
    subjectEdit->setFixedHeight(40);
    subjectEdit->setStyleSheet("QLineEdit { background-color: #f8fafc; border: 1.5px solid #cbd5e0; border-radius: 8px; padding: 0 12px; font-size: 13px; color: #2d3748; }"
                               "QLineEdit:focus { border: 1.5px solid #2b6cb0; background-color: #ffffff; }");

    QLabel *msgLbl = new QLabel("Votre message :", formCard);
    msgLbl->setStyleSheet("font-weight: bold; color: #2d3748; font-size: 13px; border: none;");

    messageEdit = new QTextEdit(formCard);
    messageEdit->setPlaceholderText("Détaillez clairement votre demande ici...");
    messageEdit->setFixedHeight(120);
    messageEdit->setStyleSheet("QTextEdit { background-color: #f8fafc; border: 1.5px solid #cbd5e0; border-radius: 8px; padding: 10px; font-size: 13px; color: #2d3748; }"
                               "QTextEdit:focus { border: 1.5px solid #2b6cb0; background-color: #ffffff; }");

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton *sendBtn = new QPushButton("🚀 Envoyer la demande", formCard);
    sendBtn->setFixedHeight(40);
    sendBtn->setFixedWidth(180);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: #ffffff; border-radius: 8px; font-weight: bold; font-size: 13px; border: none; }"
                           "QPushButton:hover { background-color: #1a3353; }"
                           "QPushButton:pressed { background-color: #081424; }");
    connect(sendBtn, &QPushButton::clicked, this, &ModuleSupport::onSendTicketClicked);
    btnRow->addWidget(sendBtn);

    fLayout->addWidget(formTitle);
    fLayout->addWidget(infoLbl);
    fLayout->addSpacing(5);
    fLayout->addWidget(subLbl);
    fLayout->addWidget(subjectEdit);
    fLayout->addWidget(msgLbl);
    fLayout->addWidget(messageEdit);
    fLayout->addLayout(btnRow);

    mainLayout->addWidget(formCard);

    // My Tickets Table Card
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(22, 20, 22, 20);
    tLayout->setSpacing(12);

    QLabel *tableTitle = new QLabel("📋 Historique de mes tickets de support", tableCard);
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    tLayout->addWidget(tableTitle);

    myTicketsTable = new QTableWidget(tableCard);
    myTicketsTable->setColumnCount(5);
    myTicketsTable->setHorizontalHeaderLabels({"ID Ticket", "Sujet", "Date", "Statut", "Message"});
    myTicketsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    myTicketsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    myTicketsTable->verticalHeader()->setVisible(false);
    myTicketsTable->verticalHeader()->setDefaultSectionSize(38);
    myTicketsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myTicketsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    myTicketsTable->setAlternatingRowColors(true);
    myTicketsTable->setShowGrid(false);
    myTicketsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(myTicketsTable);
    mainLayout->addWidget(tableCard);
}

void ModuleSupport::setupAdminUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    // Stat cards row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    auto makeMiniCard = [](const QString &icon, const QString &title, QLabel *&valLbl, const QString &color) {
        QFrame *card = new QFrame();
        card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 10px; border: 1px solid #e2e8f0; }");
        QHBoxLayout *h = new QHBoxLayout(card);
        h->setContentsMargins(15, 12, 15, 12);
        h->setSpacing(12);

        QLabel *ic = new QLabel(icon);
        ic->setFixedSize(42, 42);
        ic->setAlignment(Qt::AlignCenter);
        ic->setStyleSheet(QString("background-color: %1; color: white; border-radius: 21px; font-size: 18px;").arg(color));

        QVBoxLayout *v = new QVBoxLayout();
        v->setSpacing(2);
        valLbl = new QLabel("0");
        valLbl->setStyleSheet("font-size: 18px; font-weight: bold; color: #1a202c; border: none;");
        QLabel *sub = new QLabel(title);
        sub->setStyleSheet("font-size: 12px; color: #718096; border: none;");
        v->addWidget(valLbl);
        v->addWidget(sub);

        h->addWidget(ic);
        h->addLayout(v);
        h->addStretch();
        return card;
    };

    statsLayout->addWidget(makeMiniCard("📨", "Total Tickets", totalTicketsLabel, "#3182CE"));
    statsLayout->addWidget(makeMiniCard("⏳", "Non traités / En cours", pendingTicketsLabel, "#DD6B20"));
    statsLayout->addWidget(makeMiniCard("✅", "Résolus", resolvedTicketsLabel, "#38A169"));
    mainLayout->addLayout(statsLayout);

    // Table Card
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);
    tcLayout->setContentsMargins(22, 20, 22, 22);
    tcLayout->setSpacing(14);

    // Header filters
    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *title = new QLabel("📬 Suivi des Tickets de Support Reçus", tableCard);
    title->setStyleSheet("font-size: 17px; font-weight: bold; color: #1a202c; border: none;");

    statusFilterCombo = new QComboBox(tableCard);
    statusFilterCombo->setFixedWidth(140);
    statusFilterCombo->setFixedHeight(36);
    statusFilterCombo->setStyleSheet("QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; border-radius: 6px; padding-left: 8px; font-weight: bold; font-size: 12px; }");
    statusFilterCombo->addItems({"Tous statuts", "Non traité", "En cours", "Résolu"});
    connect(statusFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModuleSupport::onFilterChanged);

    roleFilterCombo = new QComboBox(tableCard);
    roleFilterCombo->setFixedWidth(140);
    roleFilterCombo->setFixedHeight(36);
    roleFilterCombo->setStyleSheet("QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; border-radius: 6px; padding-left: 8px; font-weight: bold; font-size: 12px; }");
    roleFilterCombo->addItems({"Tous rôles", "Étudiant", "Professeur"});
    connect(roleFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModuleSupport::onFilterChanged);

    headerRow->addWidget(title);
    headerRow->addStretch();
    headerRow->addWidget(new QLabel("Rôle :", tableCard));
    headerRow->addWidget(roleFilterCombo);
    headerRow->addWidget(new QLabel("Statut :", tableCard));
    headerRow->addWidget(statusFilterCombo);

    tcLayout->addLayout(headerRow);

    // Table
    adminTicketsTable = new QTableWidget(tableCard);
    adminTicketsTable->setColumnCount(7);
    adminTicketsTable->setHorizontalHeaderLabels({"Expéditeur", "Rôle", "Sujet", "Date", "Statut", "Détail", "Changer Statut"});
    adminTicketsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    adminTicketsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    adminTicketsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    adminTicketsTable->verticalHeader()->setVisible(false);
    adminTicketsTable->verticalHeader()->setDefaultSectionSize(38);
    adminTicketsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    adminTicketsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    adminTicketsTable->setAlternatingRowColors(true);
    adminTicketsTable->setShowGrid(false);
    adminTicketsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tcLayout->addWidget(adminTicketsTable);
    mainLayout->addWidget(tableCard);
}

void ModuleSupport::onSendTicketClicked()
{
    if (!subjectEdit || !messageEdit) return;

    QString subject = subjectEdit->text().trimmed();
    QString msg = messageEdit->toPlainText().trimmed();

    if (subject.isEmpty() || msg.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir le sujet et le message de votre demande.");
        return;
    }

    std::string ticketId = "SUP-" + std::to_string(QDateTime::currentMSecsSinceEpoch() % 1000000);
    std::string dateStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm").toStdString();

    TicketSupport t(ticketId, currentUserId, currentUserName, currentUserType,
                    subject.toStdString(), msg.toStdString(), dateStr, "Non traité");

    ajouterTicketSupport(t);

    subjectEdit->clear();
    messageEdit->clear();

    QMessageBox::information(this, "Demande envoyée",
                             "Votre demande de support a été transmise avec succès à l'administration.\n"
                             "Numéro de référence : " + QString::fromStdString(ticketId));

    refreshData();
}

void ModuleSupport::onFilterChanged()
{
    refreshData();
}

void ModuleSupport::refreshData()
{
    std::vector<TicketSupport> allTickets = chargerTicketsSupport();

    if (currentMode == UserMode) {
        if (!myTicketsTable) return;
        myTicketsTable->setRowCount(0);

        int row = 0;
        for (auto it = allTickets.rbegin(); it != allTickets.rend(); ++it) {
            if (it->expediteurId == currentUserId || (!currentUserId.empty() && it->expediteurId.empty())) {
                myTicketsTable->insertRow(row);

                QTableWidgetItem *idItem = new QTableWidgetItem(QString::fromStdString(it->id));
                idItem->setTextAlignment(Qt::AlignCenter);
                myTicketsTable->setItem(row, 0, idItem);

                QTableWidgetItem *sujetItem = new QTableWidgetItem(QString::fromStdString(it->sujet));
                myTicketsTable->setItem(row, 1, sujetItem);

                QTableWidgetItem *dateItem = new QTableWidgetItem(QString::fromStdString(it->date));
                dateItem->setTextAlignment(Qt::AlignCenter);
                myTicketsTable->setItem(row, 2, dateItem);

                // Badge de statut
                QWidget *badgeWidget = new QWidget();
                QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
                bLayout->setContentsMargins(4, 2, 4, 2);
                bLayout->setAlignment(Qt::AlignCenter);

                QLabel *badge = new QLabel(QString::fromStdString(it->statut));
                badge->setAlignment(Qt::AlignCenter);
                badge->setFixedWidth(100);
                badge->setFixedHeight(24);

                if (it->statut == "Résolu") {
                    badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 12px; font-size: 11px;");
                } else if (it->statut == "En cours") {
                    badge->setStyleSheet("background-color: #FEEBC8; color: #7B341E; font-weight: bold; border-radius: 12px; font-size: 11px;");
                } else {
                    badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 12px; font-size: 11px;");
                }
                bLayout->addWidget(badge);
                myTicketsTable->setCellWidget(row, 3, badgeWidget);

                // View message action
                QPushButton *viewBtn = new QPushButton("👁️ Voir");
                viewBtn->setCursor(Qt::PointingHandCursor);
                viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; border: none; }"
                                       "QPushButton:hover { background-color: #1a3353; }");
                QString tId = QString::fromStdString(it->id);
                connect(viewBtn, &QPushButton::clicked, [this, tId]() { onViewMessageClicked(tId); });
                myTicketsTable->setCellWidget(row, 4, viewBtn);

                row++;
            }
        }
    } else {
        if (!adminTicketsTable) return;
        adminTicketsTable->setRowCount(0);

        int total = 0, pending = 0, resolved = 0;
        QString statusFilter = statusFilterCombo ? statusFilterCombo->currentText() : "Tous statuts";
        QString roleFilter = roleFilterCombo ? roleFilterCombo->currentText() : "Tous rôles";

        int row = 0;
        for (auto it = allTickets.rbegin(); it != allTickets.rend(); ++it) {
            total++;
            if (it->statut == "Résolu") resolved++;
            else pending++;

            // Filtering
            if (statusFilter != "Tous statuts" && QString::fromStdString(it->statut) != statusFilter) {
                continue;
            }
            if (roleFilter != "Tous rôles" && QString::fromStdString(it->typeCompte) != roleFilter) {
                continue;
            }

            adminTicketsTable->insertRow(row);

            QString expInfo = QString("%1 (%2)").arg(QString::fromStdString(it->nomExpediteur), QString::fromStdString(it->expediteurId));
            adminTicketsTable->setItem(row, 0, new QTableWidgetItem(expInfo));

            QTableWidgetItem *roleItem = new QTableWidgetItem(QString::fromStdString(it->typeCompte));
            roleItem->setTextAlignment(Qt::AlignCenter);
            adminTicketsTable->setItem(row, 1, roleItem);

            adminTicketsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(it->sujet)));

            QTableWidgetItem *dateItem = new QTableWidgetItem(QString::fromStdString(it->date));
            dateItem->setTextAlignment(Qt::AlignCenter);
            adminTicketsTable->setItem(row, 3, dateItem);

            // Badge Statut
            QWidget *badgeWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);

            QLabel *badge = new QLabel(QString::fromStdString(it->statut));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(95);
            badge->setFixedHeight(24);

            if (it->statut == "Résolu") {
                badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 12px; font-size: 11px;");
            } else if (it->statut == "En cours") {
                badge->setStyleSheet("background-color: #FEEBC8; color: #7B341E; font-weight: bold; border-radius: 12px; font-size: 11px;");
            } else {
                badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 12px; font-size: 11px;");
            }
            bLayout->addWidget(badge);
            adminTicketsTable->setCellWidget(row, 4, badgeWidget);

            // View action button
            QPushButton *viewBtn = new QPushButton("👁️ Consulter");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                   "QPushButton:hover { background-color: #1a3353; }");
            QString tId = QString::fromStdString(it->id);
            connect(viewBtn, &QPushButton::clicked, [this, tId]() { onViewMessageClicked(tId); });
            adminTicketsTable->setCellWidget(row, 5, viewBtn);

            // Change status button
            QPushButton *changeStatusBtn = new QPushButton("🔄 Modifier statut");
            changeStatusBtn->setCursor(Qt::PointingHandCursor);
            changeStatusBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                          "QPushButton:hover { background-color: #1a3353; }");
            QString curSt = QString::fromStdString(it->statut);
            connect(changeStatusBtn, &QPushButton::clicked, [this, tId, curSt]() { onChangeStatusClicked(tId, curSt); });
            adminTicketsTable->setCellWidget(row, 6, changeStatusBtn);

            row++;
        }

        if (totalTicketsLabel) totalTicketsLabel->setText(QString::number(total));
        if (pendingTicketsLabel) pendingTicketsLabel->setText(QString::number(pending));
        if (resolvedTicketsLabel) resolvedTicketsLabel->setText(QString::number(resolved));
    }
}

void ModuleSupport::onChangeStatusClicked(const QString &ticketId, const QString &currentStatus)
{
    QStringList statuses = {"Non traité", "En cours", "Résolu"};
    int curIdx = statuses.indexOf(currentStatus);
    if (curIdx < 0) curIdx = 0;

    bool ok = false;
    QString selected = QInputDialog::getItem(this, "Modifier le statut du ticket",
                                            QString("Sélectionnez le nouveau statut pour le ticket #%1 :").arg(ticketId),
                                            statuses, curIdx, false, &ok);
    if (ok && !selected.isEmpty()) {
        mettreAJourStatutTicket(ticketId.toStdString(), selected.toStdString());
        refreshData();
    }
}

void ModuleSupport::onViewMessageClicked(const QString &ticketId)
{
    std::vector<TicketSupport> all = chargerTicketsSupport();
    for (const auto &t : all) {
        if (t.id == ticketId.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("Détail du Ticket - %1").arg(ticketId));
            dlg.setMinimumWidth(500);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(12);

            QLabel *tLbl = new QLabel(QString("<b>Sujet :</b> %1").arg(QString::fromStdString(t.sujet)), &dlg);
            tLbl->setWordWrap(true);
            QLabel *eLbl = new QLabel(QString("<b>De :</b> %1 (%2 - %3)").arg(QString::fromStdString(t.nomExpediteur), QString::fromStdString(t.expediteurId), QString::fromStdString(t.typeCompte)), &dlg);
            QLabel *dLbl = new QLabel(QString("<b>Date :</b> %1 | <b>Statut :</b> %2").arg(QString::fromStdString(t.date), QString::fromStdString(t.statut)), &dlg);

            QTextEdit *mEdit = new QTextEdit(&dlg);
            mEdit->setReadOnly(true);
            mEdit->setText(QString::fromStdString(t.message));
            mEdit->setMinimumHeight(180);
            mEdit->setStyleSheet("QTextEdit { background-color: #f8fafc; color: #2d3748; border: 1px solid #cbd5e0; border-radius: 6px; padding: 8px; font-size: 13px; }");

            QPushButton *closeBtn = new QPushButton("Fermer", &dlg);
            closeBtn->setFixedHeight(36);
            closeBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; }");
            connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(eLbl);
            l->addWidget(dLbl);
            l->addWidget(mEdit);
            l->addWidget(closeBtn);

            dlg.exec();
            return;
        }
    }
}
