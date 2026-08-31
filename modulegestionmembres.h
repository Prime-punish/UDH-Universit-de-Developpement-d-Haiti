#ifndef MODULEGESTIONMEMBRES_H
#define MODULEGESTIONMEMBRES_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <vector>
#include "administrateur.h"
#include "professeur.h"

class ModuleGestionMembres : public QWidget {
    Q_OBJECT

public:
    explicit ModuleGestionMembres(std::vector<Administrateur> &adminsRef,
                                  std::vector<Professeur> &profsRef,
                                  QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();
    void populateTable();
    void onActionClicked(const QString &personId, const QString &personType, const QString &action);

    std::vector<Administrateur> &admins;
    std::vector<Professeur> &profs;

    QTableWidget *table;
    QComboBox *filterCombo;
    QLineEdit *searchEdit;
};

#endif // MODULEGESTIONMEMBRES_H
