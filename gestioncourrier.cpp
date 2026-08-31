#include "gestioncourrier.h"
#include "tresorerie.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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

static const std::string FICHIER_COURRIERS = "courriers.txt";
static const char SEPARATEUR_COURRIER = '~';

static std::vector<std::string> decouperCourrier(const std::string &texte, char sep) {
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
        else if (c == SEPARATEUR_COURRIER) res += "-";
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

Courrier::Courrier(const std::string &id, const std::string &type, const std::string &exp,
                   const std::string &dest, const std::string &titre, const std::string &contenu,
                   const std::string &date, const std::string &statut)
    : id(id), type(type), expediteur(exp), destinataire(dest),
      titre(titre), contenu(contenu), date(date), statut(statut) {}

std::string Courrier::toLigne() const {
    std::ostringstream oss;
    oss << id << SEPARATEUR_COURRIER
        << type << SEPARATEUR_COURRIER
        << expediteur << SEPARATEUR_COURRIER
        << destinataire << SEPARATEUR_COURRIER
        << echapperTexte(titre) << SEPARATEUR_COURRIER
        << echapperTexte(contenu) << SEPARATEUR_COURRIER
        << date << SEPARATEUR_COURRIER
        << statut;
    return oss.str();
}

Courrier Courrier::fromLigne(const std::string &ligne) {
    auto c = decouperCourrier(ligne, SEPARATEUR_COURRIER);
    if (c.size() < 7) return Courrier();

    std::string id = c[0];
    std::string type = c[1];
    std::string exp = c[2];
    std::string dest = c[3];
    std::string titre = restaurerTexte(c[4]);
    std::string contenu = restaurerTexte(c[5]);
    std::string date = c[6];
    std::string statut = (c.size() >= 8) ? c[7] : "Envoyé";

    return Courrier(id, type, exp, dest, titre, contenu, date, statut);
}

std::vector<Courrier> chargerCourriers() {
    std::vector<Courrier> liste;
    std::ifstream f(FICHIER_COURRIERS);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        Courrier c = Courrier::fromLigne(ligne);
        if (!c.id.empty()) {
            liste.push_back(c);
        }
    }
    return liste;
}

void sauvegarderCourriers(const std::vector<Courrier> &courriers) {
    std::ofstream f(FICHIER_COURRIERS, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &c : courriers) {
        f << c.toLigne() << "\n";
    }
}

void ajouterCourrier(const Courrier &courrier) {
    std::vector<Courrier> liste = chargerCourriers();
    liste.push_back(courrier);
    sauvegarderCourriers(liste);
}

void marquerCourrierLu(const std::string &idCourrier) {
    std::vector<Courrier> liste = chargerCourriers();
    for (auto &c : liste) {
        if (c.id == idCourrier) {
            c.statut = "Lu";
            break;
        }
    }
    sauvegarderCourriers(liste);
}

// ============================================================
//  GestionCourrier Implementation
// ============================================================
GestionCourrier::GestionCourrier(std::vector<Professeur> &profsRef, std::vector<Administrateur> &adminsRef, QWidget *parent)
    : QWidget(parent), profs(profsRef), admins(adminsRef), tabWidget(nullptr),
      crTitleEdit(nullptr), crDateEdit(nullptr), crContentEdit(nullptr),
      paiementsPreviewLabel(nullptr), paiementsPreviewText(nullptr), outboxTable(nullptr)
{
    setupUI();
    refreshData();
}

void GestionCourrier::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; background: #ffffff; border-radius: 10px; top: -1px; }"
        "QTabBar::tab { background: #edf2f7; color: #4a5568; padding: 10px 22px; font-weight: bold; font-size: 13px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0b1e36; border-bottom: 3px solid #0b1e36; }"
        "QTabBar::tab:hover:!selected { background: #e2e8f0; }"
    );

    tabWidget->addTab(createCompteRenduTab(), "📝 Compte-rendu → Président");
    tabWidget->addTab(createPaiementsTab(), "💰 Liste des paiements → Recteur");
    tabWidget->addTab(createOutboxTab(), "📬 Boîte d'envoi générale");

    mainLayout->addWidget(tabWidget);
}

