#include "paiementversement.h"
#include "tresorerie.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <fstream>
#include <sstream>
#include <algorithm>

static const std::string FICHIER_VERSEMENTS = "versements.txt";
static const char SEPARATEUR_VER = '~';

static std::vector<std::string> decouperVersement(const std::string &texte, char sep) {
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
        else if (c == '~') res += "-";
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

VersementEtudiant::VersementEtudiant(const std::string &id, const std::string &nom, double montant,
                                     const std::string &date, const std::string &statut,
                                     const std::string &methode)
    : idEtudiant(id), nom(nom), montant(montant), date(date), statut(statut), methode(methode) {}

std::string VersementEtudiant::toLigne() const {
    std::ostringstream oss;
    oss << idEtudiant << "~"
        << echapperTexte(nom) << "~"
        << montant << "~"
        << date << "~"
        << statut << "~"
        << methode;
    return oss.str();
}

VersementEtudiant VersementEtudiant::fromLigne(const std::string &ligne) {
    auto c = decouperVersement(ligne, '~');
    if (c.size() < 4) return VersementEtudiant();

    std::string id = c[0];
    std::string nom = restaurerTexte(c[1]);
    double montant = 0.0;
    try {
        montant = std::stod(c[2]);
    } catch (...) {
        montant = 0.0;
    }
    std::string date = c[3];
    std::string statut = (c.size() >= 5) ? c[4] : "En attente";
    std::string methode = (c.size() >= 6) ? c[5] : "MonCash";

    return VersementEtudiant(id, nom, montant, date, statut, methode);
}

std::vector<VersementEtudiant> chargerVersements() {
    std::vector<VersementEtudiant> liste;
    std::ifstream f(FICHIER_VERSEMENTS);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        VersementEtudiant v = VersementEtudiant::fromLigne(ligne);
        if (!v.idEtudiant.empty()) {
            liste.push_back(v);
        }
    }
    return liste;
}

void sauvegarderVersements(const std::vector<VersementEtudiant> &versements) {
    std::ofstream f(FICHIER_VERSEMENTS, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &v : versements) {
        f << v.toLigne() << "\n";
    }
}

void ajouterVersement(const VersementEtudiant &versement) {
    std::vector<VersementEtudiant> liste = chargerVersements();
    liste.push_back(versement);
    sauvegarderVersements(liste);
}

bool validerVersementEtudiant(const std::string &idEtudiant, const std::string &dateVersement, const std::string &nouveauStatut) {
    std::vector<VersementEtudiant> liste = chargerVersements();
    bool modifie = false;
    for (auto &v : liste) {
        if (v.idEtudiant == idEtudiant && v.date == dateVersement) {
            v.statut = nouveauStatut;
            modifie = true;
            break;
        }
    }
    if (modifie) {
        sauvegarderVersements(liste);
    }
    return modifie;
}

// ============================================================
//  PaiementVersement Widget Implementation
// ============================================================
PaiementVersement::PaiementVersement(Mode mode, const std::string &idEtudiant,
                                     const std::string &nomEtudiant,
                                     std::vector<CompteEtudiant> *comptesPtr,
                                     QWidget *parent)
    : QWidget(parent), currentMode(mode), studentId(idEtudiant),
      studentName(nomEtudiant), comptes(comptesPtr),
      amountEdit(nullptr), methodCombo(nullptr), refTransactionEdit(nullptr),
      studentHistoryTable(nullptr), totalPaidLabel(nullptr),
      adminVersementsTable(nullptr), filterStatusCombo(nullptr),
      statPendingLabel(nullptr), statValidatedLabel(nullptr), statTotalAmountLabel(nullptr)
{
    if (currentMode == EtudiantMode) {
        setupEtudiantUI();
    } else {
        setupSecretaireUI();
    }
    refreshData();
}

