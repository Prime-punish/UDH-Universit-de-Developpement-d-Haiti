#include "teacherauthwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QPixmap>

TeacherAuthWindow::TeacherAuthWindow(std::vector<Professeur>& profsRef, QWidget *parent)
    : QWidget(parent), profs(profsRef), isSignIn(true)
{
    setupUI();
}

TeacherAuthWindow::~TeacherAuthWindow() {}

void TeacherAuthWindow::setupUI()
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
    connect(backBtn, &QPushButton::clicked, this, &TeacherAuthWindow::backToLanding);
    topBar->addWidget(backBtn);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    formContainer = new QWidget(this);
    formContainer->setFixedSize(900, 600);
    formContainer->setStyleSheet("background-color: white; border-radius: 15px;");
    rootLayout->addWidget(formContainer, 0, Qt::AlignCenter);

    signInForm = createSignInForm();
    signInForm->setParent(formContainer);
    signInForm->setGeometry(450, 0, 450, 600);

    signUpForm = createSignUpForm();
    signUpForm->setParent(formContainer);
    signUpForm->setGeometry(0, 0, 450, 600);
    signUpForm->hide();

    overlayPanel = new QWidget(formContainer);
    overlayPanel->setGeometry(0, 0, 450, 600);
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

    overlayTitle = new QLabel("PORTAIL ENSEIGNANT");
    overlayTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #d4af37; background: transparent; letter-spacing: 1px;");
    overlayTitle->setAlignment(Qt::AlignCenter);

    QLabel *univSubtitle = new QLabel("UNIVERSITÉ DE DÉVELOPPEMENT D'HAÏTI");
    univSubtitle->setStyleSheet("font-size: 11px; font-weight: 600; color: #cfd8dc; background: transparent; text-align: center;");
    univSubtitle->setAlignment(Qt::AlignCenter);
    univSubtitle->setWordWrap(true);

    overlayDesc = new QLabel("Nouveau membre du corps professoral ?\nCréez votre accès pour gérer vos cours et étudiants.");
    overlayDesc->setStyleSheet("font-size: 13px; margin-top: 10px; margin-bottom: 20px; color: #b0bec5; background: transparent;");
    overlayDesc->setAlignment(Qt::AlignCenter);
    overlayDesc->setWordWrap(true);

    overlayBtn = new QPushButton("Créer un compte");
    overlayBtn->setFixedSize(170, 42);
    overlayBtn->setCursor(Qt::PointingHandCursor);
    overlayBtn->setStyleSheet(
        "QPushButton { border: 2px solid #d4af37; border-radius: 21px; color: #d4af37; font-weight: bold; font-size: 14px; background-color: transparent; }"
        "QPushButton:hover { background-color: #d4af37; color: #0b1e36; }"
    );
    connect(overlayBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSwapToSignUp);

    overlayLayout->addWidget(overlayLogo);
    overlayLayout->addWidget(overlayTitle);
    overlayLayout->addWidget(univSubtitle);
    overlayLayout->addWidget(overlayDesc);
    overlayLayout->addWidget(overlayBtn, 0, Qt::AlignHCenter);

    animation = new QPropertyAnimation(overlayPanel, "geometry");
    animation->setDuration(400);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
}

QWidget* TeacherAuthWindow::createSignInForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(50, 40, 50, 40);
    layout->setSpacing(18);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Connexion Enseignant");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    QLabel *subTitle = new QLabel("Espace dédié au corps professoral");
    subTitle->setStyleSheet("font-size: 12px; color: #718096; background: transparent;");
    subTitle->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; }";

    loginEmailEdit = new QLineEdit();
    loginEmailEdit->setPlaceholderText("Email professionnel ou nom complet");
    loginEmailEdit->setFixedHeight(42);
    loginEmailEdit->setStyleSheet(inputStyle);

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
    connect(showHideBtn, &QPushButton::clicked, this, &TeacherAuthWindow::togglePasswordVisibility);

    pwdLayout->addWidget(loginPwdEdit);
    pwdLayout->addWidget(showHideBtn);

    QPushButton *signInBtn = new QPushButton("Se connecter");
    signInBtn->setFixedHeight(45);
    signInBtn->setCursor(Qt::PointingHandCursor);
    signInBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 15px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signInBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSignInClicked);

    layout->addWidget(title);
    layout->addWidget(subTitle);
    layout->addSpacing(10);
    layout->addWidget(loginEmailEdit);
    layout->addLayout(pwdLayout);
    layout->addSpacing(5);
    layout->addWidget(signInBtn);

    return widget;
}

