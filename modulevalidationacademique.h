#ifndef MODULEVALIDATIONACADEMIQUE_H
#define MODULEVALIDATIONACADEMIQUE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <vector>

class ModuleValidationAcademique : public QWidget {
    Q_OBJECT

public:
    explicit ModuleValidationAcademique(QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();
    void loadDecisions();
    void saveDecisions();
    void addDecision();

    QTableWidget *table;
    QLineEdit *sujetEdit;
    QTextEdit *descEdit;
    QComboBox *typeCombo;
};

#endif // MODULEVALIDATIONACADEMIQUE_H
