#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <vector>
#include <map>
#include "etudiant.h"
#include "professeur.h"
#include "administrateur.h"
#include "landingwindow.h"
#include "studentauthwindow.h"
#include "teacherauthwindow.h"
#include "adminauthwindow.h"
#include "studentdashboardwindow.h"
#include "teacherdashboardwindow.h"
#include "admindashboardwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRoleSelected(int role);
    void onBackToLanding();
    void onStudentLoginSuccessful(const CompteEtudiant& compte);
    void onTeacherLoginSuccessful(const Professeur& prof);
    void onAdminLoginSuccessful(const Administrateur& admin);
    void onLogoutRequested();

private:
    void loadData();

    QStackedWidget *mainStack;
    LandingWindow *landingWindow;
    StudentAuthWindow *studentAuthWindow;
    TeacherAuthWindow *teacherAuthWindow;
    AdminAuthWindow *adminAuthWindow;
    StudentDashboardWindow *studentDashboardWindow;
    TeacherDashboardWindow *teacherDashboardWindow;
    AdminDashboardWindow *adminDashboardWindow;

    std::vector<CompteEtudiant> comptes;
    std::vector<Professeur> profs;
    std::vector<Administrateur> admins;
    std::map<std::string, ProgrammeEtudes> programmes;
};

#endif // MAINWINDOW_H
