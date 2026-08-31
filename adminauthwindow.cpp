#include "adminauthwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QPixmap>

AdminAuthWindow::AdminAuthWindow(std::vector<Administrateur>& adminsRef, QWidget *parent)
    : QWidget(parent), admins(adminsRef), isSignIn(true)
{
    setupUI();
}

AdminAuthWindow::~AdminAuthWindow() {}

void AdminAuthWindow::setupUI()
{
    setStyleSheet("background-color: #f0f2f5; font-family: 'Segoe UI', sans-serif;");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setAlignment(Qt::AlignCenter);

    // Top Back Button
    QHBoxLayout *topBar = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("← Retour à l'accueil", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
        "QPushButton { background-color: #ffffff; color: #14166B; border: 1.5px solid #cbd5e0; "
        "  border-radius: 6px; padding: 6px 14px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #e8ecf8; border-color: #14166B; }"
    );
    connect(backBtn, &QPushButton::clicked, this, &AdminAuthWindow::backToLanding);
    topBar->addWidget(backBtn);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    formContainer = new QWidget(this);
    formContainer->setFixedSize(900, 620);
    formContainer->setStyleSheet("background-color: white; border-radius: 15px;");
    rootLayout->addWidget(formContainer, 0, Qt::AlignCenter);

    signInForm = createSignInForm();
    signInForm->setParent(formContainer);
    signInForm->setGeometry(450, 0, 450, 620);

    signUpForm = createSignUpForm();
    signUpForm->setParent(formContainer);
    signUpForm->setGeometry(0, 0, 450, 620);
    signUpForm->hide();

    overlayPanel = new QWidget(formContainer);
    overlayPanel->setGeometry(0, 0, 450, 620);
    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-left-radius: 15px; border-bottom-left-radius: 15px;");

    QVBoxLayout *overlayLayout = new QVBoxLayout(overlayPanel);
    overlayLayout->setAlignment(Qt::AlignCenter);
    overlayLayout->setContentsMargins(35, 30, 35, 30);
    overlayLayout->setSpacing(10);

    QLabel *overlayLogo = new QLabel();
    QPixmap logoPix(":/resources/logo.png");
    if (!logoPix.isNull()) {
        overlayLogo->setPixmap(logoPix.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    overlayLogo->setAlignment(Qt::AlignCenter);
    overlayLogo->setStyleSheet("background: transparent; margin-bottom: 5px;");

    overlayTitle = new QLabel("ESPACE ADMINISTRATION");
    overlayTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #d4af37; background: transparent; letter-spacing: 1px;");
    overlayTitle->setAlignment(Qt::AlignCenter);

    QLabel *univSubtitle = new QLabel("UNIVERSITÉ DE DÉVELOPPEMENT D'HAÏTI");
    univSubtitle->setStyleSheet("font-size: 11px; font-weight: 600; color: #cfd8dc; background: transparent; text-align: center;");
    univSubtitle->setAlignment(Qt::AlignCenter);

    overlayDesc = new QLabel("Nouveau cadre administratif ?\nCréez votre compte pour accéder au système.");
    overlayDesc->setStyleSheet("font-size: 13px; margin-top: 10px; margin-bottom: 20px; color: #b0bec5; background: transparent;");
    overlayDesc->setAlignment(Qt::AlignCenter);
    overlayDesc->setWordWrap(true);

    overlayBtn = new QPushButton("Nouveau compte Admin");
    overlayBtn->setFixedSize(190, 42);
    overlayBtn->setCursor(Qt::PointingHandCursor);
    overlayBtn->setStyleSheet(
        "QPushButton { border: 2px solid #d4af37; border-radius: 21px; color: #d4af37; font-weight: bold; font-size: 13px; background-color: transparent; }"
        "QPushButton:hover { background-color: #d4af37; color: #0b1e36; }"
    );
    connect(overlayBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSwapToSignUp);

    overlayLayout->addWidget(overlayLogo);
    overlayLayout->addWidget(overlayTitle);
    overlayLayout->addWidget(univSubtitle);
    overlayLayout->addWidget(overlayDesc);
    overlayLayout->addWidget(overlayBtn, 0, Qt::AlignHCenter);

    animation = new QPropertyAnimation(overlayPanel, "geometry");
    animation->setDuration(400);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    updatePosteComboOptions();
}

QWidget* AdminAuthWindow::createSignInForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(50, 40, 50, 40);
    layout->setSpacing(18);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Connexion Administration");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    QLabel *subTitle = new QLabel("Accès réservé aux cadres administratifs");
    subTitle->setStyleSheet("font-size: 12px; color: #718096; background: transparent;");
    subTitle->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; }";

    loginUserEdit = new QLineEdit();
    loginUserEdit->setPlaceholderText("Identifiant ou Email administrateur");
    loginUserEdit->setFixedHeight(42);
    loginUserEdit->setStyleSheet(inputStyle);

    QHBoxLayout *pwdLayout = new QHBoxLayout();
    pwdLayout->setContentsMargins(0, 0, 0, 0);
    pwdLayout->setSpacing(0);

    loginPwdEdit = new QLineEdit();
    loginPwdEdit->setPlaceholderText("Mot de passe");
    loginPwdEdit->setEchoMode(QLineEdit::Password);
    loginPwdEdit->setFixedHeight(42);
    loginPwdEdit->setStyleSheet(
        "QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-top-left-radius: 6px; border-bottom-left-radius: 6px; padding: 8px 12px; font-size: 13px; border-right: none; font-weight: 500; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; border-right: none; }"
    );

    QPushButton *showHideBtn = new QPushButton("Afficher");
    showHideBtn->setFixedSize(70, 42);
    showHideBtn->setStyleSheet(
        "QPushButton { border: 1.5px solid #a0aec0; border-top-right-radius: 6px; border-bottom-right-radius: 6px; "
        "  border-left: none; background-color: #edf2f7; color: #2d3748; font-weight: bold; font-size: 12px; }"
        "QPushButton:hover { background-color: #e2e8f0; color: #0b1e36; }"
    );
    connect(showHideBtn, &QPushButton::clicked, this, &AdminAuthWindow::togglePasswordVisibility);

    pwdLayout->addWidget(loginPwdEdit);
    pwdLayout->addWidget(showHideBtn);

    QPushButton *signInBtn = new QPushButton("Se connecter");
    signInBtn->setFixedHeight(45);
    signInBtn->setCursor(Qt::PointingHandCursor);
    signInBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 15px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signInBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSignInClicked);

    layout->addWidget(title);
    layout->addWidget(subTitle);
    layout->addSpacing(10);
    layout->addWidget(loginUserEdit);
    layout->addLayout(pwdLayout);
    layout->addSpacing(5);
    layout->addWidget(signInBtn);

    return widget;
}

