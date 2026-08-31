#include "landingwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QAbstractItemView>
#include <QPainter>

LandingWindow::LandingWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void LandingWindow::setupUI()
{
    // Outer white background
    setStyleSheet("background-color: #ffffff; font-family: 'Segoe UI', sans-serif;");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(40, 40, 40, 40);
    rootLayout->setAlignment(Qt::AlignCenter);

    // Inner Dark Navy Card (#14166B / #16176E)
    QWidget *card = new QWidget(this);
    card->setFixedSize(850, 520);
    card->setStyleSheet(
        "QWidget#landingCard {"
        "  background-color: #14166B;"
        "  border-radius: 18px;"
        "}"
    );
    card->setObjectName("landingCard");

    // Soft drop shadow on card
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(35);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 8);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(60, 45, 60, 50);
    cardLayout->setSpacing(20);
    cardLayout->setAlignment(Qt::AlignCenter);

    // 1. University Crest/Logo at Top
    QLabel *logoLabel = new QLabel(card);
    QPixmap logoPix(":/resources/logo.png");
    if (!logoPix.isNull()) {
        logoLabel->setPixmap(logoPix.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("background: transparent; border: none;");

    // 2. University Name (White, Bold, Italic)
    QLabel *titleLabel = new QLabel("Université de Développement d'Haïti", card);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 26px; "
        "font-weight: bold; "
        "font-style: italic; "
        "color: #ffffff; "
        "background: transparent; "
        "border: none; "
        "letter-spacing: 0.5px;"
    );

    // 3. Dropdown Role Selector
    roleCombo = new QComboBox(card);
    roleCombo->setFixedSize(500, 52);
    roleCombo->setCursor(Qt::PointingHandCursor);

    // Add items (0: placeholder, 1: Étudiant, 2: Professeur, 3: Administrateur)
    roleCombo->addItem("Connectez vous en tant que");
    roleCombo->addItem("🎓   Étudiant");
    roleCombo->addItem("👨‍🏫   Professeur");
    roleCombo->addItem("🛡️   Administrateur");

    // Style the QComboBox to match the mockup exactly:
    // White background, rounded corners, navy bold italic text, custom navy triangle arrow
    roleCombo->setStyleSheet(
        "QComboBox {"
        "  background-color: #ffffff;"
        "  color: #14166B;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  font-style: italic;"
        "  border-radius: 8px;"
        "  padding-left: 25px;"
        "  padding-right: 45px;"
        "  border: none;"
        "}"
        "QComboBox:hover {"
        "  background-color: #f7f9fc;"
        "}"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width: 45px;"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QComboBox::down-arrow {"
        "  image: none;"
        "  border-left: 9px solid transparent;"
        "  border-right: 9px solid transparent;"
        "  border-top: 11px solid #14166B;"
        "  width: 0px;"
        "  height: 0px;"
        "  margin-right: 15px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #ffffff;"
        "  color: #14166B;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "  font-style: normal;"
        "  selection-background-color: #e8ecf8;"
        "  selection-color: #14166B;"
        "  border: 1px solid #cbd5e0;"
        "  border-radius: 8px;"
        "  padding: 6px;"
        "  outline: none;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "  min-height: 40px;"
        "  padding-left: 15px;"
        "  border-radius: 6px;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "  background-color: #e8ecf8;"
        "  color: #14166B;"
        "}"
    );

    // Disable selection of placeholder in list
    QAbstractItemView *view = roleCombo->view();
    if (view) {
        view->setStyleSheet(
            "QAbstractItemView { background-color: #ffffff; color: #14166B; border: 1px solid #cbd5e0; border-radius: 8px; padding: 5px; outline: none; }"
            "QAbstractItemView::item { min-height: 42px; padding: 5px 15px; color: #14166B; font-weight: bold; border-radius: 6px; }"
            "QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }"
            "QAbstractItemView::item:selected { background-color: #dbe4f9; color: #14166B; }"
        );
    }

    connect(roleCombo, QOverload<int>::of(&QComboBox::activated), this, &LandingWindow::onRoleChosen);

    cardLayout->addStretch(1);
    cardLayout->addWidget(logoLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(5);
    cardLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(25);
    cardLayout->addWidget(roleCombo, 0, Qt::AlignCenter);
    cardLayout->addStretch(1);

    rootLayout->addWidget(card, 0, Qt::AlignCenter);
}

void LandingWindow::onRoleChosen(int index)
{
    if (index == 1) {
        emit roleSelected(RoleEtudiant);
    } else if (index == 2) {
        emit roleSelected(RoleProfesseur);
    } else if (index == 3) {
        emit roleSelected(RoleAdministrateur);
    }
    // Reset back to placeholder for future return
    roleCombo->setCurrentIndex(0);
}
