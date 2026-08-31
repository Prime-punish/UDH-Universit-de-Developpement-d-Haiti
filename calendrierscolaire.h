#ifndef CALENDRIERSCOLAIRE_H
#define CALENDRIERSCOLAIRE_H

#include <QWidget>
#include <QCalendarWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>

// ============================================================
//  Structure EvenementScolaire
// ============================================================
struct EvenementScolaire {
    std::string id;
    std::string titre;
    std::string date; // "dd/MM/yyyy"
    std::string description;
    std::string type; // "Examen", "Rentrée", "Vacances", "Autre"

    EvenementScolaire() = default;
    EvenementScolaire(const std::string &id, const std::string &titre, const std::string &date,
                      const std::string &description, const std::string &type);

    std::string toLigne() const;
    static EvenementScolaire fromLigne(const std::string &ligne);
};

// ============================================================
//  Persistance evenements.txt
// ============================================================
std::vector<EvenementScolaire> chargerEvenementsScolaires();
void sauvegarderEvenementsScolaires(const std::vector<EvenementScolaire> &events);
void ajouterEvenementScolaire(const EvenementScolaire &event);
bool supprimerEvenementScolaire(const std::string &idEvent);

// ============================================================
//  Widget CalendrierScolaire
// ============================================================
class CalendrierScolaire : public QWidget {
    Q_OBJECT

public:
    explicit CalendrierScolaire(bool allowManagement = false, QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onAddEventClicked();
    void onDeleteEventClicked(const QString &idEvent);
    void onCalendarDateSelected();

private:
    void setupUI();

    bool isManager;

    QCalendarWidget *calendarWidget;
    QTableWidget *eventsTable;

    // Form widgets (management mode)
    QLineEdit *titleEdit;
    QDateEdit *dateEdit;
    QComboBox *typeCombo;
    QTextEdit *descEdit;
    QLabel *selectedDateLabel;
};

#endif // CALENDRIERSCOLAIRE_H
