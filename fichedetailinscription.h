#ifndef FICHEDETAILINSCRIPTION_H
#define FICHEDETAILINSCRIPTION_H

#include <QDialog>
#include <vector>
#include "etudiant.h"

class FicheDetailInscription : public QDialog {
    Q_OBJECT

public:
    explicit FicheDetailInscription(CompteEtudiant &compteRef, std::vector<CompteEtudiant> &comptesRef, QWidget *parent = nullptr);

private slots:
    void onApproveClicked();
    void onRejectClicked();

private:
    void setupUI();

    CompteEtudiant &compte;
    std::vector<CompteEtudiant> &comptes;
};

#endif // FICHEDETAILINSCRIPTION_H