void PaiementVersement::setupEtudiantUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(18);

    // Form Card
    QFrame *formCard = new QFrame(this);
    formCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(formCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    formCard->setGraphicsEffect(shadow);

    QVBoxLayout *fLayout = new QVBoxLayout(formCard);
    fLayout->setContentsMargins(25, 20, 25, 20);
    fLayout->setSpacing(12);

    QLabel *title = new QLabel("💳 Payer un Versement du Semestre", formCard);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #0b1e36; border: none;");

    QLabel *sub = new QLabel("Effectuez le règlement de vos frais de scolarité universitaire via le simulateur sécurisé.", formCard);
    sub->setStyleSheet("color: #718096; font-size: 13px; border: none;");

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(15);
    grid->setVerticalSpacing(10);

    grid->addWidget(new QLabel("Montant à verser (HTG) :", formCard), 0, 0);
    amountEdit = new QLineEdit(formCard);
    amountEdit->setPlaceholderText("Ex: 25000");
    amountEdit->setText("25000");
    amountEdit->setFixedHeight(38);
    amountEdit->setStyleSheet("QLineEdit { background-color: #f8fafc; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 14px; font-weight: bold; color: #1a202c; }");
    grid->addWidget(amountEdit, 0, 1);

    grid->addWidget(new QLabel("Méthode de paiement :", formCard), 1, 0);
    methodCombo = new QComboBox(formCard);
    methodCombo->addItems({"MonCash (Digicel)", "Carte Bancaire (Visa / Mastercard)", "Natcash", "Virement Bancaire (BUH / Sogebank)"});
    methodCombo->setFixedHeight(38);
    methodCombo->setStyleSheet("QComboBox { background-color: #f8fafc; border: 1.5px solid #cbd5e0; border-radius: 6px; padding-left: 10px; font-weight: bold; font-size: 13px; }");
    grid->addWidget(methodCombo, 1, 1);

    grid->addWidget(new QLabel("Numéro de référence / Tél :", formCard), 2, 0);
    refTransactionEdit = new QLineEdit(formCard);
    refTransactionEdit->setPlaceholderText("Ex: Ref transaction MonCash #8392019 ou 4 chiffres carte");
    refTransactionEdit->setFixedHeight(38);
    refTransactionEdit->setStyleSheet("QLineEdit { background-color: #f8fafc; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; }");
    grid->addWidget(refTransactionEdit, 2, 1);

    fLayout->addWidget(title);
    fLayout->addWidget(sub);
    fLayout->addSpacing(5);
    fLayout->addLayout(grid);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton *payBtn = new QPushButton("✅ Confirmer le versement", formCard);
    payBtn->setFixedHeight(42);
    payBtn->setFixedWidth(220);
    payBtn->setCursor(Qt::PointingHandCursor);
    payBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; border: none; }"
                          "QPushButton:hover { background-color: #1a3353; }");
    connect(payBtn, &QPushButton::clicked, this, &PaiementVersement::onConfirmPaymentClicked);
    btnRow->addWidget(payBtn);

    fLayout->addLayout(btnRow);
    mainLayout->addWidget(formCard);

    // History Table Card
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(22, 18, 22, 18);
    tLayout->setSpacing(12);

    QHBoxLayout *tHead = new QHBoxLayout();
    QLabel *tTitle = new QLabel("📋 Historique de mes versements", tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");

    totalPaidLabel = new QLabel("Total validé : 0 HTG", tableCard);
    totalPaidLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #27ae60; border: none;");

    tHead->addWidget(tTitle);
    tHead->addStretch();
    tHead->addWidget(totalPaidLabel);
    tLayout->addLayout(tHead);

    studentHistoryTable = new QTableWidget(tableCard);
    studentHistoryTable->setColumnCount(4);
    studentHistoryTable->setHorizontalHeaderLabels({"Date & Heure", "Montant (HTG)", "Méthode", "Statut"});
    studentHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    studentHistoryTable->verticalHeader()->setVisible(false);
    studentHistoryTable->verticalHeader()->setDefaultSectionSize(38);
    studentHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    studentHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentHistoryTable->setAlternatingRowColors(true);
    studentHistoryTable->setShowGrid(false);
    studentHistoryTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(studentHistoryTable);
    mainLayout->addWidget(tableCard);
}

