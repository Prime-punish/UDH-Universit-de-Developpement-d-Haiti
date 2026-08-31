#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>
#include "etudiant.h"

class QLineEdit;
class QComboBox;
class QLabel;
class QCheckBox;
class QPushButton;

class RegistrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    Inscription getInscriptionData() const;

private slots:
    void onBrowseId();
    void onBrowsePhoto();
    void onBrowsePayment();
    void onValidate();

private:
    void setupUI();

    QLineEdit *nomProcheEdit;
    QLineEdit *prenomProcheEdit;
    QLineEdit *telProcheEdit;
    QComboBox *faculteCombo;

    QLabel *idPathLabel;
    QLabel *photoPathLabel;
    QLabel *paymentPathLabel;
    QCheckBox *paidCheckBox;
    QPushButton *browsePaymentBtn;

    QString idPath;
    QString photoPath;
    QString paymentPath;

    Inscription resultInscription;
};

#endif // REGISTRATIONDIALOG_H