QWidget* TeacherAuthWindow::createSignUpForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(50, 25, 50, 25);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Inscription Enseignant");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 6px 10px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus, QComboBox:focus { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1px solid #cbd5e0; outline: none; }"
        "QComboBox QAbstractItemView::item { min-height: 32px; padding: 4px 8px; color: #1a202c; background-color: #ffffff; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }"
        "QComboBox QAbstractItemView::item:selected { background-color: #dbe4f9; color: #14166B; font-weight: bold; }";

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setSpacing(10);
    regNomEdit = new QLineEdit(); regNomEdit->setPlaceholderText("Nom");
    regNomEdit->setFixedHeight(36); regNomEdit->setStyleSheet(inputStyle);
    regPrenomEdit = new QLineEdit(); regPrenomEdit->setPlaceholderText("Prénom");
    regPrenomEdit->setFixedHeight(36); regPrenomEdit->setStyleSheet(inputStyle);
    nameLayout->addWidget(regNomEdit);
    nameLayout->addWidget(regPrenomEdit);

    regEmailEdit = new QLineEdit(); regEmailEdit->setPlaceholderText("Email académique");
    regEmailEdit->setFixedHeight(36); regEmailEdit->setStyleSheet(inputStyle);

    regPhoneEdit = new QLineEdit(); regPhoneEdit->setPlaceholderText("Téléphone");
    regPhoneEdit->setFixedHeight(36); regPhoneEdit->setStyleSheet(inputStyle);

    regDeptCombo = new QComboBox();
    regDeptCombo->setFixedHeight(36);
    regDeptCombo->setStyleSheet(inputStyle);
    regDeptCombo->addItems({
        "Genie informatique",
        "Genie civil",
        "Science economique et comptabilite",
        "Science administration",
        "Science juridique",
        "Science education"
    });

    regPwdEdit = new QLineEdit(); regPwdEdit->setPlaceholderText("Mot de passe");
    regPwdEdit->setEchoMode(QLineEdit::Password);
    regPwdEdit->setFixedHeight(36); regPwdEdit->setStyleSheet(inputStyle);

    regConfirmPwdEdit = new QLineEdit(); regConfirmPwdEdit->setPlaceholderText("Confirmer mot de passe");
    regConfirmPwdEdit->setEchoMode(QLineEdit::Password);
    regConfirmPwdEdit->setFixedHeight(36); regConfirmPwdEdit->setStyleSheet(inputStyle);

    QPushButton *signUpBtn = new QPushButton("Créer mon compte");
    signUpBtn->setFixedHeight(40);
    signUpBtn->setCursor(Qt::PointingHandCursor);
    signUpBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signUpBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSignUpClicked);

    layout->addWidget(title);
    layout->addSpacing(3);
    layout->addLayout(nameLayout);
    layout->addWidget(regEmailEdit);
    layout->addWidget(regPhoneEdit);
    layout->addWidget(regDeptCombo);
    layout->addWidget(regPwdEdit);
    layout->addWidget(regConfirmPwdEdit);
    layout->addSpacing(5);
    layout->addWidget(signUpBtn);

    return widget;
}

void TeacherAuthWindow::togglePasswordVisibility()
{
    if (loginPwdEdit->echoMode() == QLineEdit::Password) {
        loginPwdEdit->setEchoMode(QLineEdit::Normal);
        qobject_cast<QPushButton*>(sender())->setText("Cacher");
    } else {
        loginPwdEdit->setEchoMode(QLineEdit::Password);
        qobject_cast<QPushButton*>(sender())->setText("Afficher");
    }
}