QWidget* GestionCourrier::createCompteRenduTab()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *title = new QLabel("Transmission de Compte-rendu au Président", tab);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36;");
    layout->addWidget(title);

    auto makeCrLabel = [tab](const QString &text) {
        QLabel *l = new QLabel(text, tab);
        l->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 13px; border: none; background: transparent;");
        return l;
    };

    QHBoxLayout *h = new QHBoxLayout();
    QVBoxLayout *c1 = new QVBoxLayout();
    c1->addWidget(makeCrLabel("Titre du compte-rendu :"));
    crTitleEdit = new QLineEdit(tab);
    crTitleEdit->setPlaceholderText("Ex: Compte-rendu de la réunion du Conseil de Faculté...");
    crTitleEdit->setFixedHeight(36);
    crTitleEdit->setStyleSheet("QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }"
                               "QLineEdit:focus { border: 1.5px solid #2b6cb0; }");
    c1->addWidget(crTitleEdit);

    QVBoxLayout *c2 = new QVBoxLayout();
    c2->addWidget(makeCrLabel("Date de la réunion :"));
    crDateEdit = new QDateEdit(QDate::currentDate(), tab);
    crDateEdit->setCalendarPopup(true);
    crDateEdit->setDisplayFormat("dd/MM/yyyy");
    crDateEdit->setFixedHeight(36);
    crDateEdit->setStyleSheet("QDateEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }");
    c2->addWidget(crDateEdit);

    h->addLayout(c1, 3);
    h->addLayout(c2, 1);
    layout->addLayout(h);

    layout->addWidget(makeCrLabel("Contenu et décisions prises :"));
    crContentEdit = new QTextEdit(tab);
    crContentEdit->setPlaceholderText("Rédigez ou collez ici la synthèse exhaustive de la séance...");
    crContentEdit->setMinimumHeight(240);
    crContentEdit->setStyleSheet("QTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 10px; font-size: 13px; }");
    layout->addWidget(crContentEdit, 1);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton *sendBtn = new QPushButton("🚀 Envoyer au Président", tab);
    sendBtn->setFixedHeight(40);
    sendBtn->setFixedWidth(200);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; border: none; }"
                           "QPushButton:hover { background-color: #1a3353; }");
    connect(sendBtn, &QPushButton::clicked, this, &GestionCourrier::onSendCompteRenduClicked);
    btnRow->addWidget(sendBtn);

    layout->addLayout(btnRow);
    scroll->setWidget(tab);
    return scroll;
}

QWidget* GestionCourrier::createPaiementsTab()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(14);

    QLabel *title = new QLabel("Transmission Quotidienne de la Liste des Paiements au Recteur", tab);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36;");
    layout->addWidget(title);

    paiementsPreviewLabel = new QLabel("Aperçu du récapitulatif généré :", tab);
    paiementsPreviewLabel->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 14px;");
    layout->addWidget(paiementsPreviewLabel);

    paiementsPreviewText = new QTextEdit(tab);
    paiementsPreviewText->setReadOnly(true);
    paiementsPreviewText->setMinimumHeight(350);
    paiementsPreviewText->setStyleSheet("QTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 12px; font-family: 'Consolas', 'Courier New', monospace; font-size: 13px; font-weight: bold; }");
    layout->addWidget(paiementsPreviewText, 1);

    QHBoxLayout *btnRow = new QHBoxLayout();
    QPushButton *refreshBtn = new QPushButton("🔄 Actualiser les calculs", tab);
    refreshBtn->setFixedHeight(40);
    refreshBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; padding: 0 15px; border: none; }"
                              "QPushButton:hover { background-color: #1a3353; }");
    connect(refreshBtn, &QPushButton::clicked, this, &GestionCourrier::refreshData);
    btnRow->addWidget(refreshBtn);

    btnRow->addStretch();

    QPushButton *sendBtn = new QPushButton("📤 Transmettre le rapport au Recteur", tab);
    sendBtn->setFixedHeight(40);
    sendBtn->setFixedWidth(260);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; border: none; }"
                           "QPushButton:hover { background-color: #1a3353; }");
    connect(sendBtn, &QPushButton::clicked, this, &GestionCourrier::onSendPaiementsListClicked);
    btnRow->addWidget(sendBtn);

    layout->addLayout(btnRow);
    scroll->setWidget(tab);
    return scroll;
}

