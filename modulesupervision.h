#ifndef MODULESUPERVISION_H
#define MODULESUPERVISION_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <vector>
#include <string>
#include "professeur.h"
#include "administrateur.h"
#include "etudiant.h"
#include "teacherdashboardwindow.h" // For StatCard

class ModuleSupervision : public QWidget {
    Q_OBJECT

public:
    explicit ModuleSupervision(QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();

    // Top StatCards (max 3)
    StatCard *cardStudents;
    StatCard *cardProfessors;
    StatCard *cardTotalEmployees;

    // Tables
    QTableWidget *subjectAveragesTable;
    QTableWidget *activityReportTable;
};

#endif // MODULESUPERVISION_H
