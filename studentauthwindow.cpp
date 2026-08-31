#include "studentauthwindow.h"
#include "registrationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QPixmap>
#include <QDateTime>

StudentAuthWindow::StudentAuthWindow(std::vector<CompteEtudiant>& comptesRef, QWidget *parent)
    : QWidget(parent), comptes(comptesRef), isSignIn(true)
{
    setupUI();
}

StudentAuthWindow::~StudentAuthWindow() {}

void StudentAuthWindow::setupUI()
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
    connect(backBtn, &QPushButton::clicked, this, &StudentAuthWindow::backToLanding);
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

    // University Logo in Overlay
    QLabel *overlayLogo = new QLabel();
    QPixmap logoPix(":/resources/logo.png");
    if (!logoPix.isNull()) {
        overlayLogo->setPixmap(logoPix.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    overlayLogo->setAlignment(Qt::AlignCenter);
    overlayLogo->setStyleSheet("background: transparent; margin-bottom: 5px;");

    overlayTitle = new QLabel("BIENVENUE");
    overlayTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #d4af37; background: transparent; letter-spacing: 1px;");
    overlayTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *univSubtitle = new QLabel("UNIVERSITÉ DE DÉVELOPPEMENT D'HAÏTI");
    univSubtitle->setStyleSheet("font-size: 11px; font-weight: 600; color: #cfd8dc; background: transparent; text-align: center;");
    univSubtitle->setAlignment(Qt::AlignCenter);
    univSubtitle->setWordWrap(true);

    overlayDesc = new QLabel("Vous n'avez pas de compte ?\nInscrivez-vous pour accéder à votre portail étudiant.");
    overlayDesc->setStyleSheet("font-size: 13px; margin-top: 10px; margin-bottom: 20px; color: #b0bec5; background: transparent;");
    overlayDesc->setAlignment(Qt::AlignCenter);
    overlayDesc->setWordWrap(true);
    
    overlayBtn = new QPushButton("S'inscrire");
    overlayBtn->setFixedSize(160, 42);
    overlayBtn->setStyleSheet("QPushButton { border: 2px solid #d4af37; border-radius: 21px; color: #d4af37; font-weight: bold; font-size: 14px; background-color: transparent; }"
                              "QPushButton:hover { background-color: #d4af37; color: #0b1e36; }");
    connect(overlayBtn, &QPushButton::clicked, this, &StudentAuthWindow::onSwapToSignUp);

    overlayLayout->addWidget(overlayLogo);
    overlayLayout->addWidget(overlayTitle);
    overlayLayout->addWidget(univSubtitle);
    overlayLayout->addWidget(overlayDesc);
    overlayLayout->addWidget(overlayBtn, 0, Qt::AlignHCenter);

    animation = new QPropertyAnimation(overlayPanel, "geometry");
    animation->setDuration(400);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
}

QWidget* StudentAuthWindow::createSignInForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(50, 40, 50, 40);
    layout->setSpacing(18);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Connexion Étudiant");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    QLabel *subTitle = new QLabel("Accédez à vos cours, notes et emploi du temps");
    subTitle->setStyleSheet("font-size: 12px; color: #718096; background: transparent;");
    subTitle->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 8px 12px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; }";

    loginIdentifierEdit = new QLineEdit();
    loginIdentifierEdit->setPlaceholderText("Email ou Nom / Prénom");
    loginIdentifierEdit->setFixedHeight(42);
    loginIdentifierEdit->setStyleSheet(inputStyle);

    QHBoxLayout *pwdLayout = new QHBoxLayout();
    pwdLayout->setContentsMargins(0, 0, 0, 0);
    pwdLayout->setSpacing(0);
    
    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setPlaceholderText("Mot de passe");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setFixedHeight(42);
    loginPasswordEdit->setStyleSheet(
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
    connect(showHideBtn, &QPushButton::clicked, this, &StudentAuthWindow::toggleLoginPasswordVisibility);

    pwdLayout->addWidget(loginPasswordEdit);
    pwdLayout->addWidget(showHideBtn);

    QPushButton *signInBtn = new QPushButton("Se connecter");
    signInBtn->setFixedHeight(45);
    signInBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 15px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signInBtn, &QPushButton::clicked, this, &StudentAuthWindow::onSignInClicked);

    layout->addWidget(title);
    layout->addWidget(subTitle);
    layout->addSpacing(10);
    layout->addWidget(loginIdentifierEdit);
    layout->addLayout(pwdLayout);
    layout->addSpacing(5);
    layout->addWidget(signInBtn);

    return widget;
}

