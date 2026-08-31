#include "gestionreunions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QMessageBox>
#include <QDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <fstream>
#include <sstream>
#include <algorithm>

static const std::string FICHIER_REUNIONS = "reunions.txt";
static const char SEPARATEUR_REUNION = '~';

static std::vector<std::string> decouperReunion(const std::string &texte, char sep) {
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
        else if (c == SEPARATEUR_REUNION) res += "-";
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

ReunionComite::ReunionComite(const std::string &id, const std::string &titre, const std::string &date,
                             const std::string &heure, const std::string &lieu, const std::string &desc,
                             const std::vector<std::string> &destinataires, const std::string &statut)
    : id(id), titre(titre), date(date), heure(heure), lieu(lieu),
      description(desc), destinataires(destinataires), statut(statut) {}

bool ReunionComite::estDestinataire(const std::string &roleOuPoste, const std::string &faculte) const {
    if (destinataires.empty()) return true;
    for (const auto &d : destinataires) {
        if (d == "Tous les administrateurs" || d == "Tous" || d == roleOuPoste) {
            return true;
        }
        if (!faculte.empty() && d.find(faculte) != std::string::npos) {
            return true;
        }
        if (roleOuPoste.find(d) != std::string::npos || d.find(roleOuPoste) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string ReunionComite::destinatairesChaine() const {
    if (destinataires.empty()) return "Tous les membres";
    std::string s;
    for (size_t i = 0; i < destinataires.size(); ++i) {
        s += destinataires[i];
        if (i + 1 < destinataires.size()) s += ", ";
    }
    return s;
}

std::string ReunionComite::toLigne() const {
    std::ostringstream oss;
    oss << id << SEPARATEUR_REUNION
        << echapperTexte(titre) << SEPARATEUR_REUNION
        << date << SEPARATEUR_REUNION
        << heure << SEPARATEUR_REUNION
        << echapperTexte(lieu) << SEPARATEUR_REUNION
        << echapperTexte(description) << SEPARATEUR_REUNION;

    for (size_t i = 0; i < destinataires.size(); ++i) {
        oss << destinataires[i];
        if (i + 1 < destinataires.size()) oss << ";";
    }
    oss << SEPARATEUR_REUNION << statut;

    return oss.str();
}

ReunionComite ReunionComite::fromLigne(const std::string &ligne) {
    auto c = decouperReunion(ligne, SEPARATEUR_REUNION);
    if (c.size() < 7) return ReunionComite();

    std::string id = c[0];
    std::string titre = restaurerTexte(c[1]);
    std::string date = c[2];
    std::string heure = c[3];
    std::string lieu = restaurerTexte(c[4]);
    std::string desc = restaurerTexte(c[5]);

    std::vector<std::string> dests;
    if (!c[6].empty()) {
        dests = decouperReunion(c[6], ';');
    }

    std::string statut = (c.size() >= 8) ? c[7] : "Convoquée";

    return ReunionComite(id, titre, date, heure, lieu, desc, dests, statut);
}

std::vector<ReunionComite> chargerReunionsComite() {
    std::vector<ReunionComite> liste;
    std::ifstream f(FICHIER_REUNIONS);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        ReunionComite r = ReunionComite::fromLigne(ligne);
        if (!r.id.empty()) {
            liste.push_back(r);
        }
    }
    return liste;
}

void sauvegarderReunionsComite(const std::vector<ReunionComite> &reunions) {
    std::ofstream f(FICHIER_REUNIONS, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &r : reunions) {
        f << r.toLigne() << "\n";
    }
}

void ajouterReunionComite(const ReunionComite &reunion) {
    std::vector<ReunionComite> liste = chargerReunionsComite();
    liste.push_back(reunion);
    sauvegarderReunionsComite(liste);
}

bool supprimerReunionComite(const std::string &idReunion) {
    std::vector<ReunionComite> liste = chargerReunionsComite();
    auto it = std::remove_if(liste.begin(), liste.end(), [&](const ReunionComite &r) {
        return r.id == idReunion;
    });
    if (it != liste.end()) {
        liste.erase(it, liste.end());
        sauvegarderReunionsComite(liste);
        return true;
    }
    return false;
}

// ============================================================
//  GestionReunions Implementation (Mode Secrétaire)
// ============================================================
GestionReunions::GestionReunions(QWidget *parent)
    : QWidget(parent), titleEdit(nullptr), dateEdit(nullptr),
      timeEdit(nullptr), placeEdit(nullptr), descEdit(nullptr), reunionsTable(nullptr)
{
    setupUI();
    refreshData();
}

void GestionReunions::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *content = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(18);

    // Form Card
    QFrame *formCard = new QFrame(content);
    formCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; } "
                            "QLabel { color: #1a202c; font-size: 13px; font-weight: bold; background: transparent; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(formCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    formCard->setGraphicsEffect(shadow);

    QVBoxLayout *fLayout = new QVBoxLayout(formCard);
    fLayout->setContentsMargins(22, 18, 22, 18);
    fLayout->setSpacing(12);

    QLabel *formTitle = new QLabel("👥 Convoquer une Réunion avec le Comité Administratif", formCard);
    formTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36; border: none;");
    fLayout->addWidget(formTitle);

    // Grid inputs
    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(15);
    grid->setVerticalSpacing(10);

    auto makeFieldLabel = [formCard](const QString &text) {
        QLabel *l = new QLabel(text, formCard);
        l->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 13px; border: none; background: transparent;");
        return l;
    };

    grid->addWidget(makeFieldLabel("Titre de la réunion :"), 0, 0);
    titleEdit = new QLineEdit(formCard);
    titleEdit->setPlaceholderText("Ex: Comité de direction, Bilan semestriel...");
    titleEdit->setFixedHeight(36);
    titleEdit->setStyleSheet("QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }"
                             "QLineEdit:focus { border: 1.5px solid #2b6cb0; }");
    grid->addWidget(titleEdit, 0, 1, 1, 3);

    grid->addWidget(makeFieldLabel("Date :"), 1, 0);
    dateEdit = new QDateEdit(QDate::currentDate(), formCard);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setFixedHeight(36);
    dateEdit->setStyleSheet("QDateEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }");
    grid->addWidget(dateEdit, 1, 1);

    grid->addWidget(makeFieldLabel("Heure :"), 1, 2);
    timeEdit = new QTimeEdit(QTime(10, 0), formCard);
    timeEdit->setDisplayFormat("HH:mm");
    timeEdit->setFixedHeight(36);
    timeEdit->setStyleSheet("QTimeEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }");
    grid->addWidget(timeEdit, 1, 3);

    grid->addWidget(makeFieldLabel("Lieu / Salle :"), 2, 0);
    placeEdit = new QLineEdit(formCard);
    placeEdit->setPlaceholderText("Ex: Salle du Conseil / Bâtiment Principal ou En ligne");
    placeEdit->setFixedHeight(36);
    placeEdit->setStyleSheet("QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }");
    grid->addWidget(placeEdit, 2, 1, 1, 3);

    grid->addWidget(makeFieldLabel("Ordre du jour :"), 3, 0);
    descEdit = new QTextEdit(formCard);
    descEdit->setPlaceholderText("Précisez les points à l'ordre du jour et documents à préparer...");
    descEdit->setFixedHeight(70);
    descEdit->setStyleSheet("QTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px; font-size: 12px; }");
    grid->addWidget(descEdit, 3, 1, 1, 3);

    fLayout->addLayout(grid);

    // Destinataires checkboxes
    QLabel *recipTitle = new QLabel("Destinataires à convoquer (Postes administratifs) :", formCard);
    recipTitle->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 13px; margin-top: 5px; border: none; background: transparent;");
    fLayout->addWidget(recipTitle);

    QHBoxLayout *cbLayout = new QHBoxLayout();
    cbLayout->setSpacing(15);

    QStringList roles = {"Recteur", "Président", "Secrétaire", "Coordonnateurs de faculté", "Tous les administrateurs"};
    recipientBoxes.clear();
    for (const auto &r : roles) {
        QCheckBox *cb = new QCheckBox(r, formCard);
        cb->setChecked(r == "Tous les administrateurs");
        cb->setStyleSheet("QCheckBox { font-size: 12px; font-weight: bold; color: #1a202c; } "
                          "QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #718096; border-radius: 3px; background-color: #ffffff; } "
                          "QCheckBox::indicator:checked { background-color: #0b1e36; border: 1.5px solid #0b1e36; }");
        recipientBoxes.push_back(cb);
        cbLayout->addWidget(cb);
    }
    cbLayout->addStretch();
    fLayout->addLayout(cbLayout);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton *sendBtn = new QPushButton("✉️ Envoyer la convocation", formCard);
    sendBtn->setFixedHeight(38);
    sendBtn->setFixedWidth(200);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; border: none; }"
                           "QPushButton:hover { background-color: #1a3353; }");
    connect(sendBtn, &QPushButton::clicked, this, &GestionReunions::onSendConvocationClicked);
    btnRow->addWidget(sendBtn);

    fLayout->addLayout(btnRow);
    mainLayout->addWidget(formCard);

    // Table Card
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow2 = new QGraphicsDropShadowEffect(tableCard);
    shadow2->setBlurRadius(15);
    shadow2->setColor(QColor(0, 0, 0, 15));
    shadow2->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow2);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(20, 16, 20, 16);
    tLayout->setSpacing(12);

    QLabel *tTitle = new QLabel("📋 Historique des Réunions et Convocations du Comité", tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    tLayout->addWidget(tTitle);

    reunionsTable = new QTableWidget(tableCard);
    reunionsTable->setColumnCount(6);
    reunionsTable->setHorizontalHeaderLabels({"Date & Heure", "Titre", "Lieu", "Destinataires", "Détail", "Action"});
    reunionsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    reunionsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    reunionsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    reunionsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    reunionsTable->verticalHeader()->setVisible(false);
    reunionsTable->verticalHeader()->setDefaultSectionSize(38);
    reunionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reunionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reunionsTable->setAlternatingRowColors(true);
    reunionsTable->setShowGrid(false);
    reunionsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(reunionsTable);
    mainLayout->addWidget(tableCard);

    scroll->setWidget(content);
    rootLayout->addWidget(scroll);
}