QWidget* GestionCourrier::createOutboxTab()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Historique de tous les courriers et transmissions", tab);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c;");
    layout->addWidget(title);

    outboxTable = new QTableWidget(tab);
    outboxTable->setColumnCount(6);
    outboxTable->setHorizontalHeaderLabels({"Type", "Destinataire", "Titre", "Date d'envoi", "Statut", "Détail"});
    outboxTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    outboxTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    outboxTable->verticalHeader()->setVisible(false);
    outboxTable->verticalHeader()->setDefaultSectionSize(38);
    outboxTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    outboxTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    outboxTable->setAlternatingRowColors(true);
    outboxTable->setShowGrid(false);
    outboxTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    layout->addWidget(outboxTable);
    scroll->setWidget(tab);
    return scroll;
}

void GestionCourrier::onSendCompteRenduClicked()
{
    if (!crTitleEdit || !crContentEdit || !crDateEdit) return;

    QString titre = crTitleEdit->text().trimmed();
    QString contenu = crContentEdit->toPlainText().trimmed();
    QString dateReunion = crDateEdit->date().toString("dd/MM/yyyy");

    if (titre.isEmpty() || contenu.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez saisir le titre et le contenu du compte-rendu.");
        return;
    }

    std::string cId = "CR-" + std::to_string(QDateTime::currentMSecsSinceEpoch() % 1000000);
    std::string nowStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm").toStdString();
    std::string fullTitre = QString("%1 (Séance du %2)").arg(titre, dateReunion).toStdString();

    Courrier c(cId, "Compte-rendu", "Secrétaire", "Président", fullTitre, contenu.toStdString(), nowStr, "Envoyé");
    ajouterCourrier(c);

    crTitleEdit->clear();
    crContentEdit->clear();

    QMessageBox::information(this, "Compte-rendu transmis",
                             "Le compte-rendu a été transmis avec succès au Président (accessible dans sa section Conseil d'administration).");
    refreshData();
}

void GestionCourrier::onSendPaiementsListClicked()
{
    if (!paiementsPreviewText) return;
    QString contenu = paiementsPreviewText->toPlainText();

    std::string cId = "PAY-" + std::to_string(QDateTime::currentMSecsSinceEpoch() % 1000000);
    std::string nowStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm").toStdString();
    std::string dateDay = QDateTime::currentDateTime().toString("dd/MM/yyyy").toStdString();
    std::string titre = "Récapitulatif des Paiements & Salaires du " + dateDay;

    Courrier c(cId, "Liste des paiements", "Secrétaire", "Recteur", titre, contenu.toStdString(), nowStr, "Envoyé");
    ajouterCourrier(c);

    QMessageBox::information(this, "Liste des paiements transmise",
                             "Le récapitulatif des paiements a été envoyé avec succès au Recteur (visible dans sa section Gestion financière).");
    refreshData();
}

void GestionCourrier::onViewCourrierClicked(const QString &idCourrier)
{
    std::vector<Courrier> all = chargerCourriers();
    for (const auto &c : all) {
        if (c.id == idCourrier.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("Courrier - %1").arg(idCourrier));
            dlg.setMinimumWidth(520);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(10);

            QLabel *tLbl = new QLabel(QString("<h3>%1</h3>").arg(QString::fromStdString(c.titre)), &dlg);
            QLabel *infoLbl = new QLabel(QString("<b>Type :</b> %1 | <b>Destinataire :</b> %2 | <b>Date :</b> %3")
                                        .arg(QString::fromStdString(c.type), QString::fromStdString(c.destinataire), QString::fromStdString(c.date)), &dlg);

            QTextEdit *edit = new QTextEdit(&dlg);
            edit->setReadOnly(true);
            edit->setText(QString::fromStdString(c.contenu));
            edit->setMinimumHeight(200);
            edit->setStyleSheet("QTextEdit { background-color: #f8fafc; color: #2d3748; border: 1px solid #cbd5e0; border-radius: 6px; padding: 8px; font-size: 13px; }");

            QPushButton *okBtn = new QPushButton("Fermer", &dlg);
            okBtn->setStyleSheet("QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: none; outline: none; }"
                                 "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }");
            connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(infoLbl);
            l->addWidget(edit);
            l->addWidget(okBtn);

            dlg.exec();
            return;
        }
    }
}