QWidget* StudentAuthWindow::createSignUpForm()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(50, 30, 50, 30);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Créer un Compte Étudiant");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; background: transparent;");
    title->setAlignment(Qt::AlignCenter);

    const QString inputStyle = 
        "QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #a0aec0; "
        "  border-radius: 6px; padding: 6px 10px; font-size: 13px; font-weight: 500; }"
        "QLineEdit:focus { border: 1.5px solid #2b6cb0; }";

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setSpacing(10);
    regNomEdit = new QLineEdit(); regNomEdit->setPlaceholderText("Nom de famille");
    regNomEdit->setFixedHeight(38); regNomEdit->setStyleSheet(inputStyle);
    regPrenomEdit = new QLineEdit(); regPrenomEdit->setPlaceholderText("Prénom");
    regPrenomEdit->setFixedHeight(38); regPrenomEdit->setStyleSheet(inputStyle);
    nameLayout->addWidget(regNomEdit);
    nameLayout->addWidget(regPrenomEdit);

    regEmailEdit = new QLineEdit(); regEmailEdit->setPlaceholderText("Adresse Email");
    regEmailEdit->setFixedHeight(38); regEmailEdit->setStyleSheet(inputStyle);
    
    regPhoneEdit = new QLineEdit(); regPhoneEdit->setPlaceholderText("Numéro de téléphone");
    regPhoneEdit->setFixedHeight(38); regPhoneEdit->setStyleSheet(inputStyle);

    regPasswordEdit = new QLineEdit(); regPasswordEdit->setPlaceholderText("Mot de passe (4 car. min)");
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regPasswordEdit->setFixedHeight(38); regPasswordEdit->setStyleSheet(inputStyle);

    regConfirmPasswordEdit = new QLineEdit(); regConfirmPasswordEdit->setPlaceholderText("Confirmer le mot de passe");
    regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    regConfirmPasswordEdit->setFixedHeight(38); regConfirmPasswordEdit->setStyleSheet(inputStyle);

    QPushButton *signUpBtn = new QPushButton("Créer mon compte");
    signUpBtn->setFixedHeight(42);
    signUpBtn->setStyleSheet(
        "QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 14px; border: none; }"
        "QPushButton:hover { background-color: #1a3353; }"
    );
    connect(signUpBtn, &QPushButton::clicked, this, &StudentAuthWindow::onSignUpClicked);

    layout->addWidget(title);
    layout->addSpacing(5);
    layout->addLayout(nameLayout);
    layout->addWidget(regEmailEdit);
    layout->addWidget(regPhoneEdit);
    layout->addWidget(regPasswordEdit);
    layout->addWidget(regConfirmPasswordEdit);
    layout->addSpacing(5);
    layout->addWidget(signUpBtn);

    return widget;
}

void StudentAuthWindow::toggleLoginPasswordVisibility()
{
    if (loginPasswordEdit->echoMode() == QLineEdit::Password) {
        loginPasswordEdit->setEchoMode(QLineEdit::Normal);
        qobject_cast<QPushButton*>(sender())->setText("Cacher");
    } else {
        loginPasswordEdit->setEchoMode(QLineEdit::Password);
        qobject_cast<QPushButton*>(sender())->setText("Afficher");
    }
}

void StudentAuthWindow::onSwapToSignUp()
{
    isSignIn = false;
    signInForm->hide();
    signUpForm->show();
    
    overlayTitle->setText("BIENVENUE !");
    overlayDesc->setText("Vous avez déjà un compte ?\nConnectez-vous pour accéder à votre tableau de bord.");
    overlayBtn->setText("Se connecter");
    overlayBtn->disconnect();
    connect(overlayBtn, &QPushButton::clicked, this, &StudentAuthWindow::onSwapToSignIn);

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-right-radius: 15px; border-bottom-right-radius: 15px;");

    animation->setStartValue(QRect(0, 0, 450, 600));
    animation->setEndValue(QRect(450, 0, 450, 600));
    animation->start();
}

void StudentAuthWindow::onSwapToSignIn()
{
    isSignIn = true;
    signUpForm->hide();
    signInForm->show();
    
    overlayTitle->setText("BIENVENUE !");
    overlayDesc->setText("Vous n'avez pas de compte ?\nInscrivez-vous pour accéder à votre portail étudiant.");
    overlayBtn->setText("S'inscrire");
    overlayBtn->disconnect();
    connect(overlayBtn, &QPushButton::clicked, this, &StudentAuthWindow::onSwapToSignUp);

    overlayPanel->setStyleSheet("background-color: #0b1e36; color: white; border-top-left-radius: 15px; border-bottom-left-radius: 15px;");

    animation->setStartValue(QRect(450, 0, 450, 600));
    animation->setEndValue(QRect(0, 0, 450, 600));
    animation->start();
}