void GestionReunions::onSendConvocationClicked()
{
    if (!titleEdit || !dateEdit || !timeEdit || !placeEdit || !descEdit) return;

    QString titre = titleEdit->text().trimmed();
    QString lieu = placeEdit->text().trimmed();
    QString desc = descEdit->toPlainText().trimmed();
    QString dateStr = dateEdit->date().toString("dd/MM/yyyy");
    QString timeStr = timeEdit->time().toString("HH:mm");

    if (titre.isEmpty() || lieu.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir le titre et le lieu de la réunion.");
        return;
    }

    std::vector<std::string> selectedDest;
    for (auto cb : recipientBoxes) {
        if (cb && cb->isChecked()) {
            selectedDest.push_back(cb->text().toStdString());
        }
    }

    if (selectedDest.empty()) {
        selectedDest.push_back("Tous les administrateurs");
    }

    std::string rId = "REU-" + std::to_string(QDateTime::currentMSecsSinceEpoch() % 1000000);
    ReunionComite r(rId, titre.toStdString(), dateStr.toStdString(), timeStr.toStdString(),
                    lieu.toStdString(), desc.toStdString(), selectedDest, "Convoquée");

    ajouterReunionComite(r);

    titleEdit->clear();
    placeEdit->clear();
    descEdit->clear();

    QMessageBox::information(this, "Convocation envoyée",
                             "La réunion a été enregistrée et la convocation a été transmise aux membres concernés.");

    refreshData();
}

