#ifndef STATUTSALAIREPROFESSEUR_H
#define STATUTSALAIREPROFESSEUR_H

#include <QWidget>
#include "professeur.h"

class StatutSalaireProfesseur : public QWidget {
    Q_OBJECT

public:
    explicit StatutSalaireProfesseur(const Professeur &prof, QWidget *parent = nullptr);
    void updateProf(const Professeur &prof);

private:
    void setupUI(const Professeur &prof);
};

#endif // STATUTSALAIREPROFESSEUR_H
