#ifndef MODULERAPPORTGRAPHIQUE_H
#define MODULERAPPORTGRAPHIQUE_H

#include <QWidget>
#include <QScrollArea>
#include <vector>
#include <string>
#include "professeur.h"
#include "administrateur.h"
#include "etudiant.h"
#include "teacherdashboardwindow.h" // For StatCard

// Forward declarations for QtCharts
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

class ModuleRapportGraphique : public QWidget {
    Q_OBJECT

public:
    explicit ModuleRapportGraphique(QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();

    // Chart views
    QChartView *profsChartView;
    QChartView *adminsChartView;
    QChartView *studentsChartView;

    // Stat Cards
    StatCard *cardTotalStudents;
    StatCard *cardTotalProfs;
    StatCard *cardSalariesPaid;
};

#endif // MODULERAPPORTGRAPHIQUE_H
