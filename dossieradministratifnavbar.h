#ifndef DOSSIERADMINISTRATIFNAVBAR_H
#define DOSSIERADMINISTRATIFNAVBAR_H

#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <vector>
#include "etudiant.h"
#include "professeur.h"
#include "paiementversement.h"
#include "modulesupport.h"
#include "modulerapportgraphique.h"

class DossierAdministratifNavBar : public QWidget {
    Q_OBJECT

public:
    explicit DossierAdministratifNavBar(std::vector<CompteEtudiant> &comptesRef,
                                        std::vector<Professeur> &profsRef,
                                        QWidget *parent = nullptr);
    explicit DossierAdministratifNavBar(std::vector<CompteEtudiant> &comptesRef,
                                        QWidget *parent = nullptr);

    void refreshData();

private slots:
    void onExamenSearchChanged();
    void onExamenFaculteChanged();
    void onOuvrirFichierExamen(const QString &chemin);

private:
    void setupUI();
    QWidget* createExamensPage();
    void refreshExamensData();

    std::vector<CompteEtudiant> &comptes;
    std::vector<Professeur> *profs;
    std::vector<Professeur> localProfs;

    QTabWidget *tabWidget;
    PaiementVersement *paiementWidget;
    QWidget *examensWidget;
    ModuleSupport *supportWidget;
    ModuleRapportGraphique *rapportWidget;

    // Examens UI elements
    QLabel *lblTotalExamens;
    QLabel *lblTotalFacultes;
    QLabel *lblTotalProfs;
    QLineEdit *examensSearchEdit;
    QComboBox *examensFaculteCombo;
    QTableWidget *examensTable;
};

#endif // DOSSIERADMINISTRATIFNAVBAR_H
