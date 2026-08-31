#include "mainwindow.h"
#include <QIcon>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), studentDashboardWindow(nullptr), teacherDashboardWindow(nullptr), adminDashboardWindow(nullptr)
{
    setMinimumSize(1100, 700);
    resize(1200, 750);
    setWindowTitle("UDH - Université de Développement d'Haïti");
    setWindowIcon(QIcon(":/resources/logo.png"));

    loadData();

    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // 0. Landing Window (Initial screen)
    landingWindow = new LandingWindow(this);
    connect(landingWindow, &LandingWindow::roleSelected, this, &MainWindow::onRoleSelected);
    mainStack->addWidget(landingWindow);

    // 1. Student Auth Window
    studentAuthWindow = new StudentAuthWindow(comptes, this);
    connect(studentAuthWindow, &StudentAuthWindow::loginSuccessful, this, &MainWindow::onStudentLoginSuccessful);
    connect(studentAuthWindow, &StudentAuthWindow::backToLanding, this, &MainWindow::onBackToLanding);
    mainStack->addWidget(studentAuthWindow);

    // 2. Teacher Auth Window
    teacherAuthWindow = new TeacherAuthWindow(profs, this);
    connect(teacherAuthWindow, &TeacherAuthWindow::loginSuccessful, this, &MainWindow::onTeacherLoginSuccessful);
    connect(teacherAuthWindow, &TeacherAuthWindow::backToLanding, this, &MainWindow::onBackToLanding);
    mainStack->addWidget(teacherAuthWindow);

    // 3. Admin Auth Window
    adminAuthWindow = new AdminAuthWindow(admins, this);
    connect(adminAuthWindow, &AdminAuthWindow::loginSuccessful, this, &MainWindow::onAdminLoginSuccessful);
    connect(adminAuthWindow, &AdminAuthWindow::backToLanding, this, &MainWindow::onBackToLanding);
    mainStack->addWidget(adminAuthWindow);

    // Set initial screen to Landing Window
    mainStack->setCurrentWidget(landingWindow);
}

MainWindow::~MainWindow() = default;

void MainWindow::loadData()
{
    programmes = construireProgrammes();
    comptes    = chargerComptes();
    profs      = chargerProfesseurs();
    admins     = chargerAdministrateurs();
}

void MainWindow::onRoleSelected(int role)
{
    if (role == LandingWindow::RoleEtudiant) {
        mainStack->setCurrentWidget(studentAuthWindow);
    } else if (role == LandingWindow::RoleProfesseur) {
        mainStack->setCurrentWidget(teacherAuthWindow);
    } else if (role == LandingWindow::RoleAdministrateur) {
        mainStack->setCurrentWidget(adminAuthWindow);
    }
}

void MainWindow::onBackToLanding()
{
    mainStack->setCurrentWidget(landingWindow);
}

void MainWindow::onStudentLoginSuccessful(const CompteEtudiant& compte)
{
    if (studentDashboardWindow) {
        mainStack->removeWidget(studentDashboardWindow);
        delete studentDashboardWindow;
    }
    
    studentDashboardWindow = new StudentDashboardWindow(compte, programmes, comptes, this);
    connect(studentDashboardWindow, &StudentDashboardWindow::logoutRequested, this, &MainWindow::onLogoutRequested);
    
    mainStack->addWidget(studentDashboardWindow);
    mainStack->setCurrentWidget(studentDashboardWindow);
}

void MainWindow::onTeacherLoginSuccessful(const Professeur& prof)
{
    if (teacherDashboardWindow) {
        mainStack->removeWidget(teacherDashboardWindow);
        delete teacherDashboardWindow;
    }

    teacherDashboardWindow = new TeacherDashboardWindow(prof, profs, this);
    connect(teacherDashboardWindow, &TeacherDashboardWindow::logoutRequested, this, &MainWindow::onLogoutRequested);

    mainStack->addWidget(teacherDashboardWindow);
    mainStack->setCurrentWidget(teacherDashboardWindow);
}

void MainWindow::onAdminLoginSuccessful(const Administrateur& admin)
{
    if (adminDashboardWindow) {
        mainStack->removeWidget(adminDashboardWindow);
        delete adminDashboardWindow;
    }

    adminDashboardWindow = new AdminDashboardWindow(admin, admins, profs, comptes, this);
    connect(adminDashboardWindow, &AdminDashboardWindow::logoutRequested, this, &MainWindow::onLogoutRequested);

    mainStack->addWidget(adminDashboardWindow);
    mainStack->setCurrentWidget(adminDashboardWindow);
}

void MainWindow::onLogoutRequested()
{
    if (studentDashboardWindow) {
        mainStack->removeWidget(studentDashboardWindow);
        delete studentDashboardWindow;
        studentDashboardWindow = nullptr;
    }
    if (teacherDashboardWindow) {
        mainStack->removeWidget(teacherDashboardWindow);
        delete teacherDashboardWindow;
        teacherDashboardWindow = nullptr;
    }
    if (adminDashboardWindow) {
        mainStack->removeWidget(adminDashboardWindow);
        delete adminDashboardWindow;
        adminDashboardWindow = nullptr;
    }
    mainStack->setCurrentWidget(landingWindow);
}
