#ifndef GESTIONSALAIRES_H
#define GESTIONSALAIRES_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "professeur.h"
#include "administrateur.h"
#include "teacherdashboardwindow.h" // For StatCard

class GestionSalaires : public QWidget {
    Q_OBJECT

public:
    explicit GestionSalaires(std::vector<Professeur> &profsRef, std::vector<Administrateur> &adminsRef, QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onFilterChanged(int index);
    void onMarkAllPaidClicked();
    void onViewJournalTresorerieClicked();

private:
    void setupUI();
    void populateTable();

    std::vector<Professeur> &profs;
    std::vector<Administrateur> &admins;

    StatCard *cardPaid;
    StatCard *cardUnpaid;
    StatCard *cardTotal;
    StatCard *cardSolde;

    QComboBox *filterCombo;
    QTableWidget *salaryTable;
};

#endif // GESTIONSALAIRES_H
