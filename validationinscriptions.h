#ifndef VALIDATIONINSCRIPTIONS_H
#define VALIDATIONINSCRIPTIONS_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <vector>
#include "etudiant.h"
#include "teacherdashboardwindow.h" // For StatCard

class ValidationInscriptions : public QWidget {
    Q_OBJECT

public:
    explicit ValidationInscriptions(std::vector<CompteEtudiant> &comptesRef, QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onFilterChanged(int index);
    void onViewDossier(int rowIndex);

private:
    void setupUI();
    void populateTable();

    std::vector<CompteEtudiant> &comptes;

    StatCard *cardPending;
    StatCard *cardApproved;
    StatCard *cardRejected;

    QComboBox *filterCombo;
    QTableWidget *requestsTable;

    struct TableRowMapping {
        int originalIndex;
    };
    std::vector<TableRowMapping> rowMappings;
};

#endif // VALIDATIONINSCRIPTIONS_H
