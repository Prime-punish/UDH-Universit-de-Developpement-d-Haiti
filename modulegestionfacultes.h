#ifndef MODULEGESTIONFACULTES_H
#define MODULEGESTIONFACULTES_H

#include <QWidget>
#include <QTableWidget>
#include <vector>
#include "administrateur.h"
#include "professeur.h"
#include "etudiant.h"

class ModuleGestionFacultes : public QWidget {
    Q_OBJECT

public:
    explicit ModuleGestionFacultes(std::vector<Administrateur> &adminsRef,
                                   std::vector<Professeur> &profsRef,
                                   std::vector<CompteEtudiant> &comptesRef,
                                   QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();
    void populateTable();

    std::vector<Administrateur> &admins;
    std::vector<Professeur> &profs;
    std::vector<CompteEtudiant> &comptes;

    QTableWidget *table;
};

#endif // MODULEGESTIONFACULTES_H
