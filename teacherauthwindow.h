#ifndef TEACHERAUTHWINDOW_H
#define TEACHERAUTHWINDOW_H

#include <QWidget>
#include <vector>
#include "professeur.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QPropertyAnimation;
class QComboBox;

class TeacherAuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherAuthWindow(std::vector<Professeur>& profsRef, QWidget *parent = nullptr);
    ~TeacherAuthWindow();

signals:
    void loginSuccessful(const Professeur& prof);
    void backToLanding();

private slots:
    void onSignInClicked();
    void onSignUpClicked();
    void onSwapToSignUp();
    void onSwapToSignIn();
    void togglePasswordVisibility();

private:
    void setupUI();
    QWidget* createSignInForm();
    QWidget* createSignUpForm();

    std::vector<Professeur>& profs;

    QWidget *formContainer;
    QWidget *signInForm;
    QWidget *signUpForm;
    QWidget *overlayPanel;
    QLabel *overlayTitle;
    QLabel *overlayDesc;
    QPushButton *overlayBtn;

    // Sign in fields
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPwdEdit;

    // Sign up fields
    QLineEdit *regNomEdit;
    QLineEdit *regPrenomEdit;
    QLineEdit *regEmailEdit;
    QLineEdit *regPhoneEdit;
    QComboBox *regDeptCombo;
    QLineEdit *regPwdEdit;
    QLineEdit *regConfirmPwdEdit;

    QPropertyAnimation *animation;
    bool isSignIn;
};

#endif // TEACHERAUTHWINDOW_H
