#include "calendrierscolaire.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QMessageBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTextCharFormat>
#include <fstream>
#include <sstream>
#include <algorithm>

static const std::string FICHIER_EVENEMENTS = "evenements.txt";
static const char SEPARATEUR_EVENT = '~';

static std::vector<std::string> decouperEvent(const std::string &texte, char sep) {
    std::vector<std::string> champs;
    std::stringstream ss(texte);
    std::string champ;
    while (getline(ss, champ, sep)) {
        champs.push_back(champ);
    }
    if (!texte.empty() && texte.back() == sep) {
        champs.push_back("");
    }
    return champs;
}

static std::string echapperTexte(std::string texte) {
    std::string res;
    for (char c : texte) {
        if (c == '\n') res += "\\n";
        else if (c == '\r') continue;
        else if (c == SEPARATEUR_EVENT) res += "-";
        else res += c;
    }
    return res;
}

static std::string restaurerTexte(const std::string &texte) {
    std::string res;
    for (size_t i = 0; i < texte.size(); ++i) {
        if (texte[i] == '\\' && i + 1 < texte.size() && texte[i + 1] == 'n') {
            res += '\n';
            i++;
        } else {
            res += texte[i];
        }
    }
    return res;
}

EvenementScolaire::EvenementScolaire(const std::string &id, const std::string &titre, const std::string &date,
                                     const std::string &description, const std::string &type)
    : id(id), titre(titre), date(date), description(description), type(type) {}

std::string EvenementScolaire::toLigne() const {
    std::ostringstream oss;
    oss << id << SEPARATEUR_EVENT
        << echapperTexte(titre) << SEPARATEUR_EVENT
        << date << SEPARATEUR_EVENT
        << echapperTexte(description) << SEPARATEUR_EVENT
        << type;
    return oss.str();
}

EvenementScolaire EvenementScolaire::fromLigne(const std::string &ligne) {
    auto c = decouperEvent(ligne, SEPARATEUR_EVENT);
    if (c.size() < 5) return EvenementScolaire();

    std::string id = c[0];
    std::string titre = restaurerTexte(c[1]);
    std::string date = c[2];
    std::string desc = restaurerTexte(c[3]);
    std::string type = c[4];

    return EvenementScolaire(id, titre, date, desc, type);
}

std::vector<EvenementScolaire> chargerEvenementsScolaires() {
    std::vector<EvenementScolaire> liste;
    std::ifstream f(FICHIER_EVENEMENTS);
    if (!f.is_open()) return liste;

    std::string ligne;
    while (std::getline(f, ligne)) {
        if (ligne.empty()) continue;
        EvenementScolaire e = EvenementScolaire::fromLigne(ligne);
        if (!e.id.empty()) {
            liste.push_back(e);
        }
    }
    return liste;
}

void sauvegarderEvenementsScolaires(const std::vector<EvenementScolaire> &events) {
    std::ofstream f(FICHIER_EVENEMENTS, std::ios::trunc);
    if (!f.is_open()) return;

    for (const auto &e : events) {
        f << e.toLigne() << "\n";
    }
}

void ajouterEvenementScolaire(const EvenementScolaire &event) {
    std::vector<EvenementScolaire> liste = chargerEvenementsScolaires();
    liste.push_back(event);
    sauvegarderEvenementsScolaires(liste);
}

bool supprimerEvenementScolaire(const std::string &idEvent) {
    std::vector<EvenementScolaire> liste = chargerEvenementsScolaires();
    auto it = std::remove_if(liste.begin(), liste.end(), [&](const EvenementScolaire &e) {
        return e.id == idEvent;
    });
    if (it != liste.end()) {
        liste.erase(it, liste.end());
        sauvegarderEvenementsScolaires(liste);
        return true;
    }
    return false;
}

// ============================================================
//  CalendrierScolaire Widget Implementation
// ============================================================
CalendrierScolaire::CalendrierScolaire(bool allowManagement, QWidget *parent)
    : QWidget(parent), isManager(allowManagement), calendarWidget(nullptr), eventsTable(nullptr),
      titleEdit(nullptr), dateEdit(nullptr), typeCombo(nullptr), descEdit(nullptr), selectedDateLabel(nullptr)
{
    setupUI();
    refreshData();
}

