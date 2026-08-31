#ifndef GESTIONREUNIONS_H
#define GESTIONREUNIONS_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <string>
#include <vector>

// ============================================================
//  Structure ReunionComite
// ============================================================
struct ReunionComite {
    std::string id;
    std::string titre;
    std::string date;  // "dd/MM/yyyy"
    std::string heure; // "HH:mm"
    std::string lieu;
    std::string description;
    std::vector<std::string> destinataires; // "Recteur", "Président", "Secrétaire", "Coordonnateur - Faculté..."
    std::string statut; // "Convoquée", "Tenue", "Annulée"

    ReunionComite() = default;
    ReunionComite(const std::string &id, const std::string &titre, const std::string &date,
                  const std::string &heure, const std::string &lieu, const std::string &desc,
                  const std::vector<std::string> &destinataires, const std::string &statut = "Convoquée");

    bool estDestinataire(const std::string &roleOuPoste, const std::string &faculte = "") const;
    std::string destinatairesChaine() const;

    std::string toLigne() const;
    static ReunionComite fromLigne(const std::string &ligne);
};

// ============================================================
//  Persistance reunions.txt
// ============================================================
std::vector<ReunionComite> chargerReunionsComite();
void sauvegarderReunionsComite(const std::vector<ReunionComite> &reunions);
void ajouterReunionComite(const ReunionComite &reunion);
bool supprimerReunionComite(const std::string &idReunion);

// ============================================================
//  Widget GestionReunions (Mode Secrétaire: création et gestion)
// ============================================================
class GestionReunions : public QWidget {
    Q_OBJECT

public:
    explicit GestionReunions(QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onSendConvocationClicked();
    void onDeleteReunionClicked(const QString &idReunion);
    void onViewDetailsClicked(const QString &idReunion);

private:
    void setupUI();

    QLineEdit *titleEdit;
    QDateEdit *dateEdit;
    QTimeEdit *timeEdit;
    QLineEdit *placeEdit;
    QTextEdit *descEdit;

    std::vector<QCheckBox*> recipientBoxes;
    QTableWidget *reunionsTable;
};

// ============================================================
//  Widget ReunionsAdminView (Mode Consultation Admin: filtré)
// ============================================================
class ReunionsAdminView : public QWidget {
    Q_OBJECT

public:
    explicit ReunionsAdminView(const std::string &posteAdmin, const std::string &faculteAdmin = "", QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onViewDetailsClicked(const QString &idReunion);

private:
    void setupUI();

    std::string poste;
    std::string faculte;
    QTableWidget *reunionsTable;
    QLabel *totalLabel;
};

#endif // GESTIONREUNIONS_H