void GestionCourrier::refreshData()
{
    // Reload fresh profs & admins
    profs = chargerProfesseurs();
    admins = chargerAdministrateurs();

    // 1. Generate live payment preview
    if (paiementsPreviewText) {
        double totalProfPaye = 0.0;
        int nbProfPayes = 0;
        int nbProfTotal = profs.size();

        double totalAdminPaye = 0.0;
        int nbAdminPayes = 0;
        int nbAdminTotal = admins.size();

        QString text = "========================================================================\n";
        text += "           UNIVERSITÉ DU HAUT (UDH) - RAPPORT DES PAIEMENTS DU JOUR\n";
        text += QString("           Généré le : %1\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm"));
        text += "========================================================================\n\n";

        text += "1. CORPS ENSEIGNANT (PROFESSEURS)\n";
        text += "------------------------------------------------------------------------\n";
        text += QString("%1 %2 %3 %4\n")
                    .arg("ID", -10)
                    .arg("Nom / Prénom", -25)
                    .arg("Statut Paiement", -18)
                    .arg("Montant / Date");
        text += "------------------------------------------------------------------------\n";

        for (const auto &p : profs) {
            QString statutStr = p.getEstPaye() ? "PAYÉ" : "NON PAYÉ";
            QString dateMontant = p.getEstPaye() ? QString("%1 HTG (%2)").arg(p.getMontantSalaire()).arg(QString::fromStdString(p.getDatePaiement())) : "-";
            text += QString("%1 %2 %3 %4\n")
                        .arg(QString::fromStdString(p.getId()), -10)
                        .arg(QString::fromStdString(p.getNom() + " " + p.getPrenom()), -25)
                        .arg(statutStr, -18)
                        .arg(dateMontant);
            if (p.getEstPaye()) {
                totalProfPaye += p.getMontantSalaire();
                nbProfPayes++;
            }
        }

        text += "\n2. PERSONNEL ADMINISTRATIF\n";
        text += "------------------------------------------------------------------------\n";
        text += QString("%1 %2 %3 %4\n")
                    .arg("Poste", -22)
                    .arg("Nom / Prénom", -22)
                    .arg("Statut", -14)
                    .arg("Montant / Date");
        text += "------------------------------------------------------------------------\n";

        for (const auto &a : admins) {
            QString statutStr = a.getEstPaye() ? "PAYÉ" : "NON PAYÉ";
            QString dateMontant = a.getEstPaye() ? QString("%1 HTG (%2)").arg(a.getMontantSalaire()).arg(QString::fromStdString(a.getDatePaiement())) : "-";
            text += QString("%1 %2 %3 %4\n")
                        .arg(QString::fromStdString(a.getPoste()), -22)
                        .arg(QString::fromStdString(a.getNom() + " " + a.getPrenom()), -22)
                        .arg(statutStr, -14)
                        .arg(dateMontant);
            if (a.getEstPaye()) {
                totalAdminPaye += a.getMontantSalaire();
                nbAdminPayes++;
            }
        }

        // Recettes Étudiantes (Frais de scolarité)
        std::vector<VersementEtudiant> versements = chargerVersements();
        double totalRecettes = 0.0;
        int nbVersementsValides = 0;
        std::map<std::string, double> parMethode;

        for (const auto &v : versements) {
            if (v.statut == "Validé" || v.statut == "Confirmé") {
                totalRecettes += v.montant;
                nbVersementsValides++;
                std::string m = v.methode.empty() ? "Autre" : v.methode;
                parMethode[m] += v.montant;
            }
        }

        text += "\n3. RECETTES ÉTUDIANTES (FRAIS DE SCOLARITÉ & INSCRIPTIONS)\n";
        text += "------------------------------------------------------------------------\n";
        text += QString("%1 %2 %3 %4\n")
                    .arg("ID Étudiant", -14)
                    .arg("Nom / Prénom", -24)
                    .arg("Méthode", -18)
                    .arg("Montant / Date");
        text += "------------------------------------------------------------------------\n";

        if (nbVersementsValides == 0) {
            text += "Aucun versement étudiant validé enregistré à ce jour.\n";
        } else {
            // Afficher les 10 derniers versements validés
            int countShown = 0;
            for (auto it = versements.rbegin(); it != versements.rend() && countShown < 10; ++it) {
                if (it->statut == "Validé" || it->statut == "Confirmé") {
                    text += QString("%1 %2 %3 %4\n")
                                .arg(QString::fromStdString(it->idEtudiant), -14)
                                .arg(QString::fromStdString(it->nom), -24)
                                .arg(QString::fromStdString(it->methode), -18)
                                .arg(QString("%L1 HTG (%2)").arg(it->montant, 0, 'f', 2).arg(QString::fromStdString(it->date)));
                    countShown++;
                }
            }
            if (nbVersementsValides > countShown) {
                text += QString("... et %1 autre(s) versement(s) validé(s).\n").arg(nbVersementsValides - countShown);
            }
        }

        double masseSalariale = totalProfPaye + totalAdminPaye;
        double soldeDisponible = totalRecettes - masseSalariale;

        text += "\n========================================================================\n";
        text += "SYNTHÈSE FINANCIÈRE GLOBALE :\n";
        text += QString("- Recettes étudiantes cumulées : %L1 HTG (%2 versement(s) validé(s))\n").arg(totalRecettes, 0, 'f', 2).arg(nbVersementsValides);
        for (const auto &pair : parMethode) {
            text += QString("    • %1 : %L2 HTG\n").arg(QString::fromStdString(pair.first)).arg(pair.second, 0, 'f', 2);
        }
        text += QString("- Professeurs payés : %1 / %2 (Total : %L3 HTG)\n").arg(nbProfPayes).arg(nbProfTotal).arg(totalProfPaye, 0, 'f', 2);
        text += QString("- Administrateurs payés : %1 / %2 (Total : %L3 HTG)\n").arg(nbAdminPayes).arg(nbAdminTotal).arg(totalAdminPaye, 0, 'f', 2);
        text += QString("- Masse salariale versée ce mois : %L1 HTG\n").arg(masseSalariale, 0, 'f', 2);
        text += "------------------------------------------------------------------------\n";
        text += QString("Solde de trésorerie disponible : %L1 HTG\n").arg(soldeDisponible, 0, 'f', 2);
        text += QString("Diagnostic de trésorerie : %1\n").arg(soldeDisponible >= 0.0 ? "✅ Trésorerie Positive (Fonds disponibles suffisants)" : "⚠️ Trésorerie Déficitaire (Vigilance requise)");
        text += "========================================================================\n";

        paiementsPreviewText->setText(text);
    }

    // 2. Refresh Outbox table
    if (!outboxTable) return;
    std::vector<Courrier> all = chargerCourriers();
    outboxTable->setRowCount(0);

    int row = 0;
    for (auto it = all.rbegin(); it != all.rend(); ++it) {
        outboxTable->insertRow(row);

        outboxTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(it->type)));

        QTableWidgetItem *destItem = new QTableWidgetItem(QString::fromStdString(it->destinataire));
        destItem->setTextAlignment(Qt::AlignCenter);
        outboxTable->setItem(row, 1, destItem);

        outboxTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(it->titre)));

        QTableWidgetItem *dateItem = new QTableWidgetItem(QString::fromStdString(it->date));
        dateItem->setTextAlignment(Qt::AlignCenter);
        outboxTable->setItem(row, 3, dateItem);

        // Statut Badge
        QWidget *badgeWidget = new QWidget();
        QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
        bLayout->setContentsMargins(4, 2, 4, 2);
        bLayout->setAlignment(Qt::AlignCenter);
        QLabel *badge = new QLabel(QString::fromStdString(it->statut));
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedWidth(80);
        badge->setFixedHeight(22);
        if (it->statut == "Lu") {
            badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
        } else {
            badge->setStyleSheet("background-color: #BEE3F8; color: #2B6CB0; font-weight: bold; border-radius: 11px; font-size: 11px;");
        }
        bLayout->addWidget(badge);
        outboxTable->setCellWidget(row, 4, badgeWidget);

        QPushButton *viewBtn = new QPushButton("👁️ Consulter");
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                               "QPushButton:hover { background-color: #1a3353; }");
        QString cId = QString::fromStdString(it->id);
        connect(viewBtn, &QPushButton::clicked, [this, cId]() { onViewCourrierClicked(cId); });
        outboxTable->setCellWidget(row, 5, viewBtn);

        row++;
    }
}

