#include "registrationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QIcon>
#include <QPixmap>
#include <QFileInfo>
#include <QScrollArea>
#include <QScreen>
#include <QGuiApplication>

RegistrationDialog::RegistrationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void RegistrationDialog::setupUI()
{
    setWindowTitle("Finalisation de l'Inscription — UDH");
    setWindowIcon(QIcon(":/resources/logo.png"));
    setMinimumSize(520, 400);

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    if (primaryScreen) {
        QRect avail = primaryScreen->availableGeometry();
        int targetW = qMin(640, avail.width() - 40);
        int targetH = qMin(580, avail.height() - 80);
        resize(targetW, targetH);
    } else {
        resize(620, 560);
    }

    setStyleSheet(
        "QDialog { background-color: #f8fafc; font-family: 'Segoe UI', sans-serif; }"
        "QLabel { color: #1a202c; font-size: 13px; background: transparent; }"
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
        "QGroupBox { font-weight: bold; color: #0b1e36; border: 1.5px solid #e2e8f0; border-radius: 10px; margin-top: 14px; padding: 18px 14px 14px 14px; background-color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 8px; color: #0b1e36; font-size: 14px; background-color: #ffffff; border-radius: 4px; }"
    );

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // QScrollArea for flexible scrolling
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f8fafc; }");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #f8fafc;");
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(22, 16, 22, 16);
    mainLayout->setSpacing(14);

    // Header Banner
    QWidget *header = new QWidget();
    header->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0b1e36, stop:1 #1a365d); border-radius: 10px; color: white;");
    QHBoxLayout *hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(16, 12, 16, 12);
    hLayout->setSpacing(14);

    QLabel *logo = new QLabel();
    QPixmap pix(":/resources/logo.png");
    if (!pix.isNull()) {
        logo->setPixmap(pix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logo->setStyleSheet("background: transparent;");

    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *t1 = new QLabel("Dossier d'Inscription Universitaire");
    t1->setStyleSheet("font-size: 16px; font-weight: bold; color: #d4af37; background: transparent;");
    QLabel *t2 = new QLabel("Complétez vos informations pour débloquer votre accès étudiant");
    t2->setStyleSheet("font-size: 11px; color: #cbd5e0; background: transparent;");
    titleLayout->addWidget(t1);
    titleLayout->addWidget(t2);

    hLayout->addWidget(logo);
    hLayout->addLayout(titleLayout, 1);
    mainLayout->addWidget(header);

    // Group 1: Emergency Contact
    QGroupBox *contactGroup = new QGroupBox("🚨 Personne de Référence / Contact d'Urgence", scrollContent);
    QGridLayout *g1 = new QGridLayout(contactGroup);
    g1->setContentsMargins(14, 16, 14, 14);
    g1->setVerticalSpacing(10);
    g1->setHorizontalSpacing(14);

    QLabel *lblNom = new QLabel("Nom du proche :");
    nomProcheEdit = new QLineEdit();
    nomProcheEdit->setPlaceholderText("Ex: Dupont");
    nomProcheEdit->setFixedHeight(34);

    QLabel *lblPrenom = new QLabel("Prénom du proche :");
    prenomProcheEdit = new QLineEdit();
    prenomProcheEdit->setPlaceholderText("Ex: Jean");
    prenomProcheEdit->setFixedHeight(34);

    QLabel *lblTel = new QLabel("Numéro de téléphone :");
    telProcheEdit = new QLineEdit();
    telProcheEdit->setPlaceholderText("Ex: +509 3123-4567");
    telProcheEdit->setFixedHeight(34);

    g1->addWidget(lblNom, 0, 0);
    g1->addWidget(nomProcheEdit, 0, 1);
    g1->addWidget(lblPrenom, 1, 0);
    g1->addWidget(prenomProcheEdit, 1, 1);
    g1->addWidget(lblTel, 2, 0);
    g1->addWidget(telProcheEdit, 2, 1);
    mainLayout->addWidget(contactGroup);

    // Group 2: Faculty Selection
    QGroupBox *facGroup = new QGroupBox("🏛️ Faculté d'Études", scrollContent);
    QVBoxLayout *g2 = new QVBoxLayout(facGroup);
    g2->setContentsMargins(14, 16, 14, 14);
    g2->setSpacing(8);

    QLabel *lblFac = new QLabel("Sélectionnez votre filière :");
    faculteCombo = new QComboBox();
    faculteCombo->setFixedHeight(36);
    faculteCombo->addItems({
        "Genie civil",
        "Genie informatique",
        "Science economique et comptabilite",
        "Science administration",
        "Science juridique",
        "Science education"
    });
    g2->addWidget(lblFac);
    g2->addWidget(faculteCombo);
    mainLayout->addWidget(facGroup);

    // Group 3: Documents
    QGroupBox *docGroup = new QGroupBox("📁 Pièces Justificatives", scrollContent);
    QVBoxLayout *g3 = new QVBoxLayout(docGroup);
    g3->setContentsMargins(14, 16, 14, 14);
    g3->setSpacing(12);

    // Document 1: ID
    QVBoxLayout *idBlock = new QVBoxLayout();
    idBlock->setSpacing(5);
    QLabel *lblId = new QLabel("Pièce d'Identité (CIN / NIF / Passeport) :");
    lblId->setStyleSheet("font-weight: 600; color: #2d3748;");

    QHBoxLayout *idRow = new QHBoxLayout();
    QPushButton *btnId = new QPushButton("📁 Parcourir...");
    btnId->setFixedHeight(34);
    btnId->setCursor(Qt::PointingHandCursor);
    btnId->setStyleSheet(
        "QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; padding: 0 14px; font-weight: bold; border: none; outline: none; }"
        "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
    );
    idPathLabel = new QLabel("Aucun fichier sélectionné");
    idPathLabel->setStyleSheet("font-size: 12px; color: #718096;");
    idPathLabel->setWordWrap(true);

    idRow->addWidget(btnId);
    idRow->addWidget(idPathLabel, 1);
    idBlock->addWidget(lblId);
    idBlock->addLayout(idRow);
    g3->addLayout(idBlock);

    connect(btnId, &QPushButton::clicked, this, &RegistrationDialog::onBrowseId);

    // Document 2: Photo
    QVBoxLayout *photoBlock = new QVBoxLayout();
    photoBlock->setSpacing(5);
    QLabel *lblPhoto = new QLabel("Photo d'Identité récente :");
    lblPhoto->setStyleSheet("font-weight: 600; color: #2d3748;");

    QHBoxLayout *photoRow = new QHBoxLayout();
    QPushButton *btnPhoto = new QPushButton("📷 Parcourir...");
    btnPhoto->setFixedHeight(34);
    btnPhoto->setCursor(Qt::PointingHandCursor);
    btnPhoto->setStyleSheet(
        "QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; padding: 0 14px; font-weight: bold; border: none; outline: none; }"
        "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
    );
    photoPathLabel = new QLabel("Aucune photo sélectionnée");
    photoPathLabel->setStyleSheet("font-size: 12px; color: #718096;");
    photoPathLabel->setWordWrap(true);

    photoRow->addWidget(btnPhoto);
    photoRow->addWidget(photoPathLabel, 1);
    photoBlock->addWidget(lblPhoto);
    photoBlock->addLayout(photoRow);
    g3->addLayout(photoBlock);

    connect(btnPhoto, &QPushButton::clicked, this, &RegistrationDialog::onBrowsePhoto);

    // Document 3: Payment Proof
    QVBoxLayout *payBlock = new QVBoxLayout();
    payBlock->setSpacing(5);
    QLabel *lblPayment = new QLabel("Preuve de Paiement / Reçu des frais (Photo du reçu, capture MonCash) :");
    lblPayment->setStyleSheet("font-weight: 600; color: #2d3748;");

    QHBoxLayout *payRow = new QHBoxLayout();
    browsePaymentBtn = new QPushButton("💳 Parcourir...");
    browsePaymentBtn->setFixedHeight(34);
    browsePaymentBtn->setCursor(Qt::PointingHandCursor);
    browsePaymentBtn->setStyleSheet(
        "QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; padding: 0 14px; font-weight: bold; border: none; outline: none; }"
        "QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
    );

    paymentPathLabel = new QLabel("Aucune photo de paiement sélectionnée");
    paymentPathLabel->setStyleSheet("font-size: 12px; color: #718096;");
    paymentPathLabel->setWordWrap(true);

    connect(browsePaymentBtn, &QPushButton::clicked, this, &RegistrationDialog::onBrowsePayment);

    payRow->addWidget(browsePaymentBtn);
    payRow->addWidget(paymentPathLabel, 1);

    paidCheckBox = new QCheckBox("Frais d'inscription réglés (reçu / preuve joint)", scrollContent);
    paidCheckBox->setCursor(Qt::PointingHandCursor);
    paidCheckBox->setStyleSheet("QCheckBox { color: #1a202c; font-weight: bold; font-size: 12px; margin-top: 4px; }");

    payBlock->addWidget(lblPayment);
    payBlock->addLayout(payRow);
    payBlock->addWidget(paidCheckBox);
    g3->addLayout(payBlock);

    mainLayout->addWidget(docGroup);
    mainLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    rootLayout->addWidget(scrollArea, 1);

    // ============================================================
    //  FIXED ACTION BAR (always visible at bottom, outside scroll)
    // ============================================================
    QWidget *actionBar = new QWidget(this);
    actionBar->setFixedHeight(60);
    actionBar->setStyleSheet("QWidget { background-color: #ffffff; border-top: 1.5px solid #e2e8f0; }");

    QHBoxLayout *btnLayout = new QHBoxLayout(actionBar);
    btnLayout->setContentsMargins(20, 10, 20, 10);
    btnLayout->setSpacing(12);

    QPushButton *cancelBtn = new QPushButton("Annuler", actionBar);
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #f1f5f9; color: #334155; border-radius: 6px; font-weight: bold; font-size: 13px; border: 1px solid #cbd5e1; padding: 0 20px; outline: none; }"
        "QPushButton:hover { background-color: #e2e8f0; color: #0f172a; }"
    );
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *submitBtn = new QPushButton("✓ Valider mon Inscription", actionBar);
    submitBtn->setFixedHeight(40);
    submitBtn->setCursor(Qt::PointingHandCursor);
    submitBtn->setStyleSheet(
        "QPushButton { background-color: #16a34a; color: #ffffff; border-radius: 6px; font-weight: bold; font-size: 14px; border: none; padding: 0 24px; outline: none; }"
        "QPushButton:hover { background-color: #15803d; color: #ffffff; }"
        "QPushButton:pressed { background-color: #166534; }"
    );
    connect(submitBtn, &QPushButton::clicked, this, &RegistrationDialog::onValidate);

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(submitBtn);

    rootLayout->addWidget(actionBar);
}