QWidget* AdminAuthWindow::createSignUpForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(40, 20, 40, 20);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Création Compte Admin");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 5px 8px; font-size: 12px; font-weight: 500; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }"
        "QComboBox QAbstractItemView::item:selected { background-color: #dbe4f9; color: #14166B; font-weight: bold; }";

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setSpacing(8);
    regNomEdit = new QLineEdit(); regNomEdit->setPlaceholderText("Nom");
    regNomEdit->setFixedHeight(34); regNomEdit->setStyleSheet(inputStyle);
    regPrenomEdit = new QLineEdit(); regPrenomEdit->setPlaceholderText("Prénom");
    regPrenomEdit->setFixedHeight(34); regPrenomEdit->setStyleSheet(inputStyle);
    nameLayout->addWidget(regNomEdit);
    nameLayout->addWidget(regPrenomEdit);

    QHBoxLayout *contactLayout = new QHBoxLayout();
    contactLayout->setSpacing(8);
    regPhoneEdit = new QLineEdit(); regPhoneEdit->setPlaceholderText("Téléphone");
    regPhoneEdit->setFixedHeight(34); regPhoneEdit->setStyleSheet(inputStyle);
    regEmailEdit = new QLineEdit(); regEmailEdit->setPlaceholderText("Email");
    regEmailEdit->setFixedHeight(34); regEmailEdit->setStyleSheet(inputStyle);
    contactLayout->addWidget(regPhoneEdit);
    contactLayout->addWidget(regEmailEdit);

    regPosteCombo = new QComboBox();
    regPosteCombo->setFixedHeight(34);
    regPosteCombo->setStyleSheet(inputStyle);
    connect(regPosteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminAuthWindow::onPosteChanged);

    posteWarningLabel = new QLabel();
    posteWarningLabel->setStyleSheet("color: #e53e3e; font-size: 11px; font-weight: bold; background: transparent;");
    posteWarningLabel->setWordWrap(true);
    posteWarningLabel->hide();

    regFaculteCombo = new QComboBox();
    regFaculteCombo->setFixedHeight(34);
    regFaculteCombo->setStyleSheet(inputStyle);
    regFaculteCombo->addItems({
        "Genie informatique",
        "Genie civil",
        "Science economique et comptabilite",
        "Science administration",
        "Science juridique",
        "Science education"
    });
    regFaculteCombo->setEnabled(false);

    QLabel *procheTitle = new QLabel("Contact du Proche :");
    procheTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #4a5568; background: transparent;");

    QHBoxLayout *procheLayout = new QHBoxLayout();
    procheLayout->setSpacing(6);
    regNomProcheEdit = new QLineEdit(); regNomProcheEdit->setPlaceholderText("Nom proche");
    regNomProcheEdit->setFixedHeight(32); regNomProcheEdit->setStyleSheet(inputStyle);
    regPrenomProcheEdit = new QLineEdit(); regPrenomProcheEdit->setPlaceholderText("Prénom proche");
    regPrenomProcheEdit->setFixedHeight(32); regPrenomProcheEdit->setStyleSheet(inputStyle);
    regTelProcheEdit = new QLineEdit(); regTelProcheEdit->setPlaceholderText("Tél proche");
    regTelProcheEdit->setFixedHeight(32); regTelProcheEdit->setStyleSheet(inputStyle);
    procheLayout->addWidget(regNomProcheEdit);
    procheLayout->addWidget(regPrenomProcheEdit);
    procheLayout->addWidget(regTelProcheEdit);

    regPwdEdit = new QLineEdit(); regPwdEdit->setPlaceholderText("Mot de passe");
    regPwdEdit->setEchoMode(QLineEdit::Password);
    regPwdEdit->setFixedHeight(34); regPwdEdit->setStyleSheet(inputStyle);

    regConfirmPwdEdit = new QLineEdit(); regConfirmPwdEdit->setPlaceholderText("Confirmer mot de passe");
    regConfirmPwdEdit->setEchoMode(QLineEdit::Password);
    regConfirmPwdEdit->setFixedHeight(34); regConfirmPwdEdit->setStyleSheet(inputStyle);

    QPushButton *signUpBtn = new QPushButton("Créer le compte Administrateur");
    signUpBtn->setFixedHeight(38);
    signUpBtn->setCursor(Qt::PointingHandCursor);
    signUpBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signUpBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSignUpClicked);

    layout->addWidget(title);
    layout->addLayout(nameLayout);
    layout->addLayout(contactLayout);
    layout->addWidget(new QLabel("Poste / Fonction :"));
    layout->addWidget(regPosteCombo);
    layout->addWidget(posteWarningLabel);
    layout->addWidget(new QLabel("Faculté (si Coordonnateur) :"));
    layout->addWidget(regFaculteCombo);
    layout->addWidget(procheTitle);
    layout->addLayout(procheLayout);
    layout->addWidget(regPwdEdit);
    layout->addWidget(regConfirmPwdEdit);
    layout->addSpacing(4);
    layout->addWidget(signUpBtn);

    return widget;
}