// ============================================================
//  CourriersPresidentView Implementation
// ============================================================
CourriersPresidentView::CourriersPresidentView(QWidget *parent)
    : QWidget(parent), reportsTable(nullptr)
{
    setupUI();
    refreshData();
}

void CourriersPresidentView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(15);

    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(22, 20, 22, 20);
    tLayout->setSpacing(12);

    QLabel *tTitle = new QLabel("👥 Conseil d'Administration — Comptes-rendus reçus de la Secrétaire", tableCard);
    tTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #0b1e36; border: none;");
    tLayout->addWidget(tTitle);

    reportsTable = new QTableWidget(tableCard);
    reportsTable->setColumnCount(4);
    reportsTable->setHorizontalHeaderLabels({"Date de transmission", "Titre du compte-rendu", "Statut", "Action"});
    reportsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    reportsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    reportsTable->verticalHeader()->setVisible(false);
    reportsTable->verticalHeader()->setDefaultSectionSize(38);
    reportsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reportsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reportsTable->setAlternatingRowColors(true);
    reportsTable->setShowGrid(false);
    reportsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(reportsTable);
    mainLayout->addWidget(tableCard);
}

void CourriersPresidentView::refreshData()
{
    if (!reportsTable) return;
    std::vector<Courrier> all = chargerCourriers();
    reportsTable->setRowCount(0);

    int count = 0;
    for (auto it = all.rbegin(); it != all.rend(); ++it) {
        if (it->destinataire == "Président" || it->destinataire == "Tous" || it->type == "Compte-rendu") {
            reportsTable->insertRow(count);

            QTableWidgetItem *dItem = new QTableWidgetItem(QString::fromStdString(it->date));
            dItem->setTextAlignment(Qt::AlignCenter);
            reportsTable->setItem(count, 0, dItem);

            reportsTable->setItem(count, 1, new QTableWidgetItem(QString::fromStdString(it->titre)));

            QWidget *badgeWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);
            QLabel *badge = new QLabel(QString::fromStdString(it->statut));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(80);
            badge->setFixedHeight(22);
            if (it->statut == "Lu") {
                badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
            } else {
                badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 11px; font-size: 11px;");
            }
            bLayout->addWidget(badge);
            reportsTable->setCellWidget(count, 2, badgeWidget);

            QPushButton *viewBtn = new QPushButton("📖 Lire le compte-rendu");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold; font-size: 12px; border: none; }"
                                   "QPushButton:hover { background-color: #1a3353; }");
            QString cId = QString::fromStdString(it->id);
            connect(viewBtn, &QPushButton::clicked, [this, cId]() { onViewReportClicked(cId); });
            reportsTable->setCellWidget(count, 3, viewBtn);

            count++;
        }
    }
}

