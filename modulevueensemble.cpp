#include "modulevueensemble.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

static QFrame* createStatCard(const QString &icon, const QString &title, QLabel *&valueLabel, const QString &gradient)
{
    QFrame *card = new QFrame();
    card->setFixedHeight(130);
    card->setMinimumWidth(200);
    card->setStyleSheet(QString(
        "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,%1); border-radius: 16px; }"
    ).arg(gradient));

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 35));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setContentsMargins(20, 16, 20, 16);
    lay->setSpacing(6);

    QLabel *iconLbl = new QLabel(icon);
    iconLbl->setStyleSheet("font-size: 28px; background: transparent;");
    lay->addWidget(iconLbl);

    valueLabel = new QLabel("0");
    valueLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white; background: transparent;");
    lay->addWidget(valueLabel);

    QLabel *titleLbl = new QLabel(title);
    titleLbl->setStyleSheet("font-size: 12px; color: rgba(255,255,255,0.85); background: transparent;");
    lay->addWidget(titleLbl);

    return card;
}

ModuleVueEnsemble::ModuleVueEnsemble(std::vector<Administrateur> &adminsRef,
                                     std::vector<Professeur> &profsRef,
                                     std::vector<CompteEtudiant> &comptesRef,
                                     QWidget *parent)
    : QWidget(parent), admins(adminsRef), profs(profsRef), comptes(comptesRef),
      lblTotalAdmins(nullptr), lblTotalProfs(nullptr), lblTotalEtudiants(nullptr),
      lblProfsValides(nullptr), lblProfsPayes(nullptr), lblAdminsActifs(nullptr)
{
    setupUI();
    updateStats();
}

void ModuleVueEnsemble::refreshData() { updateStats(); }

void ModuleVueEnsemble::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(20);

    QLabel *title = new QLabel("🏛️  Vue d'ensemble de l'université");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a202c; background: transparent;");
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("Statistiques globales et indicateurs de performance universitaires.");
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size: 13px; color: #718096; background: transparent; margin-bottom: 8px;");
    mainLayout->addWidget(subtitle);

    /* ---- Cards Grid ---- */
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(16);

    grid->addWidget(createStatCard("👥", "Total Administrateurs", lblTotalAdmins,
        "stop:0 #667eea, stop:1 #764ba2"), 0, 0);
    grid->addWidget(createStatCard("👨‍🏫", "Total Professeurs", lblTotalProfs,
        "stop:0 #f093fb, stop:1 #f5576c"), 0, 1);
    grid->addWidget(createStatCard("🎓", "Total Étudiants", lblTotalEtudiants,
        "stop:0 #4facfe, stop:1 #00f2fe"), 0, 2);
    grid->addWidget(createStatCard("✅", "Professeurs validés", lblProfsValides,
        "stop:0 #43e97b, stop:1 #38f9d7"), 1, 0);
    grid->addWidget(createStatCard("💰", "Professeurs payés", lblProfsPayes,
        "stop:0 #fa709a, stop:1 #fee140"), 1, 1);
    grid->addWidget(createStatCard("🟢", "Admins actifs", lblAdminsActifs,
        "stop:0 #a18cd1, stop:1 #fbc2eb"), 1, 2);

    mainLayout->addLayout(grid);
    mainLayout->addStretch();
}

void ModuleVueEnsemble::updateStats()
{
    if (lblTotalAdmins) lblTotalAdmins->setText(QString::number(admins.size()));
    if (lblTotalProfs) lblTotalProfs->setText(QString::number(profs.size()));
    if (lblTotalEtudiants) lblTotalEtudiants->setText(QString::number(comptes.size()));

    int profsVal = 0, profsPay = 0;
    for (const auto &p : profs) {
        if (p.estValide()) profsVal++;
        if (p.getEstPaye()) profsPay++;
    }
    if (lblProfsValides) lblProfsValides->setText(QString::number(profsVal));
    if (lblProfsPayes) lblProfsPayes->setText(QString::number(profsPay));

    int admActifs = 0;
    for (const auto &a : admins) {
        if (a.getStatutCompte() == "Actif") admActifs++;
    }
    if (lblAdminsActifs) lblAdminsActifs->setText(QString::number(admActifs));
}
