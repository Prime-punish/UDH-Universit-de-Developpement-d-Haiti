#include "fichedetailinscription.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QScrollArea>

FicheDetailInscription::FicheDetailInscription(CompteEtudiant &compteRef, std::vector<CompteEtudiant> &comptesRef, QWidget *parent)
    : QDialog(parent), compte(compteRef), comptes(comptesRef)
{
    setupUI();
}

void FicheDetailInscription::setupUI()
{
    setWindowTitle(QString("Dossier d'Inscription — %1").arg(QString::fromStdString(compte.getId())));
    setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setMinimumSize(620, 580);
    resize(720, 780);

    setStyleSheet(
        "QDialog { background-color: #f8fafc; font-family: 'Segoe UI', sans-serif; }"
        "QLabel { color: #1a202c; font-size: 13px; background: transparent; }"
    );

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ============================================================
    //  SCROLLABLE CONTENT (header, info, documents, status)
    // ============================================================
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f8fafc; }");

    QWidget *content = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(18);

    // Banner Header
    QWidget *header = new QWidget();
    header->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0b1e36, stop:1 #1a365d); border-radius: 10px; color: white;");
    QVBoxLayout *hLayout = new QVBoxLayout(header);
    hLayout->setContentsMargins(20, 16, 20, 16);

    QLabel *hTitle = new QLabel(QString("Dossier d'Inscription : %1 %2")
        .arg(QString::fromStdString(compte.getPrenom()), QString::fromStdString(compte.getNom())));
    hTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #d4af37; background: transparent;");

    QLabel *hSub = new QLabel(QString("ID Étudiant : %1  |  Date de soumission : %2")
        .arg(QString::fromStdString(compte.getId()), QString::fromStdString(compte.getDateSoumission().empty() ? "Non spécifiée" : compte.getDateSoumission())));
    hSub->setStyleSheet("font-size: 12px; color: #cbd5e0; background: transparent;");

    hLayout->addWidget(hTitle);
    hLayout->addWidget(hSub);
    layout->addWidget(header);

    // Personal Info Card
    QFrame *infoCard = new QFrame();
    infoCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1.5px solid #e2e8f0; }");
    QVBoxLayout *icLayout = new QVBoxLayout(infoCard);
    icLayout->setContentsMargins(20, 20, 20, 20);
    icLayout->setSpacing(10);

    QLabel *infoTitle = new QLabel("👤 Informations Personnelles & Contact");
    infoTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #0b1e36; border: none; margin-bottom: 5px;");
    icLayout->addWidget(infoTitle);

    auto addRow = [icLayout](const QString &lbl, const QString &val) {
        QHBoxLayout *h = new QHBoxLayout();
        QLabel *l = new QLabel(lbl + " :"); l->setFixedWidth(160);
        l->setStyleSheet("font-weight: bold; color: #4a5568; border: none;");
        QLabel *v = new QLabel(val);
        v->setStyleSheet("color: #1a202c; border: none;");
        v->setWordWrap(true);
        h->addWidget(l); h->addWidget(v, 1);
        icLayout->addLayout(h);
    };

    const Inscription &insc = compte.getInscription();
    addRow("Nom complet", QString("%1 %2").arg(QString::fromStdString(compte.getPrenom()), QString::fromStdString(compte.getNom())));
    addRow("Email", QString::fromStdString(compte.getEmail()));
    addRow("Téléphone", QString::fromStdString(compte.getTelephone()));
    addRow("Faculté choisie", QString::fromStdString(insc.faculte.empty() ? "Non spécifiée" : insc.faculte));
    addRow("Contact d'urgence", QString("%1 %2 (%3)").arg(QString::fromStdString(insc.prenomProche), QString::fromStdString(insc.nomProche), QString::fromStdString(insc.telProche)));
    addRow("Frais réglés", insc.aPaye ? "✅ Oui (Preuve de paiement jointe)" : "❌ Non réglé");

    layout->addWidget(infoCard);

    // Documents Card
    QFrame *docCard = new QFrame();
    docCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1.5px solid #e2e8f0; }");
    QVBoxLayout *dcLayout = new QVBoxLayout(docCard);
    dcLayout->setContentsMargins(20, 20, 20, 20);
    dcLayout->setSpacing(12);

    QLabel *docTitle = new QLabel("📁 Pièces Justificatives Transmises");
    docTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #0b1e36; border: none;");
    dcLayout->addWidget(docTitle);

    auto addDocRow = [dcLayout](const QString &docName, const std::string &path) {
        QHBoxLayout *h = new QHBoxLayout();
        QLabel *l = new QLabel(docName + " :");
        l->setFixedWidth(180);
        l->setStyleSheet("font-weight: bold; color: #4a5568; border: none;");

        QString pStr = QString::fromStdString(path);
        QLabel *pathLbl = new QLabel(pStr.isEmpty() ? "<i>Non fourni</i>" : QFileInfo(pStr).fileName());
        pathLbl->setStyleSheet("color: #2d3748; border: none;");

        QPushButton *openBtn = new QPushButton("👁️ Ouvrir");
        openBtn->setFixedWidth(90);
        openBtn->setFixedHeight(32);
        openBtn->setCursor(Qt::PointingHandCursor);
        openBtn->setEnabled(!pStr.isEmpty());
        openBtn->setStyleSheet(
            pStr.isEmpty() ?
            "QPushButton { background-color: #edf2f7; color: #a0aec0; border-radius: 4px; border: none; }" :
            "QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; font-weight: bold; border: none; }"
            "QPushButton:hover { background-color: #1a3353; }"
        );

        QObject::connect(openBtn, &QPushButton::clicked, [pStr]() {
            if (!pStr.isEmpty()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(pStr));
            }
        });

        h->addWidget(l);
        h->addWidget(pathLbl, 1);
        h->addWidget(openBtn);
        dcLayout->addLayout(h);
    };

    addDocRow("Pièce d'Identité (CIN/NIF)", insc.pieceIdentitePath);
    addDocRow("Photo d'Identité récente", insc.photoPath);
    addDocRow("Preuve de Paiement", insc.preuvePaiementPath);

    layout->addWidget(docCard);

    // Current Status Banner
    QFrame *statusCard = new QFrame();
    statusCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1.5px solid #e2e8f0; }");
    QHBoxLayout *scLayout = new QHBoxLayout(statusCard);
    scLayout->setContentsMargins(20, 15, 20, 15);

    std::string st = compte.getStatutInscription();
    QString stText = st == "Approuvé" ? "✅ Inscription Approuvée" : (st == "Rejeté" ? "❌ Inscription Rejetée" : "⏳ En attente de décision");
    QString stColor = st == "Approuvé" ? "#27ae60" : (st == "Rejeté" ? "#e74c3c" : "#e67e22");

    QLabel *stLbl = new QLabel(stText);
    stLbl->setStyleSheet(QString("font-size: 15px; font-weight: bold; color: %1; border: none;").arg(stColor));
    scLayout->addWidget(stLbl);
    layout->addWidget(statusCard);

    layout->addStretch();

    scroll->setWidget(content);
    rootLayout->addWidget(scroll, 1);

    // ============================================================
    //  FIXED ACTION BUTTONS BAR (always visible at bottom)
    // ============================================================
    QWidget *actionBar = new QWidget(this);
    actionBar->setFixedHeight(70);
    actionBar->setStyleSheet("QWidget { background-color: #ffffff; border-top: 2px solid #e2e8f0; }");

    QHBoxLayout *actionLayout = new QHBoxLayout(actionBar);
    actionLayout->setContentsMargins(25, 12, 25, 12);
    actionLayout->setSpacing(15);

    QPushButton *closeBtn = new QPushButton("Fermer");
    closeBtn->setFixedHeight(44);
    closeBtn->setFixedWidth(120);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *rejectBtn = new QPushButton("✖  Rejeter l'inscription");
    rejectBtn->setFixedHeight(44);
    rejectBtn->setCursor(Qt::PointingHandCursor);
    rejectBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; border: none; padding: 0 24px; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(rejectBtn, &QPushButton::clicked, this, &FicheDetailInscription::onRejectClicked);

    QPushButton *approveBtn = new QPushButton("✔  Approuver l'inscription");
    approveBtn->setFixedHeight(44);
    approveBtn->setCursor(Qt::PointingHandCursor);
    approveBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; border: none; padding: 0 24px; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(approveBtn, &QPushButton::clicked, this, &FicheDetailInscription::onApproveClicked);

    actionLayout->addWidget(closeBtn);
    actionLayout->addStretch();
    actionLayout->addWidget(rejectBtn);
    actionLayout->addWidget(approveBtn);

    rootLayout->addWidget(actionBar);
}