void CourriersPresidentView::onViewReportClicked(const QString &idCourrier)
{
    marquerCourrierLu(idCourrier.toStdString());
    std::vector<Courrier> all = chargerCourriers();
    for (const auto &c : all) {
        if (c.id == idCourrier.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("Compte-rendu du Conseil — %1").arg(QString::fromStdString(c.titre)));
            dlg.setMinimumSize(600, 450);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(12);

            QLabel *tLbl = new QLabel(QString("<h2>%1</h2>").arg(QString::fromStdString(c.titre)), &dlg);
            tLbl->setWordWrap(true);
            QLabel *dLbl = new QLabel(QString("<b>Transmis par :</b> Secrétaire | <b>Date :</b> %1").arg(QString::fromStdString(c.date)), &dlg);

            QTextEdit *edit = new QTextEdit(&dlg);
            edit->setReadOnly(true);
            edit->setText(QString::fromStdString(c.contenu));
            edit->setStyleSheet("QTextEdit { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; border-radius: 8px; padding: 12px; font-size: 13px; line-height: 1.4; }");

            QPushButton *closeBtn = new QPushButton("Fermer la lecture", &dlg);
            closeBtn->setFixedHeight(38);
            closeBtn->setStyleSheet("QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; font-weight: bold; border: none; outline: none; }"
                                   "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }");
            connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(dLbl);
            l->addWidget(edit, 1);
            l->addWidget(closeBtn);

            dlg.exec();
            refreshData();
            return;
        }
    }
}

// ============================================================
//  CourriersRecteurView Implementation
// ============================================================
CourriersRecteurView::CourriersRecteurView(QWidget *parent)
    : QWidget(parent), paiementsTable(nullptr)
{
    setupUI();
    refreshData();
}