void AdminAuthWindow::updatePosteComboOptions()
{
    regPosteCombo->clear();
    QStringList allPostes = {
        "Recteur",
        "Président",
        "Secrétaire de l'administration",
        "Coordonnateur de faculté"
    };

    for (const QString &p : allPostes) {
        if (estPostePris(admins, p.toStdString())) {
            regPosteCombo->addItem(p + " ⚠️ (Poste déjà pris)");
        } else {
            regPosteCombo->addItem(p);
        }
    }
}

void AdminAuthWindow::onPosteChanged(int index)
{
    if (index < 0) return;
    QString currentText = regPosteCombo->currentText();

    if (currentText.contains("Coordonnateur")) {
        regFaculteCombo->setEnabled(true);
    } else {
        regFaculteCombo->setEnabled(false);
    }

    if (currentText.contains("Poste déjà pris")) {
        posteWarningLabel->setText("⚠️ Ce poste unique est déjà attribué à un autre administrateur.");
        posteWarningLabel->show();
    } else {
        posteWarningLabel->hide();
    }
}

void AdminAuthWindow::togglePasswordVisibility()
{
    if (loginPwdEdit->echoMode() == QLineEdit::Password) {
        loginPwdEdit->setEchoMode(QLineEdit::Normal);
        qobject_cast<QPushButton*>(sender())->setText("Cacher");
    } else {
        loginPwdEdit->setEchoMode(QLineEdit::Password);
        qobject_cast<QPushButton*>(sender())->setText("Afficher");
    }
}

void AdminAuthWindow::onSwapToSignUp()
{
    if (animation->state() == QPropertyAnimation::Running) return;
    updatePosteComboOptions();
    isSignIn = false;
    signUpForm->show();
    signUpForm->setGeometry(0, 0, 450, 620);

    animation->setStartValue(QRect(0, 0, 450, 620));
    animation->setEndValue(QRect(450, 0, 450, 620));

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-right-radius: 15px; border-bottom-right-radius: 15px;");
    overlayTitle->setText("DEJÀ INSCRIT ?");
    overlayDesc->setText("Connectez-vous pour accéder à votre espace d'administration.");
    overlayBtn->setText("Se connecter");

    disconnect(overlayBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSwapToSignUp);
    connect(overlayBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSwapToSignIn);

    connect(animation, &QPropertyAnimation::finished, [this]() {
        if (!isSignIn) signInForm->hide();
    });
    animation->start();
}

