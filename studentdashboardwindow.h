#ifndef STUDENTDASHBOARDWINDOW_H
#define STUDENTDASHBOARDWINDOW_H

#include <QWidget>
#include <map>
#include <vector>
#include <string>
#include "etudiant.h"
#include "calendrierscolaire.h"
#include "paiementversement.h"
#include "modulesupport.h"
#include "parametrescomptebase.h"

class QStackedWidget;
class QLabel;
class QVBoxLayout;
class QCalendarWidget;
class QButtonGroup;

class StudentDashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StudentDashboardWindow(CompteEtudiant compte, 
                                    const std::map<std::string, ProgrammeEtudes>& progs,
                                    std::vector<CompteEtudiant>& comptesRef,
                                    QWidget *parent = nullptr);
    ~StudentDashboardWindow();

signals:
    void logoutRequested();

private slots:
    void onSidebarBtnClicked(int index);
    void updateProfileInfo();
    void onCompleteRegistrationClicked();
    void reloadAllPages();

private:
    void setupUI();
    QWidget* createSidebar();
    
    // Page creation methods
    QWidget* createDashboardPage();
    QWidget* createProfilePage();
    QWidget* createCoursesPage();
    QWidget* createResultsPage();
    QWidget* createTimetablePage();
    QWidget* createNoticeBoardPage();
    QWidget* createAcademicCalendarPage();
    QWidget* createPaymentPage();
    QWidget* createDownloadsPage();
    QWidget* createSupportPage();
    QWidget* createSettingsPage();
    QWidget* createUnregisteredNoticeCard();
    
    // Dashboard section helpers
    QWidget* createStatCard(const QString& icon, const QString& value, const QString& label, const QString& sublabel, const QString& accentColor);
    QWidget* createRecentResultsSection();
    QWidget* createNoticeBoardSection();
    QWidget* createCalendarSection();
    QWidget* createQuickAccessSection();
    void updateAnnoncesBadge();

    CompteEtudiant currentCompte;
    const std::map<std::string, ProgrammeEtudes>& programmes;
    std::vector<CompteEtudiant>& comptes;
    
    QStackedWidget *stackedWidget;
    QButtonGroup *sidebarBtnGroup;
    
    // Dynamic labels
    QLabel *greetingLabel;
    QLabel *statusLabel;

    CalendrierScolaire *calendrierWidget;
    PaiementVersement *paiementWidget;
    ModuleSupport *supportWidget;
};

#endif // STUDENTDASHBOARDWINDOW_H
