#ifndef MODULESUPPORT_H
#define MODULESUPPORT_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>

// ============================================================
//  Structure TicketSupport
// ============================================================
struct TicketSupport {
    std::string id;
    std::string expediteurId;
    std::string nomExpediteur;
    std::string typeCompte; // "Étudiant", "Professeur"
    std::string sujet;
    std::string message;
    std::string date;
    std::string statut; // "Non traité", "En cours", "Résolu"

    TicketSupport() = default;
    TicketSupport(const std::string &id, const std::string &expId, const std::string &nomExp,
                  const std::string &typeC, const std::string &suj, const std::string &msg,
                  const std::string &dt, const std::string &st = "Non traité");

    std::string toLigne() const;
    static TicketSupport fromLigne(const std::string &ligne);
};

// ============================================================
//  Persistance support.txt
// ============================================================
std::vector<TicketSupport> chargerTicketsSupport();
void sauvegarderTicketsSupport(const std::vector<TicketSupport> &tickets);
void ajouterTicketSupport(const TicketSupport &ticket);
bool mettreAJourStatutTicket(const std::string &idTicket, const std::string &nouveauStatut);

// ============================================================
//  Widget ModuleSupport
// ============================================================
class ModuleSupport : public QWidget {
    Q_OBJECT

public:
    enum Mode { UserMode, AdminMode };

    explicit ModuleSupport(Mode mode, const std::string &userId = "", const std::string &userName = "",
                          const std::string &userType = "", QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onSendTicketClicked();
    void onFilterChanged();
    void onChangeStatusClicked(const QString &ticketId, const QString &currentStatus);
    void onViewMessageClicked(const QString &ticketId);

private:
    void setupUserUI();
    void setupAdminUI();

    Mode currentMode;
    std::string currentUserId;
    std::string currentUserName;
    std::string currentUserType;

    // User mode widgets
    QLineEdit *subjectEdit;
    QTextEdit *messageEdit;
    QTableWidget *myTicketsTable;

    // Admin mode widgets
    QTableWidget *adminTicketsTable;
    QComboBox *statusFilterCombo;
    QComboBox *roleFilterCombo;
    QLabel *totalTicketsLabel;
    QLabel *pendingTicketsLabel;
    QLabel *resolvedTicketsLabel;
};

#endif // MODULESUPPORT_H