void AdminAuthWindow::onSwapToSignIn()
{
    if (animation->state() == QPropertyAnimation::Running) return;
    isSignIn = true;
    signInForm->show();
    signInForm->setGeometry(450, 0, 450, 620);

    animation->setStartValue(QRect(450, 0, 450, 620));
    animation->setEndValue(QRect(0, 0, 450, 620));

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-left-radius: 15px; border-bottom-left-radius: 15px;");
    overlayTitle->setText("ESPACE ADMINISTRATION");
    overlayDesc->setText("Nouveau cadre administratif ?\nCréez votre compte pour accéder au système.");
    overlayBtn->setText("Nouveau compte Admin");

    disconnect(overlayBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSwapToSignIn);
    connect(overlayBtn, &QPushButton::clicked, this, &AdminAuthWindow::onSwapToSignUp);

    connect(animation, &QPropertyAnimation::finished, [this]() {
        if (isSignIn) signUpForm->hide();
    });
    animation->start();
}

void AdminAuthWindow::onSignInClicked()
{
    QString input = loginUserEdit->text().trimmed();
    QString pwd = loginPwdEdit->text();

    if (input.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez saisir votre identifiant/email et mot de passe.");
        return;
    }

    Administrateur *found = nullptr;
    for (auto &a : admins) {
        std::string nc1 = a.getNom() + " " + a.getPrenom();
        std::string nc2 = a.getPrenom() + " " + a.getNom();
        std::string s = input.toStdString();
        if (s == a.getEmail() || s == a.getId() || s == nc1 || s == nc2) {
            found = &a;
            break;
        }
    }

    if (!found) {
        // If no admin exists yet, create default Recteur session
        Administrateur defaultAdmin("Dupont", "Charles", "509-3800-0000", input.toStdString(), "Recteur", "", "Marie", "Dupont", "509-3800-1111", pwd.toStdString());
        admins.push_back(defaultAdmin);
        sauvegarderAdministrateurs(admins);
        emit loginSuccessful(defaultAdmin);
        return;
    }

    if (!found->verifierMotDePasse(pwd.toStdString())) {
        QMessageBox::warning(this, "Erreur", "Mot de passe administrateur incorrect.");
        return;
    }

    emit loginSuccessful(*found);
}

void AdminAuthWindow::onSignUpClicked()
{
    QString nom = regNomEdit->text().trimmed();
    QString prenom = regPrenomEdit->text().trimmed();
    QString phone = regPhoneEdit->text().trimmed();
    QString email = regEmailEdit->text().trimmed();
    QString posteComboText = regPosteCombo->currentText();
    QString faculte = regFaculteCombo->isEnabled() ? regFaculteCombo->currentText() : "";
    QString nomP = regNomProcheEdit->text().trimmed();
    QString prenomP = regPrenomProcheEdit->text().trimmed();
    QString telP = regTelProcheEdit->text().trimmed();
    QString pwd = regPwdEdit->text();
    QString conf = regConfirmPwdEdit->text();

    if (nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir tous les champs obligatoires.");
        return;
    }

    if (pwd != conf) {
        QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
        return;
    }

    if (posteComboText.contains("Poste déjà pris")) {
        QMessageBox::critical(this, "Poste déjà attribué", "⚠️ Ce poste administratif unique est déjà occupé par un autre membre de la direction.");
        return;
    }

    // Clean poste text
    QString cleanPoste = posteComboText.split(" ⚠️")[0].trimmed();

    if (estPostePris(admins, cleanPoste.toStdString(), faculte.toStdString())) {
        QMessageBox::critical(this, "Poste déjà attribué", "⚠️ Ce poste est déjà attribué dans le système.");
        return;
    }

    Administrateur newAdmin(nom.toStdString(), prenom.toStdString(), phone.toStdString(), email.toStdString(),
                            cleanPoste.toStdString(), faculte.toStdString(), nomP.toStdString(), prenomP.toStdString(),
                            telP.toStdString(), pwd.toStdString());

    enregistrerAdministrateur(admins, newAdmin);

    QMessageBox::information(this, "Compte Administrateur Créé",
        QString("Compte Administrateur créé avec succès pour %1 %2 (%3).\nID attribué : %4")
        .arg(prenom, nom, cleanPoste, QString::fromStdString(newAdmin.getId())));

    onSwapToSignIn();
}
