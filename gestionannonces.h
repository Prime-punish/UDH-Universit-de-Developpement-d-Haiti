#ifndef GESTIONANNONCES_H
#define GESTIONANNONCES_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "annonce.h"
#include "teacherdashboardwindow.h" // For StatCard

class GestionAnnonces : public QWidget {
    Q_OBJECT

public:
    explicit GestionAnnonces(QWidget *parent = nullptr);
    void refreshData();

signals:
    void annoncesUpdated();

private slots:
    void onPublishClicked();
    void onTousToggled(bool checked);

private:
    void setupUI();

    // 3 StatCards
    StatCard *cardTotal;
    StatCard *cardMonth;
    StatCard *cardAudience;

    // Form inputs
    QLineEdit *titleEdit;
    QTextEdit *contentEdit;
    QCheckBox *cbEtudiants;
    QCheckBox *cbProfesseurs;
    QCheckBox *cbAdmins;
    QCheckBox *cbTous;
    QLabel *dateLabel;

    // Table
    QTableWidget *annoncesTable;
};

#endif // GESTIONANNONCES_H