void CalendrierScolaire::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *content = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(18);

    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setSpacing(20);

    // Left Frame: Calendar Widget
    QFrame *calCard = new QFrame(content);
    calCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; } "
                           "QLabel { color: #1a202c; font-size: 13px; font-weight: bold; background: transparent; }");
    QGraphicsDropShadowEffect *shadow1 = new QGraphicsDropShadowEffect(calCard);
    shadow1->setBlurRadius(15);
    shadow1->setColor(QColor(0, 0, 0, 15));
    shadow1->setOffset(0, 4);
    calCard->setGraphicsEffect(shadow1);

    QVBoxLayout *cLayout = new QVBoxLayout(calCard);
    cLayout->setContentsMargins(18, 16, 18, 16);
    cLayout->setSpacing(12);

    QLabel *calTitle = new QLabel("🗓️ Calendrier Académique", calCard);
    calTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36; border: none;");
    cLayout->addWidget(calTitle);

    calendarWidget = new QCalendarWidget(calCard);
    calendarWidget->setGridVisible(true);
    calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendarWidget->setStyleSheet(
        "QCalendarWidget QWidget { background-color: #ffffff; color: #2d3748; font-size: 13px; }"
        "QCalendarWidget QToolButton { color: #0b1e36; font-weight: bold; background-color: #edf2f7; border-radius: 6px; padding: 6px; margin: 2px; }"
        "QCalendarWidget QToolButton:hover { background-color: #e2e8f0; }"
        "QCalendarWidget QMenu { background-color: #ffffff; color: #2d3748; }"
        "QCalendarWidget QSpinBox { background-color: #ffffff; color: #1a202c; border: 1px solid #cbd5e0; border-radius: 4px; }"
        "QCalendarWidget QAbstractItemView:enabled { font-size: 12px; color: #2d3748; background-color: #ffffff; selection-background-color: #0b1e36; selection-color: #ffffff; }"
    );
    connect(calendarWidget, &QCalendarWidget::selectionChanged, this, &CalendrierScolaire::onCalendarDateSelected);
    cLayout->addWidget(calendarWidget);

    selectedDateLabel = new QLabel(QString("Date sélectionnée : %1").arg(calendarWidget->selectedDate().toString("dd/MM/yyyy")), calCard);
    selectedDateLabel->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 13px; border: none; background: transparent;");
    cLayout->addWidget(selectedDateLabel);

    topRow->addWidget(calCard, isManager ? 1 : 1);

    // Right Frame: Add Event Form (if isManager)
    if (isManager) {
        QFrame *formCard = new QFrame(content);
        formCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; } "
                                "QLabel { color: #1a202c; font-size: 13px; font-weight: bold; background: transparent; }");
        QGraphicsDropShadowEffect *shadow2 = new QGraphicsDropShadowEffect(formCard);
        shadow2->setBlurRadius(15);
        shadow2->setColor(QColor(0, 0, 0, 15));
        shadow2->setOffset(0, 4);
        formCard->setGraphicsEffect(shadow2);

        QVBoxLayout *fLayout = new QVBoxLayout(formCard);
        fLayout->setContentsMargins(20, 16, 20, 16);
        fLayout->setSpacing(10);

        QLabel *formTitle = new QLabel("➕ Programmer un événement", formCard);
        formTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0b1e36; border: none;");
        fLayout->addWidget(formTitle);

        auto makeCalFieldLabel = [formCard](const QString &text) {
            QLabel *l = new QLabel(text, formCard);
            l->setStyleSheet("font-weight: bold; color: #1a202c; font-size: 13px; border: none; background: transparent;");
            return l;
        };

        fLayout->addWidget(makeCalFieldLabel("Titre de l'événement :"));
        titleEdit = new QLineEdit(formCard);
        titleEdit->setPlaceholderText("Ex: Examens intra, Rentrée d'automne...");
        titleEdit->setFixedHeight(36);
        titleEdit->setStyleSheet("QLineEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }"
                                 "QLineEdit:focus { border: 1.5px solid #2b6cb0; }");
        fLayout->addWidget(titleEdit);

        QHBoxLayout *rowFields = new QHBoxLayout();
        rowFields->setSpacing(10);

        QVBoxLayout *dCol = new QVBoxLayout();
        dCol->addWidget(makeCalFieldLabel("Date :"));
        dateEdit = new QDateEdit(QDate::currentDate(), formCard);
        dateEdit->setCalendarPopup(true);
        dateEdit->setDisplayFormat("dd/MM/yyyy");
        dateEdit->setFixedHeight(36);
        dateEdit->setStyleSheet("QDateEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 0 10px; font-size: 13px; font-weight: 500; }");
        dCol->addWidget(dateEdit);
        rowFields->addLayout(dCol);

        QVBoxLayout *tCol = new QVBoxLayout();
        tCol->addWidget(makeCalFieldLabel("Type :"));
        typeCombo = new QComboBox(formCard);
        typeCombo->addItems({"Examen", "Rentrée", "Vacances", "Autre"});
        typeCombo->setFixedHeight(36);
        typeCombo->setStyleSheet("QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding-left: 8px; font-weight: bold; font-size: 12px; } "
                                 "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; }");
        tCol->addWidget(typeCombo);
        rowFields->addLayout(tCol);

        fLayout->addLayout(rowFields);

        fLayout->addWidget(makeCalFieldLabel("Description :"));
        descEdit = new QTextEdit(formCard);
        descEdit->setPlaceholderText("Précisions pour la communauté universitaire...");
        descEdit->setFixedHeight(65);
        descEdit->setStyleSheet("QTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px; font-size: 12px; }");
        fLayout->addWidget(descEdit);

        QPushButton *addBtn = new QPushButton("📢 Diffuser l'événement", formCard);
        addBtn->setFixedHeight(38);
        addBtn->setCursor(Qt::PointingHandCursor);
        addBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; border: none; }"
                              "QPushButton:hover { background-color: #1a3353; }");
        connect(addBtn, &QPushButton::clicked, this, &CalendrierScolaire::onAddEventClicked);
        fLayout->addWidget(addBtn);

        topRow->addWidget(formCard, 1);
    }

    mainLayout->addLayout(topRow);

    // Bottom Table Card: Events list
    QFrame *tableCard = new QFrame(this);
    tableCard->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e2e8f0; }");
    QGraphicsDropShadowEffect *shadow3 = new QGraphicsDropShadowEffect(tableCard);
    shadow3->setBlurRadius(15);
    shadow3->setColor(QColor(0, 0, 0, 15));
    shadow3->setOffset(0, 4);
    tableCard->setGraphicsEffect(shadow3);

    QVBoxLayout *tLayout = new QVBoxLayout(tableCard);
    tLayout->setContentsMargins(20, 16, 20, 16);
    tLayout->setSpacing(12);

    QLabel *tTitle = new QLabel("📋 Événements du Calendrier Scolaire Universitaire", tableCard);
    tTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1a202c; border: none;");
    tLayout->addWidget(tTitle);

    eventsTable = new QTableWidget(tableCard);
    eventsTable->setColumnCount(isManager ? 5 : 4);
    if (isManager) {
        eventsTable->setHorizontalHeaderLabels({"Date", "Titre", "Type", "Description", "Action"});
        eventsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    } else {
        eventsTable->setHorizontalHeaderLabels({"Date", "Titre", "Type", "Description"});
    }
    eventsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    eventsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    eventsTable->verticalHeader()->setVisible(false);
    eventsTable->verticalHeader()->setDefaultSectionSize(38);
    eventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    eventsTable->setAlternatingRowColors(true);
    eventsTable->setShowGrid(false);
    eventsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; color: #2d3748; font-size: 13px; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f8fafc; }"
        "QHeaderView::section { background-color: #edf2f7; color: #4a5568; font-weight: bold; border: none; border-bottom: 2px solid #cbd5e0; padding: 8px; font-size: 12px; }"
    );

    tLayout->addWidget(eventsTable);
    mainLayout->addWidget(tableCard);

    scroll->setWidget(content);
    rootLayout->addWidget(scroll);
}