void RegistrationDialog::onBrowseId()
{
    QString file = QFileDialog::getOpenFileName(this, "Sélectionner la pièce d'identité", QDir::homePath(), "Documents (*.pdf *.png *.jpg *.jpeg)");
    if (!file.isEmpty()) {
        idPath = file;
        idPathLabel->setText(QFileInfo(file).fileName());
        idPathLabel->setStyleSheet("font-size: 12px; color: #2b6cb0; font-weight: bold;");
    }
}

void RegistrationDialog::onBrowsePhoto()
{
    QString file = QFileDialog::getOpenFileName(this, "Sélectionner la photo d'identité", QDir::homePath(), "Images (*.png *.jpg *.jpeg)");
    if (!file.isEmpty()) {
        photoPath = file;
        photoPathLabel->setText(QFileInfo(file).fileName());
        photoPathLabel->setStyleSheet("font-size: 12px; color: #2b6cb0; font-weight: bold;");
    }
}

void RegistrationDialog::onBrowsePayment()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Sélectionner la photo ou preuve de paiement",
        QDir::homePath(),
        "Photos et Reçus (*.png *.jpg *.jpeg *.bmp *.webp *.pdf);;Images (*.png *.jpg *.jpeg *.bmp *.webp);;PDF (*.pdf);;Tous les fichiers (*.*)"
    );
    if (!file.isEmpty()) {
        paymentPath = file;
        paymentPathLabel->setText(QFileInfo(file).fileName());
        paymentPathLabel->setStyleSheet("font-size: 12px; color: #16a34a; font-weight: bold;");
        if (paidCheckBox) {
            paidCheckBox->setChecked(true);
        }
    }
}

void RegistrationDialog::onValidate()
{
    QString nom = nomProcheEdit->text().trimmed();
    QString prenom = prenomProcheEdit->text().trimmed();
    QString tel = telProcheEdit->text().trimmed();

    if (nom.isEmpty() || prenom.isEmpty() || tel.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir les coordonnées du contact d'urgence (Nom, Prénom et Téléphone).");
        return;
    }

    resultInscription.nomProche = nom.toStdString();
    resultInscription.prenomProche = prenom.toStdString();
    resultInscription.telProche = tel.toStdString();
    resultInscription.faculte = faculteCombo->currentText().toStdString();
    resultInscription.pieceIdentitePath = idPath.toStdString();
    resultInscription.photoPath = photoPath.toStdString();
    resultInscription.aPaye = paidCheckBox->isChecked();
    resultInscription.preuvePaiementPath = paymentPath.toStdString();

    accept();
}

Inscription RegistrationDialog::getInscriptionData() const
{
    return resultInscription;
}
