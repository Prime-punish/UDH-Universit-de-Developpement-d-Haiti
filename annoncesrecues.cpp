#include "annoncesrecues.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <algorithm>

AnnoncesRecues::AnnoncesRecues(const std::string &roleCompte, const std::string &idCompte, QWidget *parent)
    : QWidget(parent), roleCompte(roleCompte), idCompte(idCompte),
      cardsContainerLayout(nullptr), unreadBadgeLabel(nullptr), countInfoLabel(nullptr)
{
    setupUI();
}

void AnnoncesRecues::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *content = new QWidget();
    content->setStyleSheet("background-color: #f0f2f5;");
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(20);

    // Top Header Row
    QHBoxLayout *headerRow = new QHBoxLayout();

    QLabel *title = new QLabel("📢 Annonces & Communications Officielles", content);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0b1e36; background: transparent; border: none;");

    unreadBadgeLabel = new QLabel(content);
    unreadBadgeLabel->setStyleSheet(
        "background-color: #e53e3e; color: white; border-radius: 12px; "
        "padding: 4px 12px; font-size: 12px; font-weight: bold; border: none;"
    );

    countInfoLabel = new QLabel(content);
    countInfoLabel->setStyleSheet("font-size: 13px; color: #718096; background: transparent; border: none;");

    QPushButton *markAllBtn = new QPushButton("✓ Tout marquer comme lu", content);
    markAllBtn->setCursor(Qt::PointingHandCursor);
    markAllBtn->setStyleSheet(
        "QPushButton { background-color: #edf2f7; color: #2b6cb0; border-radius: 6px; padding: 6px 14px; font-weight: bold; font-size: 12px; border: 1px solid #cbd5e0; }"
        "QPushButton:hover { background-color: #e2e8f0; color: #1a4971; }"
    );
    connect(markAllBtn, &QPushButton::clicked, [this]() {
        auto annonces = chargerAnnonces();
        for (const auto &a : annonces) {
            if (a.estConcerne(roleCompte) && !a.estVuPar(idCompte)) {
                marquerAnnonceLue(a.id, idCompte);
            }
        }
        refreshData();
        emit lectureEffectuee();
    });

    headerRow->addWidget(title);
    headerRow->addWidget(unreadBadgeLabel);
    headerRow->addSpacing(10);
    headerRow->addWidget(countInfoLabel);
    headerRow->addStretch();
    headerRow->addWidget(markAllBtn);

    mainLayout->addLayout(headerRow);

    // Container for dynamic cards
    QWidget *cardsContainer = new QWidget(content);
    cardsContainer->setStyleSheet("background: transparent; border: none;");
    cardsContainerLayout = new QVBoxLayout(cardsContainer);
    cardsContainerLayout->setContentsMargins(0, 0, 0, 0);
    cardsContainerLayout->setSpacing(16);

    mainLayout->addWidget(cardsContainer);
    mainLayout->addStretch();

    scroll->setWidget(content);
    rootLayout->addWidget(scroll);

    refreshData();
}