void GestionReunions::onDeleteReunionClicked(const QString &idReunion)
{
    auto reply = QMessageBox::question(this, "Confirmation", "Voulez-vous supprimer cette réunion ?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        supprimerReunionComite(idReunion.toStdString());
        refreshData();
    }
}

void GestionReunions::onViewDetailsClicked(const QString &idReunion)
{
    std::vector<ReunionComite> list = chargerReunionsComite();
    for (const auto &r : list) {
        if (r.id == idReunion.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("Détail de la Réunion - %1").arg(idReunion));
            dlg.setMinimumWidth(480);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(10);

            QLabel *tLbl = new QLabel(QString("<h3>%1</h3>").arg(QString::fromStdString(r.titre)), &dlg);
            QLabel *dtLbl = new QLabel(QString("<b>📅 Date & Heure :</b> %1 à %2").arg(QString::fromStdString(r.date), QString::fromStdString(r.heure)), &dlg);
            QLabel *lLbl = new QLabel(QString("<b>📍 Lieu :</b> %1").arg(QString::fromStdString(r.lieu)), &dlg);
            QLabel *destLbl = new QLabel(QString("<b>👥 Convoqués :</b> %1").arg(QString::fromStdString(r.destinatairesChaine())), &dlg);
            destLbl->setWordWrap(true);

            QTextEdit *dEdit = new QTextEdit(&dlg);
            dEdit->setReadOnly(true);
            dEdit->setText(QString::fromStdString(r.description));
            dEdit->setFixedHeight(120);
            dEdit->setStyleSheet("QTextEdit { background-color: #f8fafc; color: #2d3748; border: 1px solid #cbd5e0; border-radius: 6px; padding: 6px; font-size: 13px; }");

            QPushButton *okBtn = new QPushButton("Fermer", &dlg);
            okBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; padding: 6px; font-weight: bold; }");
            connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(dtLbl);
            l->addWidget(lLbl);
            l->addWidget(destLbl);
            l->addWidget(new QLabel("<b>Ordre du jour & notes :</b>", &dlg));
            l->addWidget(dEdit);
            l->addWidget(okBtn);

            dlg.exec();
            return;
        }
    }
}