void StudentAuthWindow::onSignInClicked()
{
    QString identifier = loginIdentifierEdit->text().trimmed();
    QString password = loginPasswordEdit->text();

    if (identifier.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Veuillez remplir tous les champs.");
        return;
    }

    CompteEtudiant *compteTrouve = nullptr;
    for (auto &c : comptes) {
        QString nomComplet1 = QString::fromStdString(c.getNom() + " " + c.getPrenom());
        QString nomComplet2 = QString::fromStdString(c.getPrenom() + " " + c.getNom());
        QString email = QString::fromStdString(c.getEmail());
        
        if (identifier == email || identifier == nomComplet1 || identifier == nomComplet2) {
            compteTrouve = &c;
            break;
        }
    }

    if (!compteTrouve) {
        QMessageBox::warning(this, "Erreur", "Compte non trouvé.");
        return;
    }

    if (compteTrouve->verifierMotDePasse(password.toStdString())) {
        std::string status = compteTrouve->getStatutInscription();
        if (status == "Rejeté") {
            QString msg = QString("Votre inscription a été rejetée par le secrétariat de l'université.");
            if (!compteTrouve->getMotifRejet().empty()) {
                msg += QString("\n\nMotif du rejet : %1").arg(QString::fromStdString(compteTrouve->getMotifRejet()));
            }
            QMessageBox::critical(this, "Inscription Rejetée", msg);
            return;
        } else if (status == "En attente" || (!compteTrouve->estActif() && status != "Approuvé")) {
            QMessageBox::warning(this, "Validation en attente", "⏳ Votre inscription est en attente de validation par le secrétariat.\n\nVous pourrez vous connecter dès que votre dossier aura été approuvé.");
            return;
        }
        emit loginSuccessful(*compteTrouve);
    } else {
        auto reply = QMessageBox::question(this, "Erreur", "Mot de passe incorrect. Voulez-vous le reinitialiser?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            bool ok;
            QString emailConfirm = QInputDialog::getText(this, "Réinitialiser le mot de passe", "Confirmez votre email:", QLineEdit::Normal, "", &ok);
            if (ok && emailConfirm.toStdString() == compteTrouve->getEmail()) {
                QString newPwd = QInputDialog::getText(this, "Nouveau mot de passe", "Entrez votre nouveau mot de passe:", QLineEdit::Password, "", &ok);
                if (ok && newPwd.length() >= 4) {
                    compteTrouve->modifierMotDePasse(newPwd.toStdString());
                    enregistrerCompte(comptes, *compteTrouve);
                    QMessageBox::information(this, "Succès", "Mot de passe réinitialisé avec succès. Veuillez vous reconnecter.");
                } else {
                    QMessageBox::warning(this, "Erreur", "Le mot de passe doit contenir au moins 4 caractères.");
                }
            } else if (ok) {
                QMessageBox::warning(this, "Erreur", "Email ne correspond pas.");
            }
        }
    }
}

void StudentAuthWindow::onSignUpClicked()
{
    QString nom = regNomEdit->text().trimmed();
    QString prenom = regPrenomEdit->text().trimmed();
    QString email = regEmailEdit->text().trimmed();
    QString phone = regPhoneEdit->text().trimmed();
    QString pwd = regPasswordEdit->text();
    QString confirmPwd = regConfirmPasswordEdit->text();

    if (nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || phone.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }
    if (pwd != confirmPwd) {
        QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
        return;
    }
    if (pwd.length() < 4) {
        QMessageBox::warning(this, "Erreur", "Le mot de passe doit contenir au moins 4 caractères.");
        return;
    }

    CompteEtudiant newCompte(nom.toStdString(), prenom.toStdString(), email.toStdString(), phone.toStdString(), pwd.toStdString());
    newCompte.setStatutInscription("En attente");
    newCompte.setActif(false);
    newCompte.setDateSoumission(QDateTime::currentDateTime().toString("dd/MM/yyyy").toStdString());
    enregistrerCompte(comptes, newCompte);

    QMessageBox::information(this, "Compte Créé", QString("Compte créé avec succès !\nID étudiant : %1").arg(QString::fromStdString(newCompte.getId())));
    
    promptRegistration(newCompte);
}

void StudentAuthWindow::promptRegistration(CompteEtudiant& newCompte)
{
    auto reply = QMessageBox::question(
        this, 
        "Inscription Universitaire", 
        "Votre compte a été créé.\n\nSouhaitez-vous compléter votre dossier d'inscription maintenant pour le soumettre à la validation du secrétariat ?", 
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        RegistrationDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            newCompte.setInscription(dialog.getInscriptionData());
            newCompte.setInscriptionFaite(true);
            newCompte.setStatutInscription("En attente");
            newCompte.setActif(false);
            newCompte.setDateSoumission(QDateTime::currentDateTime().toString("dd/MM/yyyy").toStdString());

            enregistrerCompte(comptes, newCompte);
            QMessageBox::information(this, "Dossier Soumis", "📩 Votre dossier d'inscription a été transmis avec succès.\nIl est désormais en attente de validation par le secrétariat.");
        }
    }
    
    onSwapToSignIn();
}