void PaiementVersement::setupSecretaireUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(16);

    // Top Stats Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    auto makeCard = [](const QString &icon, const QString &title, QLabel *&valLbl, const QString &color) {
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

    statsLayout->addWidget(makeCard("⏳", "Versements en attente", statPendingLabel, "#DD6B20"));
    statsLayout->addWidget(makeCard("✅", "Versements validés", statValidatedLabel, "#38A169"));
    statsLayout->addWidget(makeCard("💰", "Total perçu (HTG)", statTotalAmountLabel, "#3182CE"));
    mainLayout->addLayout(statsLayout);

    // Table Card
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(22, 18, 22, 18);
    tLayout->setSpacing(14);

    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *tTitle = new QLabel("📋 Registre des Versements Étudiants Reçus", tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36; border: none;");

    filterStatusCombo = new QComboBox(tableCard);
    filterStatusCombo->setFixedWidth(160);
    filterStatusCombo->setFixedHeight(36);
    filterStatusCombo->setStyleSheet("QComboBox { background-color: #ffffff; color: #2d3748; border: 1.5px solid #cbd5e0; border-radius: 6px; padding-left: 10px; font-weight: bold; font-size: 12px; }");
    filterStatusCombo->addItems({"Tous les statuts", "En attente", "Validé", "Rejeté"});
    connect(filterStatusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PaiementVersement::refreshData);

    headerRow->addWidget(tTitle);
    headerRow->addStretch();
    headerRow->addWidget(new QLabel("Filtrer :", tableCard));
    headerRow->addWidget(filterStatusCombo);
    tLayout->addLayout(headerRow);

    adminVersementsTable = new QTableWidget(tableCard);
    adminVersementsTable->setColumnCount(7);
    adminVersementsTable->setHorizontalHeaderLabels({"ID Étudiant", "Nom / Prénom", "Montant", "Méthode", "Date", "Statut", "Action"});
    adminVersementsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    adminVersementsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    adminVersementsTable->verticalHeader()->setVisible(false);
    adminVersementsTable->verticalHeader()->setDefaultSectionSize(38);
    adminVersementsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    adminVersementsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    adminVersementsTable->setAlternatingRowColors(true);
    adminVersementsTable->setShowGrid(false);
    adminVersementsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(adminVersementsTable);
    mainLayout->addWidget(tableCard);
}

void PaiementVersement::onConfirmPaymentClicked()
{
    if (!amountEdit || !methodCombo) return;

    QString amtStr = amountEdit->text().trimmed();
    double amount = amtStr.toDouble();

    if (amount <= 0) {
        QMessageBox::warning(this, "Montant invalide", "Veuillez spécifier un montant positif supérieur à 0 HTG.");
        return;
    }

    QString methode = methodCombo->currentText();
    QString dateStr = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");

    VersementEtudiant v(studentId, studentName, amount, dateStr.toStdString(), "En attente", methode.toStdString());
    ajouterVersement(v);

    QMessageBox::information(this, "Paiement envoyé",
                             QString("Votre versement de %1 HTG via %2 a été enregistré.\n"
                                     "Il a été transmis au Secrétariat pour validation et confirmation de réception.")
                                 .arg(amount).arg(methode));

    refreshData();
}