void GestionReunions::refreshData()
{
    if (!reunionsTable) return;
    std::vector<ReunionComite> list = chargerReunionsComite();
    reunionsTable->setRowCount(0);

    for (size_t i = 0; i < list.size(); ++i) {
        const auto &r = list[i];
        reunionsTable->insertRow(i);

        QString dtStr = QString("%1 %2").arg(QString::fromStdString(r.date), QString::fromStdString(r.heure));
        QTableWidgetItem *dtItem = new QTableWidgetItem(dtStr);
        dtItem->setTextAlignment(Qt::AlignCenter);
        reunionsTable->setItem(i, 0, dtItem);

        reunionsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(r.titre)));
        reunionsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r.lieu)));
        reunionsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(r.destinatairesChaine())));

        QPushButton *viewBtn = new QPushButton("👁️ Détails");
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                               "QPushButton:hover { background-color: #1a3353; }");
        QString rId = QString::fromStdString(r.id);
        connect(viewBtn, &QPushButton::clicked, [this, rId]() { onViewDetailsClicked(rId); });
        reunionsTable->setCellWidget(i, 4, viewBtn);

        QPushButton *delBtn = new QPushButton("🗑️ Supprimer");
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                              "QPushButton:hover { background-color: #1a3353; }");
        connect(delBtn, &QPushButton::clicked, [this, rId]() { onDeleteReunionClicked(rId); });
        reunionsTable->setCellWidget(i, 5, delBtn);
    }
}

// ============================================================
//  ReunionsAdminView Implementation (Mode Consultation Admin)
// ============================================================
ReunionsAdminView::ReunionsAdminView(const std::string &posteAdmin, const std::string &faculteAdmin, QWidget *parent)
    : QWidget(parent), poste(posteAdmin), faculte(faculteAdmin), reunionsTable(nullptr), totalLabel(nullptr)
{
    setupUI();
    refreshData();
}

