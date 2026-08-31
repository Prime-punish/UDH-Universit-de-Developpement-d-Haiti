#ifndef ADMINAUTHWINDOW_H
#define ADMINAUTHWINDOW_H

#include <QWidget>
#include <vector>
#include <QComboBox>
#include "administrateur.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QPropertyAnimation;

class AdminAuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminAuthWindow(std::vector<Administrateur>& adminsRef, QWidget *parent = nullptr);
    ~AdminAuthWindow();

signals:
    void loginSuccessful(const Administrateur& admin);
    void backToLanding();

private slots:
    void onSignInClicked();
    void onSignUpClicked();
    void onSwapToSignUp();
    void onSwapToSignIn();
    void onPosteChanged(int index);
    void togglePasswordVisibility();

private:
    void setupUI();
    QWidget* createSignInForm();
    QWidget* createSignUpForm();
    void updatePosteComboOptions();

    std::vector<Administrateur>& admins;

    QWidget *formContainer;
    QWidget *signInForm;
    QWidget *signUpForm;
    QWidget *overlayPanel;
    QLabel *overlayTitle;
    QLabel *overlayDesc;
    QPushButton *overlayBtn;

    // Sign in fields
    QLineEdit *loginUserEdit;
    QLineEdit *loginPwdEdit;

    // Sign up fields
    QLineEdit *regNomEdit;
    QLineEdit *regPrenomEdit;
    QLineEdit *regPhoneEdit;
    QLineEdit *regEmailEdit;
    QComboBox *regPosteCombo;
    QComboBox *regFaculteCombo;
    QLineEdit *regNomProcheEdit;
    QLineEdit *regPrenomProcheEdit;
    QLineEdit *regTelProcheEdit;
    QLineEdit *regPwdEdit;
    QLineEdit *regConfirmPwdEdit;
    QLabel *posteWarningLabel;

    QPropertyAnimation *animation;
    bool isSignIn;
};

#endif // ADMINAUTHWINDOW_H