void CalendrierScolaire::onCalendarDateSelected()
{
    QDate d = calendarWidget->selectedDate();
    if (selectedDateLabel) {
        selectedDateLabel->setText(QString("Date sélectionnée : %1").arg(d.toString("dd/MM/yyyy")));
    }
    if (dateEdit) {
        dateEdit->setDate(d);
    }
}

void CalendrierScolaire::onAddEventClicked()
{
    if (!titleEdit || !descEdit || !dateEdit || !typeCombo) return;

    QString titre = titleEdit->text().trimmed();
    QString desc = descEdit->toPlainText().trimmed();
    QString dateStr = dateEdit->date().toString("dd/MM/yyyy");
    QString typeStr = typeCombo->currentText();

    if (titre.isEmpty()) {
        QMessageBox::warning(this, "Champ manquant", "Veuillez spécifier le titre de l'événement.");
        return;
    }

    std::string evId = "EVT-" + std::to_string(QDateTime::currentMSecsSinceEpoch() % 1000000);
    EvenementScolaire ev(evId, titre.toStdString(), dateStr.toStdString(), desc.toStdString(), typeStr.toStdString());

    ajouterEvenementScolaire(ev);

    titleEdit->clear();
    descEdit->clear();

    QMessageBox::information(this, "Événement créé",
                             "L'événement scolaire a été enregistré et diffusé auprès de tous les étudiants et professeurs.");

    refreshData();
}