QWidget* AnnoncesRecues::createAnnonceCard(const Annonce &annonce, bool nonLue)
{
    QFrame *card = new QFrame();
    card->setStyleSheet(QString(
        "QFrame { background-color: #ffffff; border-radius: 12px; border: %1; }"
    ).arg(nonLue ? "2px solid #3182ce" : "1px solid #e2e8f0"));

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(14);
    shadow->setColor(QColor(0, 0, 0, nonLue ? 25 : 12));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(25, 20, 25, 22);
    cardLayout->setSpacing(12);

    // Top Header: Badge Status + Title + Date
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setSpacing(10);

    QLabel *badge = new QLabel(nonLue ? "🆕 NOUVEAU" : "✓ Lu", card);
    badge->setStyleSheet(QString(
        "background-color: %1; color: %2; border-radius: 6px; "
        "font-size: 11px; font-weight: bold; padding: 4px 8px; border: none;"
    ).arg(nonLue ? "#e53e3e" : "#edf2f7").arg(nonLue ? "#ffffff" : "#718096"));

    QLabel *cardTitle = new QLabel(QString::fromStdString(annonce.titre), card);
    cardTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36; background: transparent; border: none;");
    cardTitle->setWordWrap(true);

    QLabel *dateLbl = new QLabel(QString("📅 Publiée le %1").arg(QString::fromStdString(annonce.dateEnvoi)), card);
    dateLbl->setStyleSheet("font-size: 12px; color: #718096; background: transparent; border: none;");

    topRow->addWidget(badge);
    topRow->addWidget(cardTitle, 1);
    topRow->addWidget(dateLbl);
    cardLayout->addLayout(topRow);

    // Divider
    QFrame *line = new QFrame(card);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #edf2f7; background-color: #edf2f7; height: 1px; border: none;");
    cardLayout->addWidget(line);

    // Message Content
    QLabel *contentLbl = new QLabel(QString::fromStdString(annonce.contenu), card);
    contentLbl->setStyleSheet("font-size: 14px; color: #2d3748; line-height: 1.5; background: transparent; border: none;");
    contentLbl->setWordWrap(true);
    contentLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    cardLayout->addWidget(contentLbl);

    // Footer: Sender signature + Target audience badge
    QHBoxLayout *footerRow = new QHBoxLayout();
    footerRow->setSpacing(10);

    QLabel *senderLbl = new QLabel("🏛️ Direction de l'Administration & Secrétariat Général (UDH)", card);
    senderLbl->setStyleSheet("font-size: 12px; font-style: italic; color: #718096; background: transparent; border: none;");

    QLabel *destBadge = new QLabel(QString("👥 Destiné à : %1").arg(QString::fromStdString(annonce.destinatairesChaine())), card);
    destBadge->setStyleSheet("background-color: #ebf8ff; color: #2b6cb0; border-radius: 4px; padding: 3px 8px; font-size: 11px; font-weight: bold; border: none;");

    footerRow->addWidget(senderLbl);
    footerRow->addStretch();
    footerRow->addWidget(destBadge);

    cardLayout->addLayout(footerRow);

    // If unread, mark read when opened/clicked
    if (nonLue) {
        marquerAnnonceLue(annonce.id, idCompte);
    }

    return card;
}

void AnnoncesRecues::refreshData()
{
    if (!cardsContainerLayout) return;

    // Clear old cards
    while (cardsContainerLayout->count() > 0) {
        QLayoutItem *item = cardsContainerLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    auto annonces = chargerAnnonces();

    // Sort newest first
    std::reverse(annonces.begin(), annonces.end());

    int unreadCount = 0;
    int relevantCount = 0;

    for (const auto &a : annonces) {
        if (a.estConcerne(roleCompte)) {
            relevantCount++;
            bool nonLue = !a.estVuPar(idCompte);
            if (nonLue) {
                unreadCount++;
            }
            cardsContainerLayout->addWidget(createAnnonceCard(a, nonLue));
        }
    }

    if (unreadBadgeLabel) {
        if (unreadCount > 0) {
            unreadBadgeLabel->setText(QString("%1 non lue(s)").arg(unreadCount));
            unreadBadgeLabel->show();
        } else {
            unreadBadgeLabel->hide();
        }
    }

    if (countInfoLabel) {
        countInfoLabel->setText(QString("(%1 annonce%2 au total)").arg(relevantCount).arg(relevantCount > 1 ? "s" : ""));
    }

    if (relevantCount == 0) {
        QFrame *emptyCard = new QFrame();
        emptyCard->setStyleSheet("background-color: #ffffff; border-radius: 12px; border: 1.5px dashed #cbd5e0;");
        QVBoxLayout *eLayout = new QVBoxLayout(emptyCard);
        eLayout->setContentsMargins(40, 50, 40, 50);
        eLayout->setAlignment(Qt::AlignCenter);
        eLayout->setSpacing(10);

        QLabel *icon = new QLabel("📭", emptyCard);
        icon->setStyleSheet("font-size: 44px; border: none; background: transparent;");
        icon->setAlignment(Qt::AlignCenter);

        QLabel *msg = new QLabel("Aucune annonce pour le moment", emptyCard);
        msg->setStyleSheet("font-size: 16px; font-weight: bold; color: #4a5568; border: none; background: transparent;");
        msg->setAlignment(Qt::AlignCenter);

        QLabel *sub = new QLabel("Les communiqués et informations institutionnelles transmis par le Secrétariat Général s'afficheront ici en temps réel.", emptyCard);
        sub->setStyleSheet("font-size: 13px; color: #a0aec0; border: none; background: transparent;");
        sub->setAlignment(Qt::AlignCenter);

        eLayout->addWidget(icon);
        eLayout->addWidget(msg);
        eLayout->addWidget(sub);

        cardsContainerLayout->addWidget(emptyCard);
    }
}