void PaiementVersement::onValidatePaymentClicked(const QString &idEtudiant, const QString &dateVersement)
{
    auto reply = QMessageBox::question(this, "Confirmer réception",
                                       QString("Voulez-vous valider et confirmer la réception du versement pour l'étudiant %1 ?").arg(idEtudiant),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        validerVersementEtudiant(idEtudiant.toStdString(), dateVersement.toStdString(), "Validé");

        // Also update student inscription aPaye state if comptes vector is available
        std::vector<CompteEtudiant> listComptes = chargerComptes();
        for (auto &c : listComptes) {
            if (c.getId() == idEtudiant.toStdString()) {
                Inscription ins = c.getInscription();
                ins.aPaye = true;
                c.setInscription(ins);
                break;
            }
        }
        sauvegarderComptes(listComptes);

        // Enregistrer la recette dans le journal de trésorerie
        std::vector<VersementEtudiant> allVers = chargerVersements();
        for (const auto &v : allVers) {
            if (v.idEtudiant == idEtudiant.toStdString() && v.date == dateVersement.toStdString()) {
                double solde = calculerSoldeTresorerieDisponible();
                enregistrerOperationTresorerie(
                    "Recette",
                    "Frais de scolarité",
                    v.nom + " (" + v.idEtudiant + ")",
                    v.montant,
                    solde,
                    v.methode,
                    "Validation du versement étudiant par le secrétariat"
                );
                break;
            }
        }

        refreshData();
    }
}

void PaiementVersement::onRejectPaymentClicked(const QString &idEtudiant, const QString &dateVersement)
{
    auto reply = QMessageBox::question(this, "Rejeter versement",
                                       QString("Voulez-vous rejeter ce versement pour l'étudiant %1 ?").arg(idEtudiant),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        validerVersementEtudiant(idEtudiant.toStdString(), dateVersement.toStdString(), "Rejeté");
        refreshData();
    }
}