void CalendrierScolaire::onDeleteEventClicked(const QString &idEvent)
{
    auto reply = QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer cet événement ?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        supprimerEvenementScolaire(idEvent.toStdString());
        refreshData();
    }
}

void CalendrierScolaire::refreshData()
{
    std::vector<EvenementScolaire> list = chargerEvenementsScolaires();

    // Reset calendar date highlights
    QTextCharFormat defaultFormat;
    calendarWidget->setDateTextFormat(QDate(), defaultFormat);

    for (const auto &e : list) {
        QDate d = QDate::fromString(QString::fromStdString(e.date), "dd/MM/yyyy");
        if (d.isValid()) {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            if (e.type == "Examen") {
                fmt.setBackground(QColor("#FED7D7"));
                fmt.setForeground(QColor("#9B2C2C"));
            } else if (e.type == "Rentrée") {
                fmt.setBackground(QColor("#BEE3F8"));
                fmt.setForeground(QColor("#2B6CB0"));
            } else if (e.type == "Vacances") {
                fmt.setBackground(QColor("#C6F6D5"));
                fmt.setForeground(QColor("#22543D"));
            } else {
                fmt.setBackground(QColor("#FEEBC8"));
                fmt.setForeground(QColor("#C05621"));
            }
            calendarWidget->setDateTextFormat(d, fmt);
        }
    }

    if (!eventsTable) return;
    eventsTable->setRowCount(0);

    for (size_t i = 0; i < list.size(); ++i) {
        const auto &e = list[i];
        eventsTable->insertRow(i);

        QTableWidgetItem *dateItem = new QTableWidgetItem(QString::fromStdString(e.date));
        dateItem->setTextAlignment(Qt::AlignCenter);
        eventsTable->setItem(i, 0, dateItem);

        eventsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.titre)));

        // Type Badge
        QWidget *badgeWidget = new QWidget();
        QHBoxLayout *bLayout = new QHBoxLayout(badgeWidget);
        bLayout->setContentsMargins(4, 2, 4, 2);
        bLayout->setAlignment(Qt::AlignCenter);

        QLabel *badge = new QLabel(QString::fromStdString(e.type));
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedWidth(90);
        badge->setFixedHeight(22);

        if (e.type == "Examen") {
            badge->setStyleSheet("background-color: #FED7D7; color: #9B2C2C; font-weight: bold; border-radius: 11px; font-size: 11px;");
        } else if (e.type == "Rentrée") {
            badge->setStyleSheet("background-color: #BEE3F8; color: #2B6CB0; font-weight: bold; border-radius: 11px; font-size: 11px;");
        } else if (e.type == "Vacances") {
            badge->setStyleSheet("background-color: #C6F6D5; color: #22543D; font-weight: bold; border-radius: 11px; font-size: 11px;");
        } else {
            badge->setStyleSheet("background-color: #FEEBC8; color: #C05621; font-weight: bold; border-radius: 11px; font-size: 11px;");
        }
        bLayout->addWidget(badge);
        eventsTable->setCellWidget(i, 2, badgeWidget);

        eventsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(e.description)));

        if (isManager) {
            QPushButton *delBtn = new QPushButton("🗑️ Supprimer");
            delBtn->setCursor(Qt::PointingHandCursor);
            delBtn->setStyleSheet("QPushButton { background-color: #0b1e36; color: white; border-radius: 4px; padding: 4px 8px; font-weight: bold; font-size: 11px; border: none; }"
                                  "QPushButton:hover { background-color: #1a3353; }");
            QString evId = QString::fromStdString(e.id);
            connect(delBtn, &QPushButton::clicked, [this, evId]() { onDeleteEventClicked(evId); });
            eventsTable->setCellWidget(i, 4, delBtn);
        }
    }
}
