#ifndef ANNONCESRECUES_H
#define ANNONCESRECUES_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include "annonce.h"

class AnnoncesRecues : public QWidget {
    Q_OBJECT

public:
    explicit AnnoncesRecues(const std::string &roleCompte, const std::string &idCompte, QWidget *parent = nullptr);
    void refreshData();

signals:
    void lectureEffectuee();

private:
    void setupUI();
    QWidget* createAnnonceCard(const Annonce &annonce, bool nonLue);

    std::string roleCompte;
    std::string idCompte;

    QVBoxLayout *cardsContainerLayout;
    QLabel *unreadBadgeLabel;
    QLabel *countInfoLabel;
};

#endif // ANNONCESRECUES_H