void PaiementVersement::refreshData()
{
    std::vector<VersementEtudiant> all = chargerVersements();

    if (currentMode == EtudiantMode) {
        if (!studentHistoryTable) return;
        studentHistoryTable->setRowCount(0);

        double totalValide = 0.0;
        int row = 0;
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            if (it->idEtudiant == studentId || (!studentId.empty() && it->idEtudiant.empty())) {
                studentHistoryTable->insertRow(row);

                QTableWidgetItem *dItem = new QTableWidgetItem(QString::fromStdString(it->date));
                dItem->setTextAlignment(Qt::AlignCenter);
                studentHistoryTable->setItem(row, 0, dItem);

                QTableWidgetItem *mItem = new QTableWidgetItem(QString::number(it->montant, 'f', 2) + " HTG");
                mItem->setTextAlignment(Qt::AlignCenter);
                mItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
                studentHistoryTable->setItem(row, 1, mItem);

                QTableWidgetItem *methItem = new QTableWidgetItem(QString::fromStdString(it->methode));
                methItem->setTextAlignment(Qt::AlignCenter);
                studentHistoryTable->setItem(row, 2, methItem);

                // Badge
                QWidget *badgeWidget = new QWidget();
                QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
                bLayout->setContentsMargins(4, 2, 4, 2);
                bLayout->setAlignment(Qt::AlignCenter);
                QLabel *badge = new QLabel(QString::fromStdString(it->statut));
                badge->setAlignment(Qt::AlignCenter);
                badge->setFixedWidth(95);
                badge->setFixedHeight(22);
                if (it->statut == "Validé") {
                    badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
                    totalValide += it->montant;
                } else if (it->statut == "En attente") {
                    badge->setStyleSheet("background-color: #FEEBC8; color: #7B341E; font-weight: bold; border-radius: 11px; font-size: 11px;");
                } else {
                    badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 11px; font-size: 11px;");
                }
                bLayout->addWidget(badge);
                studentHistoryTable->setCellWidget(row, 3, badgeWidget);

                row++;
            }
        }

        if (totalPaidLabel) {
            totalPaidLabel->setText(QString("Total validé : %1 HTG").arg(totalValide));
        }
    } else {
        if (!adminVersementsTable) return;
        adminVersementsTable->setRowCount(0);

        int pendingCount = 0;
        int validatedCount = 0;
        double totalAmount = 0.0;

        QString f = filterStatusCombo ? filterStatusCombo->currentText() : "Tous les statuts";

        int row = 0;
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            if (it->statut == "Validé") {
                validatedCount++;
                totalAmount += it->montant;
            } else if (it->statut == "En attente") {
                pendingCount++;
            }

            if (f != "Tous les statuts" && QString::fromStdString(it->statut) != f) {
                continue;
            }

            adminVersementsTable->insertRow(row);

            QTableWidgetItem *idItem = new QTableWidgetItem(QString::fromStdString(it->idEtudiant));
            idItem->setTextAlignment(Qt::AlignCenter);
            adminVersementsTable->setItem(row, 0, idItem);

            adminVersementsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(it->nom)));

            QTableWidgetItem *mItem = new QTableWidgetItem(QString::number(it->montant, 'f', 2) + " HTG");
            mItem->setTextAlignment(Qt::AlignCenter);
            mItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
            adminVersementsTable->setItem(row, 2, mItem);

            QTableWidgetItem *methItem = new QTableWidgetItem(QString::fromStdString(it->methode));
            methItem->setTextAlignment(Qt::AlignCenter);
            adminVersementsTable->setItem(row, 3, methItem);

            QTableWidgetItem *dItem = new QTableWidgetItem(QString::fromStdString(it->date));
            dItem->setTextAlignment(Qt::AlignCenter);
            adminVersementsTable->setItem(row, 4, dItem);

            // Badge
            QWidget *badgeWidget = new QWidget();
            QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
            bLayout->setContentsMargins(4, 2, 4, 2);
            bLayout->setAlignment(Qt::AlignCenter);
            QLabel *badge = new QLabel(QString::fromStdString(it->statut));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedWidth(90);
            badge->setFixedHeight(22);
            if (it->statut == "Validé") {
                badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
            } else if (it->statut == "En attente") {
                badge->setStyleSheet("background-color: #FEEBC8; color: #7B341E; font-weight: bold; border-radius: 11px; font-size: 11px;");
            } else {
                badge->setStyleSheet("background-color: #FED7D7; color: #742A2A; font-weight: bold; border-radius: 11px; font-size: 11px;");
            }
            bLayout->addWidget(badge);
            adminVersementsTable->setCellWidget(row, 5, badgeWidget);

            // Action Buttons
            QWidget *actWidget = new QWidget();
            QHBoxLayout *aLayout = new QHBoxLayout(actWidget);
            aLayout->setContentsMargins(2, 2, 2, 2);
            aLayout->setSpacing(6);

            if (it->statut == "En attente") {
                QPushButton *okBtn = new QPushButton("✅ Confirmer");
                okBtn->setCursor(Qt::PointingHandCursor);
                okBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                     "QPushButton:hover { background-color: #1a3353; }");
                QString stId = QString::fromStdString(it->idEtudiant);
                QString dt = QString::fromStdString(it->date);
                connect(okBtn, &QPushButton::clicked, [this, stId, dt]() { onValidatePaymentClicked(stId, dt); });
                aLayout->addWidget(okBtn);

                QPushButton *rejBtn = new QPushButton("❌ Rejeter");
                rejBtn->setCursor(Qt::PointingHandCursor);
                rejBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                      "QPushButton:hover { background-color: #1a3353; }");
                connect(rejBtn, &QPushButton::clicked, [this, stId, dt]() { onRejectPaymentClicked(stId, dt); });
                aLayout->addWidget(rejBtn);
            } else {
                QLabel *doneLbl = new QLabel("Traité");
                doneLbl->setStyleSheet("color: #a0aec0; font-size: 11px; font-style: italic;");
                aLayout->addWidget(doneLbl);
            }
            adminVersementsTable->setCellWidget(row, 6, actWidget);

            row++;
        }

        if (statPendingLabel) statPendingLabel->setText(QString::number(pendingCount));
        if (statValidatedLabel) statValidatedLabel->setText(QString::number(validatedCount));
        if (statTotalAmountLabel) statTotalAmountLabel->setText(QString::number(totalAmount, 'f', 0) + " HTG");
    }
}