void TeacherAuthWindow::onSwapToSignUp()
{
    if (animation->state() == QPropertyAnimation::Running) return;
    isSignIn = false;
    signUpForm->show();
    signUpForm->setGeometry(0, 0, 450, 600);

    animation->setStartValue(QRect(0, 0, 450, 600));
    animation->setEndValue(QRect(450, 0, 450, 600));

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-right-radius: 15px; border-bottom-right-radius: 15px;");
    overlayTitle->setText("DEJÀ INSCRIT ?");
    overlayDesc->setText("Connectez-vous pour accéder à votre espace enseignant.");
    overlayBtn->setText("Se connecter");

    disconnect(overlayBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSwapToSignUp);
    connect(overlayBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSwapToSignIn);

    connect(animation, &QPropertyAnimation::finished, [this]() {
        if (!isSignIn) signInForm->hide();
    });
    animation->start();
}

void TeacherAuthWindow::onSwapToSignIn()
{
    if (animation->state() == QPropertyAnimation::Running) return;
    isSignIn = true;
    signInForm->show();
    signInForm->setGeometry(450, 0, 450, 600);

    animation->setStartValue(QRect(450, 0, 450, 600));
    animation->setEndValue(QRect(0, 0, 450, 600));

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-left-radius: 15px; border-bottom-left-radius: 15px;");
    overlayTitle->setText("PORTAIL ENSEIGNANT");
    overlayDesc->setText("Nouveau membre du corps professoral ?\nCréez votre accès pour gérer vos cours.");
    overlayBtn->setText("Créer un compte");

    disconnect(overlayBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSwapToSignIn);
    connect(overlayBtn, &QPushButton::clicked, this, &TeacherAuthWindow::onSwapToSignUp);

    connect(animation, &QPropertyAnimation::finished, [this]() {
        if (isSignIn) signUpForm->hide();
    });
    animation->start();
}

void TeacherAuthWindow::onSignInClicked()
{
    QString idOrEmail = loginEmailEdit->text().trimmed();
    QString pwd = loginPwdEdit->text();

    if (idOrEmail.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez saisir votre email / identifiant et mot de passe.");
        return;
    }

    Professeur *found = nullptr;
    for (auto &p : profs) {
        std::string nomComplet1 = p.getNom() + " " + p.getPrenom();
        std::string nomComplet2 = p.getPrenom() + " " + p.getNom();
        std::string inputStr = idOrEmail.toStdString();
        if (inputStr == p.getEmail() || inputStr == p.getId() || inputStr == nomComplet1 || inputStr == nomComplet2) {
            found = &p;
            break;
        }
    }

    if (!found) {
        // If not found in file, create a default prof session for demonstration
        std::vector<std::string> defaultMatieres = {"Algorithmique et programmation", "Structures de données"};
        Professeur demoProf("Jean", "Baptiste", idOrEmail.toStdString(), "509-3700-1122", pwd.toStdString(), "Genie informatique", defaultMatieres);
        profs.push_back(demoProf);
        sauvegarderProfesseurs(profs);
        emit loginSuccessful(demoProf);
        return;
    }

    if (!found->verifierMotDePasse(pwd.toStdString())) {
        QMessageBox::warning(this, "Erreur", "Mot de passe incorrect.");
        return;
    }

    emit loginSuccessful(*found);
}

void TeacherAuthWindow::onSignUpClicked()
{
    QString nom = regNomEdit->text().trimmed();
    QString prenom = regPrenomEdit->text().trimmed();
    QString email = regEmailEdit->text().trimmed();
    QString phone = regPhoneEdit->text().trimmed();
    QString dept = regDeptCombo->currentText();
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

    std::vector<std::string> defaultMatieres;
    if (dept == "Genie informatique") {
        defaultMatieres = {"Algorithmique et programmation", "Structures de données"};
    } else {
        defaultMatieres = {"Introduction générale", "Pratiques avancées"};
    }

    Professeur newProf(nom.toStdString(), prenom.toStdString(), email.toStdString(), phone.toStdString(), pwd.toStdString(), dept.toStdString(), defaultMatieres);
    enregistrerProfesseur(profs, newProf);

    QMessageBox::information(this, "Compte Créé", QString("Compte enseignant créé pour Prof. %1 %2 (ID: %3).\nVous pouvez maintenant vous connecter.").arg(prenom, nom, QString::fromStdString(newProf.getId())));
    onSwapToSignIn();
}
