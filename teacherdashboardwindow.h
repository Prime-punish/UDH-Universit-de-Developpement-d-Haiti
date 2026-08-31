#ifndef TEACHERDASHBOARDWINDOW_H
#define TEACHERDASHBOARDWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "professeur.h"
#include "parametrescomptebase.h"

class StatutSalaireProfesseur;
class AnnoncesRecues;
class CalendrierScolaire;
class ModuleSupport;

// ============================================================
//  StatCard Widget Custom (Card statistique réutilisable)
// ============================================================
class StatCard : public QFrame {
    Q_OBJECT
public:
    explicit StatCard(const QString &icon, const QString &value, const QString &label, const QString &circleColor, QWidget *parent = nullptr);
    void updateValue(const QString &newValue);

private:
    QLabel *valLabel;
};

// ============================================================
//  TeacherDashboardWindow / DashboardProfesseur
// ============================================================
class TeacherDashboardWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TeacherDashboardWindow(Professeur prof, std::vector<Professeur> &profsRef, QWidget *parent = nullptr);
    ~TeacherDashboardWindow() override;

signals:
    void logoutRequested();

private slots:
    void onSidebarBtnClicked(int index);
    void onSaveNoteClicked();
    void onUploadPhotoClicked();
    void onSaveSettingsClicked();

private:
    void setupUI();
    QWidget* createSidebar();
    QWidget* createDashboardPage();
    QWidget* createProfilePage();
    QWidget* createSubjectsPage();
    QWidget* createTimetablePage();
    QWidget* createAssignGradePage();
    QWidget* createViewGradesPage();
    QWidget* createUploadPhotoPage();
    QWidget* createViewPhotosPage();
    QWidget* createSalaryPage();
    QWidget* createAnnoncesPage();
    QWidget* createEventsPage();
    QWidget* createSupportPage();
    QWidget* createSettingsPage();

    void refreshGradesTable();
    void refreshPhotosTable();
    void refreshSalaryPage();
    void updateAnnoncesBadge();

    Professeur currentProf;
    std::vector<Professeur> &profs;

    QStackedWidget *stackedWidget;
    QButtonGroup *sidebarBtnGroup;

    // Stat Cards
    StatCard *cardSubjects;
    StatCard *cardGrades;
    StatCard *cardPhotos;

    // Salary widget
    StatutSalaireProfesseur *salaryWidget;

    // Annonces widget
    AnnoncesRecues *annoncesRecuesWidget;

    // Events & Support widgets
    CalendrierScolaire *calendrierWidget;
    ModuleSupport *supportWidget;

    // Dashboard table
    QTableWidget *dashGradesTable;
    QTableWidget *viewGradesTable;
    QTableWidget *viewPhotosTable;

    // Assign Grade Form
    QComboBox *gradeSubjectCombo;
    QLineEdit *gradeStudentIdEdit;
    QLineEdit *gradeValueEdit;

    // Upload Photo Form
    QComboBox *photoSubjectCombo;
    QLineEdit *photoFilePathEdit;

    // Settings Form
    QLineEdit *settPhoneEdit;
    QLineEdit *settEmailEdit;
    QLineEdit *settCurrentPwdEdit;
    QLineEdit *settNewPwdEdit;
};

#endif // TEACHERDASHBOARDWINDOW_H