void ReunionsAdminView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);

    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(20, 16, 20, 16);
    tLayout->setSpacing(12);

    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel(QString("🏛️ Convocations aux Réunions du Comité (%1)").arg(QString::fromStdString(poste)), tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");

    totalLabel = new QLabel("0 réunion(s)", tableCard);
    totalLabel->setStyleSheet("color: #718096; font-size: 13px; font-weight: bold;");

    headerRow->addWidget(tTitle);
    headerRow->addStretch();
    headerRow->addWidget(totalLabel);
    tLayout->addLayout(headerRow);

    reunionsTable = new QTableWidget(tableCard);
    reunionsTable->setColumnCount(5);
    reunionsTable->setHorizontalHeaderLabels({"Date & Heure", "Titre de la réunion", "Lieu", "Statut", "Détails"});
    reunionsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    reunionsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    reunionsTable->verticalHeader()->setVisible(false);
    reunionsTable->verticalHeader()->setDefaultSectionSize(38);
    reunionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reunionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reunionsTable->setAlternatingRowColors(true);
    reunionsTable->setShowGrid(false);
    reunionsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(reunionsTable);
    mainLayout->addWidget(tableCard);
}

void ReunionsAdminView::refreshData()
{
    if (!reunionsTable) return;
    std::vector<ReunionComite> all = chargerReunionsComite();
    reunionsTable->setRowCount(0);

    int count = 0;
    for (const auto &r : all) {
        if (r.estDestinataire(poste, faculte)) {
            reunionsTable->insertRow(count);

            QString dtStr = QString("%1 %2").arg(QString::fromStdString(r.date), QString::fromStdString(r.heure));
            QTableWidgetItem *dtItem = new QTableWidgetItem(dtStr);
            dtItem->setTextAlignment(Qt::AlignCenter);
            reunionsTable->setItem(count, 0, dtItem);

            reunionsTable->setItem(count, 1, new QTableWidgetItem(QString::fromStdString(r.titre)));
            reunionsTable->setItem(count, 2, new QTableWidgetItem(QString::fromStdString(r.lieu)));

            // Statut Badge
            QWidget *badgeWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);
            QLabel *badge = new QLabel(QString::fromStdString(r.statut));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(90);
            badge->setFixedHeight(22);
            badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
            bLayout->addWidget(badge);
            reunionsTable->setCellWidget(count, 3, badgeWidget);

            QPushButton *viewBtn = new QPushButton("👁️ Consulter");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                   "QPushButton:hover { background-color: #1a3353; }");
            QString rId = QString::fromStdString(r.id);
            connect(viewBtn, &QPushButton::clicked, [this, rId]() { onViewDetailsClicked(rId); });
            reunionsTable->setCellWidget(count, 4, viewBtn);

            count++;
        }
    }

    if (totalLabel) totalLabel->setText(QString("%1 réunion(s) convoquée(s)").arg(count));
}

void ReunionsAdminView::onViewDetailsClicked(const QString &idReunion)
{
    std::vector<ReunionComite> list = chargerReunionsComite();
    for (const auto &r : list) {
        if (r.id == idReunion.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("Convocation à la Réunion - %1").arg(idReunion));
            dlg.setMinimumWidth(480);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(12);

            QLabel *tLbl = new QLabel(QString("<h3>%1</h3>").arg(QString::fromStdString(r.titre)), &dlg);
            QLabel *dtLbl = new QLabel(QString("<b>📅 Date & Heure :</b> %1 à %2").arg(QString::fromStdString(r.date), QString::fromStdString(r.heure)), &dlg);
            QLabel *lLbl = new QLabel(QString("<b>📍 Lieu :</b> %1").arg(QString::fromStdString(r.lieu)), &dlg);

            QTextEdit *dEdit = new QTextEdit(&dlg);
            dEdit->setReadOnly(true);
            dEdit->setText(QString::fromStdString(r.description));
            dEdit->setFixedHeight(140);
            dEdit->setStyleSheet("QTextEdit { background-color: #f8fafc; color: #2d3748; border: 1px solid #cbd5e0; border-radius: 6px; padding: 8px; font-size: 13px; }");

            QPushButton *okBtn = new QPushButton("Fermer", &dlg);
            okBtn->setFixedHeight(36);
            okBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; }");
            connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(dtLbl);
            l->addWidget(lLbl);
            l->addWidget(new QLabel("<b>Ordre du jour :</b>", &dlg));
            l->addWidget(dEdit);
            l->addWidget(okBtn);

            dlg.exec();
            return;
        }
    }
}