void CourriersRecteurView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(15);

    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(22, 20, 22, 20);
    tLayout->setSpacing(12);

    QLabel *tTitle = new QLabel("💼 Gestion Financière — Récapitulatifs des Paiements reçus de la Secrétaire", tableCard);
    tTitle->setStyleSheet("font-size: 17px; font-weight: bold; color: #0b1e36; border: none;");
    tLayout->addWidget(tTitle);

    paiementsTable = new QTableWidget(tableCard);
    paiementsTable->setColumnCount(4);
    paiementsTable->setHorizontalHeaderLabels({"Date de transmission", "Intitulé du document", "Statut", "Action"});
    paiementsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    paiementsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    paiementsTable->verticalHeader()->setVisible(false);
    paiementsTable->verticalHeader()->setDefaultSectionSize(38);
    paiementsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    paiementsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    paiementsTable->setAlternatingRowColors(true);
    paiementsTable->setShowGrid(false);
    paiementsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(paiementsTable);
    mainLayout->addWidget(tableCard);
}

void CourriersRecteurView::refreshData()
{
    if (!paiementsTable) return;
    std::vector<Courrier> all = chargerCourriers();
    paiementsTable->setRowCount(0);

    int count = 0;
    for (auto it = all.rbegin(); it != all.rend(); ++it) {
        if (it->destinataire == "Recteur" || it->type == "Liste des paiements") {
            paiementsTable->insertRow(count);

            QTableWidgetItem *dItem = new QTableWidgetItem(QString::fromStdString(it->date));
            dItem->setTextAlignment(Qt::AlignCenter);
            paiementsTable->setItem(count, 0, dItem);

            paiementsTable->setItem(count, 1, new QTableWidgetItem(QString::fromStdString(it->titre)));

            QWidget *badgeWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);
            QLabel *badge = new QLabel(QString::fromStdString(it->statut));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(80);
            badge->setFixedHeight(22);
            if (it->statut == "Lu") {
                badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
            } else {
                badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 11px; font-size: 11px;");
            }
            bLayout->addWidget(badge);
            paiementsTable->setCellWidget(count, 2, badgeWidget);

            QPushButton *viewBtn = new QPushButton("📊 Consulter le tableau");
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold; font-size: 12px; border: none; }"
                                   "QPushButton:hover { background-color: #1a3353; }");
            QString cId = QString::fromStdString(it->id);
            connect(viewBtn, &QPushButton::clicked, [this, cId]() { onViewPaiementDocClicked(cId); });
            paiementsTable->setCellWidget(count, 3, viewBtn);

            count++;
        }
    }
}

void CourriersRecteurView::onViewPaiementDocClicked(const QString &idCourrier)
{
    marquerCourrierLu(idCourrier.toStdString());
    std::vector<Courrier> all = chargerCourriers();
    for (const auto &c : all) {
        if (c.id == idCourrier.toStdString()) {
            QDialog dlg(this);
            dlg.setWindowTitle(QString("État Financier — %1").arg(QString::fromStdString(c.titre)));
            dlg.setMinimumSize(700, 500);

            QVBoxLayout *l = new QVBoxLayout(&dlg);
            l->setSpacing(12);

            QLabel *tLbl = new QLabel(QString("<h2>%1</h2>").arg(QString::fromStdString(c.titre)), &dlg);
            QLabel *dLbl = new QLabel(QString("<b>Transmis par :</b> Secrétaire | <b>Date :</b> %1").arg(QString::fromStdString(c.date)), &dlg);

            QTextEdit *edit = new QTextEdit(&dlg);
            edit->setReadOnly(true);
            edit->setText(QString::fromStdString(c.contenu));
            edit->setStyleSheet("QTextEdit { background-color: #f8fafc; color: #2d3748; border: 1.5px solid #cbd5e0; border-radius: 8px; padding: 12px; font-family: monospace; font-size: 12px; }");

            QPushButton *closeBtn = new QPushButton("Fermer", &dlg);
            closeBtn->setFixedHeight(38);
            closeBtn->setStyleSheet("QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; font-weight: bold; border: none; outline: none; }"
                                   "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }");
            connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

            l->addWidget(tLbl);
            l->addWidget(dLbl);
            l->addWidget(edit, 1);
            l->addWidget(closeBtn);

            dlg.exec();
            refreshData();
            return;
        }
    }
}
