#ifndef STUDENTAUTHWINDOW_H
#define STUDENTAUTHWINDOW_H

#include <QWidget>
#include <vector>
#include "etudiant.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QPropertyAnimation;

class StudentAuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StudentAuthWindow(std::vector<CompteEtudiant>& comptesRef, QWidget *parent = nullptr);
    ~StudentAuthWindow();

signals:
    void loginSuccessful(const CompteEtudiant& compte);
    void backToLanding();

private slots:
    void onSignInClicked();
    void onSignUpClicked();
    void onSwapToSignUp();
    void onSwapToSignIn();
    void toggleLoginPasswordVisibility();
    void promptRegistration(CompteEtudiant& newCompte);

private:
    void setupUI();
    QWidget* createSignInForm();
    QWidget* createSignUpForm();
    
    std::vector<CompteEtudiant>& comptes;
    
    QWidget *formContainer;
    QWidget *signInForm;
    QWidget *signUpForm;
    QWidget *overlayPanel;
    QLabel *overlayTitle;
    QLabel *overlayDesc;
    QPushButton *overlayBtn;

    // Sign in fields
    QLineEdit *loginIdentifierEdit;
    QLineEdit *loginPasswordEdit;

    // Sign up fields
    QLineEdit *regNomEdit;
    QLineEdit *regPrenomEdit;
    QLineEdit *regEmailEdit;
    QLineEdit *regPhoneEdit;
    QLineEdit *regPasswordEdit;
    QLineEdit *regConfirmPasswordEdit;
    
    QPropertyAnimation *animation;
    bool isSignIn; // True if sign in is visible
};

#endif // STUDENTAUTHWINDOW_H