void FicheDetailInscription::onApproveClicked()
{
    compte.setStatutInscription("Approuvé");
    compte.setActif(true);
    compte.setInscriptionFaite(true);

    enregistrerCompte(comptes, compte);

    QMessageBox::information(this, "Inscription Approuvée",
        QString("L'inscription de l'étudiant %1 %2 (ID: %3) a été validée avec succès.\nSon compte est désormais actif.")
        .arg(QString::fromStdString(compte.getPrenom()), QString::fromStdString(compte.getNom()), QString::fromStdString(compte.getId())));

    accept();
}

void FicheDetailInscription::onRejectClicked()
{
    bool ok;
    QString motif = QInputDialog::getText(
        this,
        "Motif du Rejet",
        "Veuillez indiquer le motif du rejet de cette inscription :",
        QLineEdit::Normal,
        "Dossier incomplet / Pièces non conformes",
        &ok
    );

    if (!ok) return;

    compte.setStatutInscription("Rejeté");
    compte.setActif(false);
    compte.setMotifRejet(motif.toStdString());

    enregistrerCompte(comptes, compte);

    QMessageBox::warning(this, "Inscription Rejetée",
        QString("L'inscription de l'étudiant %1 %2 a été rejetée.\n\nMotif : %3")
        .arg(QString::fromStdString(compte.getPrenom()), QString::fromStdString(compte.getNom()), motif));

    accept();
}
