#include "statutsalaireprofesseur.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

StatutSalaireProfesseur::StatutSalaireProfesseur(const Professeur &prof, QWidget *parent)
    : QWidget(parent)
{
    setupUI(prof);
}

void StatutSalaireProfesseur::updateProf(const Professeur &prof)
{
    qDeleteAll(children());
    setupUI(prof);
}

void StatutSalaireProfesseur::setupUI(const Professeur &prof)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(20);

    QFrame *card = new QFrame(this);
    card->setFixedSize(650, 380);
    card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 16px; border: 1px solid #e2e8f0; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 6);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 35, 40, 35);
    cardLayout->setSpacing(20);
    cardLayout->setAlignment(Qt::AlignCenter);

    bool paid = prof.getEstPaye();
    QString dateStr = QString::fromStdString(prof.getDatePaiement());

    // Round icon circle
    QLabel *iconCircle = new QLabel(paid ? "✅" : "⏳", card);
    iconCircle->setFixedSize(90, 90);
    iconCircle->setAlignment(Qt::AlignCenter);
    iconCircle->setStyleSheet(QString(
        "background-color: %1; "
        "border-radius: 45px; "
        "font-size: 42px; "
        "border: none;"
    ).arg(paid ? "#27ae60" : "#e67e22"));

    // Status Title
    QLabel *statusTitle = new QLabel(paid ? "Salaire reçu" : "Salaire non reçu", card);
    statusTitle->setAlignment(Qt::AlignCenter);
    statusTitle->setStyleSheet(QString(
        "font-size: 24px; font-weight: bold; color: %1; border: none;"
    ).arg(paid ? "#27ae60" : "#e67e22"));

    // Details Block
    QVBoxLayout *detailsLayout = new QVBoxLayout();
    detailsLayout->setSpacing(8);

    QLabel *monthLbl = new QLabel("Mois concerné : <b>Août 2026</b>", card);
    monthLbl->setAlignment(Qt::AlignCenter);
    monthLbl->setStyleSheet("font-size: 15px; color: #2d3748; border: none;");

    QLabel *montantLbl = new QLabel(paid ? (prof.getMontantSalaire() > 0 ? QString("Montant versé : <b style='color: #27ae60;'>%L1 HTG</b>").arg(prof.getMontantSalaire(), 0, 'f', 2) : "Montant versé : <b>Versé</b>")
                                         : "Montant : <i>En attente de versement</i>", card);
    montantLbl->setAlignment(Qt::AlignCenter);
    montantLbl->setStyleSheet("font-size: 15px; color: #2d3748; border: none;");

    QLabel *dateLbl = new QLabel(paid ? QString("Date de traitement : <b>%1</b>").arg(dateStr.isEmpty() ? "Traitée ce mois" : dateStr)
                                      : "Date de traitement : <i>En attente de traitement par le Secrétariat</i>", card);
    dateLbl->setAlignment(Qt::AlignCenter);
    dateLbl->setStyleSheet("font-size: 14px; color: #4a5568; border: none;");

    QLabel *noticeLbl = new QLabel("🔒 Statut en lecture seule — Mis à jour exclusivement par le Secrétariat de l'Administration.", card);
    noticeLbl->setAlignment(Qt::AlignCenter);
    noticeLbl->setStyleSheet("font-size: 12px; color: #a0aec0; font-style: italic; border: none; margin-top: 10px;");

    detailsLayout->addWidget(monthLbl);
    detailsLayout->addWidget(montantLbl);
    detailsLayout->addWidget(dateLbl);

    cardLayout->addWidget(iconCircle, 0, Qt::AlignCenter);
    cardLayout->addWidget(statusTitle, 0, Qt::AlignCenter);
    cardLayout->addLayout(detailsLayout);
    cardLayout->addWidget(noticeLbl, 0, Qt::AlignCenter);

    mainLayout->addWidget(card, 0, Qt::AlignCenter);
    mainLayout->addStretch();
}
