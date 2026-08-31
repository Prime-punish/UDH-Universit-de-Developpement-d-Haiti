#ifndef ADMINDASHBOARDWINDOW_H
#define ADMINDASHBOARDWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <vector>
#include "administrateur.h"
#include "professeur.h"
#include "etudiant.h"
#include "gestionsalaires.h"
#include "validationinscriptions.h"
#include "gestionannonces.h"
#include "annoncesrecues.h"
#include "teacherdashboardwindow.h" // For StatCard
#include "gestioncourrier.h"
#include "calendrierscolaire.h"
#include "gestionreunions.h"
#include "dossieradministratifnavbar.h"
#include "modulesupervision.h"
#include "modulerapportgraphique.h"
#include "evenementsadminwidget.h"
#include "modulegestionmembres.h"
#include "modulevueensemble.h"
#include "modulevalidationacademique.h"
#include "modulegestionfacultes.h"
#include "isolationfaculteutils.h"
#include "parametrescomptebase.h"

class AdminDashboardWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminDashboardWindow(Administrateur admin, std::vector<Administrateur> &adminsRef,
                                 std::vector<Professeur> &profsRef, std::vector<CompteEtudiant> &comptesRef,
                                 QWidget *parent = nullptr);
    ~AdminDashboardWindow() override;

signals:
    void logoutRequested();

private slots:
    void onSidebarBtnClicked(int index);

private:
    void setupUI();
    QWidget* createSidebar();
    QWidget* createGenericPage(const QString &titleText, const QString &descriptionText, const QString &icon = "📋");
    QWidget* createProfilePage();
    QWidget* createSettingsPage();
    QWidget* createSalaryManagementPage();
    QWidget* createValidationInscriptionsPage();
    QWidget* createSecretaryNotesTransferPage();
    QWidget* createSecretaryAnnoncesPage();
    QWidget* createAnnoncesRecuesPage();

    // Coordonnateur Modules (Isolation stricte par faculté assignée)
    QWidget* createCoordMaFacultePage();
    QWidget* createCoordProfsPage();
    QWidget* createCoordProgrammesPage();
    QWidget* createCoordEmploiDuTempsPage();
    QWidget* createCoordResultsPage();

    void refreshCoordMaFaculte();
    void refreshCoordProfs();
    void refreshCoordProgrammes();
    void refreshCoordEmploiDuTemps();
    void refreshCoordResultsTable();

    // Secrétaire, Président & Recteur modules
    QWidget* createSecretaryCourrierPage();
    QWidget* createSecretaryRdvPage();
    QWidget* createSecretaryDossiersPage();
    QWidget* createPresidentConseilPage();
    QWidget* createPresidentFinancePage();
    QWidget* createRecteurVueEnsemblePage();
    QWidget* createRecteurGestionMembresPage();
    QWidget* createRecteurValidationAcademiquePage();
    QWidget* createRecteurGestionFacultesPage();
    QWidget* createRecteurFinancePage();
    QWidget* createAdminEventsPage();
    QWidget* createSupervisionPage();
    QWidget* createRapportGraphiquePage();

    void refreshSecretaryNotesTable();
    void updateAnnoncesBadge();

    Administrateur currentAdmin;
    std::vector<Administrateur> &admins;
    std::vector<Professeur> &profs;
    std::vector<CompteEtudiant> &comptes;

    // Faculté assignée au coordonnateur connecté (immuable, spécifique à cette instance)
    QString m_faculteAssignee;

    QStackedWidget *stackedWidget;
    QButtonGroup *sidebarBtnGroup;
    GestionSalaires *gestionSalairesWidget;
    ValidationInscriptions *validationInscriptionsWidget;
    GestionAnnonces *gestionAnnoncesWidget;
    AnnoncesRecues *annoncesRecuesWidget;

    // New widgets
    GestionCourrier *gestionCourrierWidget;
    QWidget *secretaryRdvWidget;
    CalendrierScolaire *secCalWidget;
    GestionReunions *secReunionsWidget;
    DossierAdministratifNavBar *dossierNavBarWidget;
    CourriersPresidentView *presidentConseilWidget;
    CourriersRecteurView *presidentFinanceWidget;
    CourriersRecteurView *recteurFinanceWidget;
    EvenementsAdminWidget *adminEventsWidget;
    ModuleSupervision *supervisionWidget;
    ModuleRapportGraphique *rapportGraphiqueWidget;

    // Recteur modules
    ModuleVueEnsemble *recteurVueEnsembleWidget;
    ModuleGestionMembres *recteurGestionMembresWidget;
    ModuleValidationAcademique *recteurValidationWidget;
    ModuleGestionFacultes *recteurGestionFacultesWidget;

    // Secrétaire Notes Transfer widgets
    StatCard *cardSecNotesPending;
    StatCard *cardSecNotesTransferred;
    StatCard *cardSecNotesTotal;
    QTableWidget *secNotesTable;
    QComboBox *secNotesFilterCombo;

    // Coordonnateur UI widgets
    // 1. Ma Faculté
    StatCard *cardCoordFacProfs;
    StatCard *cardCoordFacStudents;
    StatCard *cardCoordFacYears;
    StatCard *cardCoordFacCourses;
    QTableWidget *coordFacStudentsTable;
    QLineEdit *coordFacStudentSearch;

    // 2. Gestion Professeurs
    StatCard *cardCoordProfsTotal;
    StatCard *cardCoordProfsPaid;
    StatCard *cardCoordProfsCourses;
    QTableWidget *coordProfsTable;
    QLineEdit *coordProfsSearch;

    // 3. Programmes & Matières
    QComboBox *coordProgramYearCombo;
    QTableWidget *coordProgramTable;
    QLabel *coordProgramSummaryLbl;

    // 4. Emploi du temps
    QComboBox *coordEdtYearCombo;
    QTableWidget *coordEdtTable;

    // 5. Résultats Coordonnateur
    StatCard *cardCoordNotesCount;
    StatCard *cardCoordNotesAverage;
    StatCard *cardCoordStudentsCount;
    QTableWidget *coordResultsTable;
    QLineEdit *coordResultsSearch;
};

#endif // ADMINDASHBOARDWINDOW_H
