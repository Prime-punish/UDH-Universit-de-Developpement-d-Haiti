#include "evenementsadminwidget.h"
#include <QVBoxLayout>

EvenementsAdminWidget::EvenementsAdminWidget(const std::string &posteAdmin, const std::string &faculteAdmin, QWidget *parent)
    : QWidget(parent), poste(posteAdmin), faculte(faculteAdmin),
      tabWidget(nullptr), calendrierWidget(nullptr), reunionsWidget(nullptr)
{
    setupUI();
}

void EvenementsAdminWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 15);
    mainLayout->setSpacing(10);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e2e8f0; background: #ffffff; border-radius: 10px; top: -1px; }"
        "QTabBar::tab { background: #edf2f7; color: #4a5568; padding: 10px 24px; font-weight: bold; font-size: 13px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #ffffff; color: #0b1e36; border-bottom: 3px solid #0b1e36; }"
        "QTabBar::tab:hover:!selected { background: #e2e8f0; }"
    );

    calendrierWidget = new CalendrierScolaire(false, this);
    reunionsWidget = new ReunionsAdminView(poste, faculte, this);

    tabWidget->addTab(calendrierWidget, "🗓️ Calendrier Scolaire");
    tabWidget->addTab(reunionsWidget, "🏛️ Réunions du Comité de Direction");

    mainLayout->addWidget(tabWidget);

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int idx) {
        if (idx == 0 && calendrierWidget) calendrierWidget->refreshData();
        else if (idx == 1 && reunionsWidget) reunionsWidget->refreshData();
    });
}

void EvenementsAdminWidget::refreshData()
{
    if (calendrierWidget) calendrierWidget->refreshData();
    if (reunionsWidget) reunionsWidget->refreshData();
}
