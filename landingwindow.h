#ifndef LANDINGWINDOW_H
#define LANDINGWINDOW_H

#include <QWidget>

class QComboBox;
class QLabel;

class LandingWindow : public QWidget
{
    Q_OBJECT

public:
    enum Role { RoleEtudiant = 0, RoleProfesseur = 1, RoleAdministrateur = 2 };

    explicit LandingWindow(QWidget *parent = nullptr);

signals:
    void roleSelected(int role);

private slots:
    void onRoleChosen(int index);

private:
    void setupUI();
    QComboBox *roleCombo;
};

#endif // LANDINGWINDOW_H
