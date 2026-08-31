#ifndef MODULEVUEENSEMBLE_H
#define MODULEVUEENSEMBLE_H

#include <QWidget>
#include <QLabel>
#include <vector>
#include "administrateur.h"
#include "professeur.h"
#include "etudiant.h"

class ModuleVueEnsemble : public QWidget {
    Q_OBJECT

public:
    explicit ModuleVueEnsemble(std::vector<Administrateur> &adminsRef,
                               std::vector<Professeur> &profsRef,
                               std::vector<CompteEtudiant> &comptesRef,
                               QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();
    void updateStats();

    std::vector<Administrateur> &admins;
    std::vector<Professeur> &profs;
    std::vector<CompteEtudiant> &comptes;

    QLabel *lblTotalAdmins;
    QLabel *lblTotalProfs;
    QLabel *lblTotalEtudiants;
    QLabel *lblProfsValides;
    QLabel *lblProfsPayes;
    QLabel *lblAdminsActifs;
};

#endif // MODULEVUEENSEMBLE_H
