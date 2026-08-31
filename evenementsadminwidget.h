#ifndef EVENEMENTSADMINWIDGET_H
#define EVENEMENTSADMINWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <string>
#include "calendrierscolaire.h"
#include "gestionreunions.h"

class EvenementsAdminWidget : public QWidget {
    Q_OBJECT

public:
    explicit EvenementsAdminWidget(const std::string &posteAdmin, const std::string &faculteAdmin = "", QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();

    std::string poste;
    std::string faculte;

    QTabWidget *tabWidget;
    CalendrierScolaire *calendrierWidget;
    ReunionsAdminView *reunionsWidget;
};

#endif // EVENEMENTSADMINWIDGET_H
