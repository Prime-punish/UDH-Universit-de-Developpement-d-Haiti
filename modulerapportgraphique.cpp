#include "modulerapportgraphique.h"
#include "paiementversement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <map>
#include <set>

ModuleRapportGraphique::ModuleRapportGraphique(QWidget *parent)
    : QWidget(parent), profsChartView(nullptr), adminsChartView(nullptr), studentsChartView(nullptr),
      cardTotalStudents(nullptr), cardTotalProfs(nullptr), cardSalariesPaid(nullptr)
{
    setupUI();
    refreshData();
}

void ModuleRapportGraphique::setupUI()
{
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(25, 20, 25, 25);
    mainLayout->setSpacing(20);

    // Title Row
    QLabel *pageTitle = new QLabel("📊 Rapports Graphiques & Indicateurs Institutionnels", contentWidget);
    pageTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #0b1e36;");
    mainLayout->addWidget(pageTitle);

    // Stat Cards Row
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    cardTotalStudents = new StatCard("🎓", "0", "Étudiants Inscrits", "#3182CE", contentWidget);
    cardTotalProfs    = new StatCard("👨‍🏫", "0", "Corps Professoral", "#805AD5", contentWidget);
    cardSalariesPaid  = new StatCard("💰", "0%", "Taux de Paiement Global", "#38A169", contentWidget);

    statsLayout->addWidget(cardTotalStudents);
    statsLayout->addWidget(cardTotalProfs);
    statsLayout->addWidget(cardSalariesPaid);
    mainLayout->addLayout(statsLayout);

    // Charts Row 1: Profs Chart & Admins Chart
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->setSpacing(16);

    auto makeChartCard = [](const QString &title, QChartView *&view, QWidget *parent) {
        QFrame *card = new QFrame(parent);
        card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
        shadow->setBlurRadius(15);
        shadow->setColor(QColor(0, 0, 0, 15));
        shadow->setOffset(0, 4);
        card->setGraphicsEffect(shadow);

        QVBoxLayout *l = new QVBoxLayout(card);
        l->setContentsMargins(16, 14, 16, 14);
        l->setSpacing(10);

        QLabel *t = new QLabel(title, card);
        t->setStyleSheet("font-size: 15px; font-weight: bold; color: #1a202c; border: none;");
        l->addWidget(t);

        view = new QChartView(new QChart(), card);
        view->setRenderHint(QPainter::Antialiasing);
        view->chart()->setBackgroundVisible(false);
        view->chart()->setMargins(QMargins(5, 5, 5, 5));
        view->setMinimumHeight(280);
        l->addWidget(view);

        return card;
    };

    row1->addWidget(makeChartCard("👨‍🏫 Statut des Paiements des Professeurs (par Faculté)", profsChartView, contentWidget), 1);
    row1->addWidget(makeChartCard("👔 Statut des Salaires Administratifs (par Poste)", adminsChartView, contentWidget), 1);
    mainLayout->addLayout(row1);

    // Charts Row 2: Students Semester Payment Pie Chart
    QFrame *studentCard = makeChartCard("💳 Répartition des Versements Étudiants du Semestre", studentsChartView, contentWidget);
    studentCard->setMinimumHeight(320);
    mainLayout->addWidget(studentCard);

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void ModuleRapportGraphique::refreshData()
{
    std::vector<Professeur> profs = chargerProfesseurs();
    std::vector<Administrateur> admins = chargerAdministrateurs();
    std::vector<CompteEtudiant> comptes = chargerComptes();
    std::vector<VersementEtudiant> versements = chargerVersements();

    // 1. Update StatCards
    int nbInscrits = 0;
    for (const auto &c : comptes) {
        if (c.estInscrit() || c.getStatutInscription() == "Approuvé") nbInscrits++;
    }
    if (cardTotalStudents) cardTotalStudents->updateValue(QString::number(nbInscrits));

    int nbProfsValides = 0;
    int nbProfsPayes = 0;
    for (const auto &p : profs) {
        if (p.estValide()) nbProfsValides++;
        if (p.getEstPaye()) nbProfsPayes++;
    }
    if (cardTotalProfs) cardTotalProfs->updateValue(QString::number(nbProfsValides));

    int nbAdminsPayes = 0;
    for (const auto &a : admins) {
        if (a.getEstPaye()) nbAdminsPayes++;
    }

    int totalPersonnel = profs.size() + admins.size();
    int totalPayes = nbProfsPayes + nbAdminsPayes;
    int pct = (totalPersonnel > 0) ? (totalPayes * 100 / totalPersonnel) : 0;
    if (cardSalariesPaid) cardSalariesPaid->updateValue(QString("%1%").arg(pct));

    // 2. Build Profs Chart (Bar chart by faculty)
    if (profsChartView) {
        QChart *chart = new QChart();
        chart->setBackgroundVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Faculties map: faculte -> {payes, non_payes}
        std::map<QString, std::pair<int, int>> facProfs;
        for (const auto &p : profs) {
            QString fac = QString::fromStdString(p.getFaculte());
            if (fac.isEmpty()) fac = "Général";
            if (p.getEstPaye()) facProfs[fac].first++;
            else facProfs[fac].second++;
        }

        if (facProfs.empty()) {
            facProfs["Génie Civil"] = {0, 0};
            facProfs["Sciences Info"] = {0, 0};
        }

        QBarSet *setPaye = new QBarSet("Payé");
        setPaye->setColor(QColor("#38A169"));
        QBarSet *setNonPaye = new QBarSet("Non payé");
        setNonPaye->setColor(QColor("#E53E3E"));

        QStringList categories;
        int maxVal = 1;
        for (const auto &pair : facProfs) {
            categories << pair.first;
            *setPaye << pair.second.first;
            *setNonPaye << pair.second.second;
            int sum = pair.second.first + pair.second.second;
            if (sum > maxVal) maxVal = sum;
        }

        QBarSeries *series = new QBarSeries();
        series->append(setPaye);
        series->append(setNonPaye);
        chart->addSeries(series);

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setLabelsColor(QColor("#4A5568"));
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, maxVal + 1);
        axisY->setLabelFormat("%d");
        axisY->setLabelsColor(QColor("#4A5568"));
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setLabelColor(QColor("#2D3748"));

        QChart *old = profsChartView->chart();
        profsChartView->setChart(chart);
        if (old) delete old;
    }

    // 3. Build Admins Chart (Bar chart by Poste)
    if (adminsChartView) {
        QChart *chart = new QChart();
        chart->setBackgroundVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        std::map<QString, std::pair<int, int>> posteAdmins;
        for (const auto &a : admins) {
            QString p = QString::fromStdString(a.getPoste());
            if (p.isEmpty()) p = "Admin";
            if (a.getEstPaye()) posteAdmins[p].first++;
            else posteAdmins[p].second++;
        }

        if (posteAdmins.empty()) {
            posteAdmins["Secrétaire"] = {0, 0};
            posteAdmins["Recteur"] = {0, 0};
        }

        QBarSet *setPayeAdmin = new QBarSet("Payé");
        setPayeAdmin->setColor(QColor("#3182CE"));
        QBarSet *setNonPayeAdmin = new QBarSet("Non payé");
        setNonPayeAdmin->setColor(QColor("#ED8936"));

        QStringList categories;
        int maxVal = 1;
        for (const auto &pair : posteAdmins) {
            categories << pair.first;
            *setPayeAdmin << pair.second.first;
            *setNonPayeAdmin << pair.second.second;
            int sum = pair.second.first + pair.second.second;
            if (sum > maxVal) maxVal = sum;
        }

        QBarSeries *series = new QBarSeries();
        series->append(setPayeAdmin);
        series->append(setNonPayeAdmin);
        chart->addSeries(series);

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setLabelsColor(QColor("#4A5568"));
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, maxVal + 1);
        axisY->setLabelFormat("%d");
        axisY->setLabelsColor(QColor("#4A5568"));
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setLabelColor(QColor("#2D3748"));

        QChart *old = adminsChartView->chart();
        adminsChartView->setChart(chart);
        if (old) delete old;
    }

    // 4. Build Students Semester Payment Pie Chart
    if (studentsChartView) {
        QChart *chart = new QChart();
        chart->setBackgroundVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        int totalStudentsCount = comptes.size();
        int payesCount = 0;
        int enAttenteCount = 0;

        // Check versements
        std::set<std::string> payedStudentIds;
        std::set<std::string> pendingStudentIds;
        for (const auto &v : versements) {
            if (v.statut == "Validé") {
                payedStudentIds.insert(v.idEtudiant);
            } else if (v.statut == "En attente") {
                pendingStudentIds.insert(v.idEtudiant);
            }
        }

        for (const auto &c : comptes) {
            if (c.getInscription().aPaye || payedStudentIds.find(c.getId()) != payedStudentIds.end()) {
                payesCount++;
            } else if (pendingStudentIds.find(c.getId()) != pendingStudentIds.end()) {
                enAttenteCount++;
            }
        }

        int nonPayesCount = totalStudentsCount - payesCount - enAttenteCount;
        if (nonPayesCount < 0) nonPayesCount = 0;
        if (totalStudentsCount == 0) {
            nonPayesCount = 1;
        }

        QPieSeries *pieSeries = new QPieSeries();
        pieSeries->setHoleSize(0.35); // Donut style

        QPieSlice *slicePaye = pieSeries->append(QString("Frais Versés (%1)").arg(payesCount), std::max(payesCount, 0));
        slicePaye->setColor(QColor("#38A169"));
        slicePaye->setLabelVisible(true);

        QPieSlice *sliceAttente = pieSeries->append(QString("En Attente Validation (%1)").arg(enAttenteCount), std::max(enAttenteCount, 0));
        sliceAttente->setColor(QColor("#DD6B20"));
        sliceAttente->setLabelVisible(enAttenteCount > 0);

        QPieSlice *sliceNonPaye = pieSeries->append(QString("Non Versé (%1)").arg(nonPayesCount), std::max(nonPayesCount, 0));
        sliceNonPaye->setColor(QColor("#E53E3E"));
        sliceNonPaye->setLabelVisible(true);

        chart->addSeries(pieSeries);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);
        chart->legend()->setLabelColor(QColor("#2D3748"));

        QChart *old = studentsChartView->chart();
        studentsChartView->setChart(chart);
        if (old) delete old;
    }
}
