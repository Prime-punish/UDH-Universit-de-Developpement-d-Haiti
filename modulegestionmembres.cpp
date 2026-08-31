#include "modulegestionmembres.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>

ModuleGestionMembres::ModuleGestionMembres(std::vector<Administrateur> &adminsRef,
                                           std::vector<Professeur> &profsRef,
                                           QWidget *parent)
    : QWidget(parent), admins(adminsRef), profs(profsRef),
      table(nullptr), filterCombo(nullptr), searchEdit(nullptr)
{
    setupUI();
    populateTable();
}

void ModuleGestionMembres::refreshData() { populateTable(); }

void ModuleGestionMembres::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(18);

    /* ---- Header ---- */
    QLabel *title = new QLabel("👥  Gestion des membres");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #1a202c; background: transparent;");
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("Gérez le personnel administratif et les professeurs : nomination, suspension, licenciement, etc.");
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size: 13px; color: #718096; background: transparent; margin-bottom: 6px;");
    mainLayout->addWidget(subtitle);

    /* ---- Toolbar ---- */
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(12);

    filterCombo = new QComboBox();
    filterCombo->addItems({"Tous", "Administrateurs", "Professeurs", "Actif", "En congé", "Rétrogradé", "Licencié", "Révoqué"});
    filterCombo->setFixedHeight(36);
    filterCombo->setMinimumWidth(180);
    filterCombo->setStyleSheet(
        "QComboBox { border: 1px solid #cbd5e0; border-radius: 8px; padding: 0 12px; font-size: 13px; background: white; color: #2d3748; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: white; color: #2d3748; selection-background-color: #edf2f7; }"
    );
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]{ populateTable(); });
    toolbarLayout->addWidget(filterCombo);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍  Rechercher par nom...");
    searchEdit->setFixedHeight(36);
    searchEdit->setMinimumWidth(250);
    searchEdit->setStyleSheet(
        "QLineEdit { border: 1px solid #cbd5e0; border-radius: 8px; padding: 0 12px; font-size: 13px; background: white; color: #2d3748; }"
    );
    connect(searchEdit, &QLineEdit::textChanged, this, [this]{ populateTable(); });
    toolbarLayout->addWidget(searchEdit);

    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    /* ---- Table ---- */
    table = new QTableWidget();
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"ID", "Nom / Prénom", "Type", "Poste / Faculté", "Statut", "Actions"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(44);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->setStyleSheet(
        "QTableWidget { background-color: white; border: 1px solid #e2e8f0; border-radius: 12px; font-size: 13px; }"
        "QHeaderView::section { background-color: #f7fafc; color: #4a5568; font-weight: bold; font-size: 12px; padding: 10px; border: none; border-bottom: 2px solid #e2e8f0; }"
        "QTableWidget::item { padding: 8px; border-bottom: 1px solid #edf2f7; color: #2d3748; }"
        "QTableWidget::item:alternate { background-color: #f9fafb; }"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(18);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 3);
    table->setGraphicsEffect(shadow);

    mainLayout->addWidget(table, 1);
}

void ModuleGestionMembres::populateTable()
{
    table->setRowCount(0);

    QString filter = filterCombo->currentText();
    QString search = searchEdit->text().trimmed().toLower();

    int row = 0;

    // -- Admins --
    if (filter == "Tous" || filter == "Administrateurs" || filter == "Actif" || filter == "En congé"
        || filter == "Rétrogradé" || filter == "Licencié" || filter == "Révoqué") {
        for (int i = 0; i < (int)admins.size(); ++i) {
            const auto &a = admins[i];
            QString statut = QString::fromStdString(a.getStatutCompte());
            QString nom = QString::fromStdString(a.getNom() + " " + a.getPrenom());

            if (filter == "Administrateurs" || filter == "Tous") { /* pass */ }
            else if (filter != statut) continue;

            if (!search.isEmpty() && !nom.toLower().contains(search)) continue;

            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(a.getId())));
            table->setItem(row, 1, new QTableWidgetItem(nom));
            table->setItem(row, 2, new QTableWidgetItem("Admin"));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(a.getPoste()) +
                (a.getFaculte().empty() ? "" : " (" + QString::fromStdString(a.getFaculte()) + ")")));

            // Statut badge
            QLabel *badge = new QLabel(statut);
            badge->setAlignment(Qt::AlignCenter);
            QString badgeColor;
            if (statut == "Actif") badgeColor = "background-color: #c6f6d5; color: #276749;";
            else if (statut == "En congé") badgeColor = "background-color: #fefcbf; color: #975a16;";
            else if (statut == "Rétrogradé") badgeColor = "background-color: #fed7d7; color: #9b2c2c;";
            else if (statut == "Licencié") badgeColor = "background-color: #feb2b2; color: #742a2a;";
            else if (statut == "Révoqué") badgeColor = "background-color: #e2e8f0; color: #4a5568;";
            else badgeColor = "background-color: #e2e8f0; color: #4a5568;";
            badge->setStyleSheet(QString("font-size: 11px; font-weight: bold; border-radius: 10px; padding: 4px 12px; %1").arg(badgeColor));
            QWidget *bw = new QWidget();
            QHBoxLayout *bl = new QHBoxLayout(bw);
            bl->setContentsMargins(4, 2, 4, 2);
            bl->addWidget(badge, 0, Qt::AlignCenter);
            table->setCellWidget(row, 4, bw);

            // Actions button
            bool isPresident = (a.getPoste() == "Président" || a.getPoste() == "President");
            if (isPresident) {
                QLabel *noActLbl = new QLabel("🔒 Supérieur (Président)");
                noActLbl->setStyleSheet("font-size: 11px; font-weight: bold; color: #718096; background-color: #edf2f7; border-radius: 6px; padding: 4px 10px;");
                noActLbl->setAlignment(Qt::AlignCenter);
                QWidget *aw = new QWidget();
                QHBoxLayout *al = new QHBoxLayout(aw);
                al->setContentsMargins(4, 2, 4, 2);
                al->addWidget(noActLbl, 0, Qt::AlignCenter);
                table->setCellWidget(row, 5, aw);
            } else {
                QPushButton *actBtn = new QPushButton("⚡ Actions");
                actBtn->setFixedHeight(32);
                actBtn->setMinimumWidth(100);
                actBtn->setCursor(Qt::PointingHandCursor);
                actBtn->setStyleSheet(
                    "QPushButton { background-color: #0b1e36; color: white; border: none; border-radius: 6px; font-size: 12px; font-weight: bold; padding: 4px 14px; }"
                    "QPushButton:hover { background-color: #1a3353; }"
                );
                QString pid = QString::fromStdString(a.getId());
                connect(actBtn, &QPushButton::clicked, this, [this, pid]{ onActionClicked(pid, "admin", "menu"); });
                QWidget *aw = new QWidget();
                QHBoxLayout *al = new QHBoxLayout(aw);
                al->setContentsMargins(4, 2, 4, 2);
                al->addWidget(actBtn, 0, Qt::AlignCenter);
                table->setCellWidget(row, 5, aw);
            }

            row++;
        }
    }

    // -- Professeurs --
    if (filter == "Tous" || filter == "Professeurs" || filter == "Actif" || filter == "En congé"
        || filter == "Rétrogradé" || filter == "Licencié" || filter == "Révoqué") {
        for (int i = 0; i < (int)profs.size(); ++i) {
            const auto &pr = profs[i];
            QString statut = QString::fromStdString(pr.getStatutCompte());
            QString nom = QString::fromStdString(pr.getNom() + " " + pr.getPrenom());

            if (filter == "Professeurs" || filter == "Tous") { /* pass */ }
            else if (filter != statut) continue;

            if (!search.isEmpty() && !nom.toLower().contains(search)) continue;

            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(pr.getId())));
            table->setItem(row, 1, new QTableWidgetItem(nom));
            table->setItem(row, 2, new QTableWidgetItem("Professeur"));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(pr.getFaculte())));

            // Statut badge
            QLabel *badge = new QLabel(statut);
            badge->setAlignment(Qt::AlignCenter);
            QString badgeColor;
            if (statut == "Actif") badgeColor = "background-color: #c6f6d5; color: #276749;";
            else if (statut == "En congé") badgeColor = "background-color: #fefcbf; color: #975a16;";
            else if (statut == "Rétrogradé") badgeColor = "background-color: #fed7d7; color: #9b2c2c;";
            else if (statut == "Licencié") badgeColor = "background-color: #feb2b2; color: #742a2a;";
            else if (statut == "Révoqué") badgeColor = "background-color: #e2e8f0; color: #4a5568;";
            else badgeColor = "background-color: #e2e8f0; color: #4a5568;";
            badge->setStyleSheet(QString("font-size: 11px; font-weight: bold; border-radius: 10px; padding: 4px 12px; %1").arg(badgeColor));
            QWidget *bw = new QWidget();
            QHBoxLayout *bll = new QHBoxLayout(bw);
            bll->setContentsMargins(4, 2, 4, 2);
            bll->addWidget(badge, 0, Qt::AlignCenter);
            table->setCellWidget(row, 4, bw);

            // Actions button
            QPushButton *actBtn = new QPushButton("⚡ Actions");
            actBtn->setFixedHeight(32);
            actBtn->setMinimumWidth(100);
            actBtn->setCursor(Qt::PointingHandCursor);
            actBtn->setStyleSheet(
                "QPushButton { background-color: #0b1e36; color: white; border: none; border-radius: 6px; font-size: 12px; font-weight: bold; padding: 4px 14px; }"
                "QPushButton:hover { background-color: #1a3353; }"
            );
            QString pid = QString::fromStdString(pr.getId());
            connect(actBtn, &QPushButton::clicked, this, [this, pid]{ onActionClicked(pid, "prof", "menu"); });
            QWidget *aw = new QWidget();
            QHBoxLayout *al = new QHBoxLayout(aw);
            al->setContentsMargins(4, 2, 4, 2);
            al->addWidget(actBtn, 0, Qt::AlignCenter);
            table->setCellWidget(row, 5, aw);

            row++;
        }
    }
}

void ModuleGestionMembres::onActionClicked(const QString &personId, const QString &personType, const QString &action)
{
    Q_UNUSED(action);
    if (personType == "admin") {
        for (const auto &a : admins) {
            if (QString::fromStdString(a.getId()) == personId) {
                if (a.getPoste() == "Président" || a.getPoste() == "President") {
                    QMessageBox::warning(this, "Action non autorisée", "Le Recteur ne peut pas effectuer d'action sur le Président (autorité supérieure).");
                    return;
                }
                break;
            }
        }
    }

    QMenu menu;
    menu.setStyleSheet(
        "QMenu { background: white; border: 1px solid #e2e8f0; border-radius: 8px; padding: 6px; }"
        "QMenu::item { padding: 8px 20px; font-size: 13px; color: #2d3748; border-radius: 4px; }"
        "QMenu::item:selected { background-color: #edf2f7; }"
    );

    if (personType == "admin") {
        menu.addAction("📋  Nommer à un poste");
    }
    menu.addAction("🛑  Mettre en congé");
    menu.addAction("⬇️  Rétrograder");
    menu.addAction("🔴  Licencier");
    menu.addAction("⛔  Révoquer");
    menu.addAction("✅  Réactiver");

    QAction *selected = menu.exec(QCursor::pos());
    if (!selected) return;

    QString text = selected->text();

    if (personType == "admin") {
        for (auto &a : admins) {
            if (QString::fromStdString(a.getId()) == personId) {
                if (text.contains("Nommer")) {
                    QStringList postes = {"Recteur", "Président", "Secrétaire de l'administration", "Coordonnateur de faculté"};
                    bool ok;
                    QString newPoste = QInputDialog::getItem(this, "Nomination", "Sélectionnez le nouveau poste :", postes, 0, false, &ok);
                    if (ok && !newPoste.isEmpty()) {
                        a.setPoste(newPoste.toStdString());
                        if (newPoste == "Coordonnateur de faculté") {
                            QStringList facs = {"Sciences Informatiques", "Médecine", "Droit", "Gestion", "Sciences de l'Éducation", "Agronomie"};
                            QString fac = QInputDialog::getItem(this, "Faculté", "Sélectionnez la faculté :", facs, 0, false, &ok);
                            if (ok) a.setFaculte(fac.toStdString());
                        }
                        sauvegarderAdministrateurs(admins);
                        QMessageBox::information(this, "Nomination", QString("%1 a été nommé(e) au poste de %2.").arg(QString::fromStdString(a.getNom() + " " + a.getPrenom())).arg(newPoste));
                    }
                } else if (text.contains("congé")) {
                    a.setStatutCompte("En congé");
                    sauvegarderAdministrateurs(admins);
                    QMessageBox::information(this, "Congé", "Le membre a été mis en congé.");
                } else if (text.contains("Rétrograder")) {
                    a.setStatutCompte("Rétrogradé");
                    sauvegarderAdministrateurs(admins);
                    QMessageBox::information(this, "Rétrogradation", "Le membre a été rétrogradé.");
                } else if (text.contains("Licencier")) {
                    a.setStatutCompte("Licencié");
                    sauvegarderAdministrateurs(admins);
                    QMessageBox::information(this, "Licenciement", "Le membre a été licencié.");
                } else if (text.contains("Révoquer")) {
                    a.setStatutCompte("Révoqué");
                    sauvegarderAdministrateurs(admins);
                    QMessageBox::information(this, "Révocation", "Le membre a été révoqué.");
                } else if (text.contains("Réactiver")) {
                    a.setStatutCompte("Actif");
                    sauvegarderAdministrateurs(admins);
                    QMessageBox::information(this, "Réactivation", "Le membre a été réactivé.");
                }
                break;
            }
        }
    } else {
        for (auto &pr : profs) {
            if (QString::fromStdString(pr.getId()) == personId) {
                if (text.contains("congé")) {
                    pr.setStatutCompte("En congé");
                } else if (text.contains("Rétrograder")) {
                    pr.setStatutCompte("Rétrogradé");
                } else if (text.contains("Licencier")) {
                    pr.setStatutCompte("Licencié");
                } else if (text.contains("Révoquer")) {
                    pr.setStatutCompte("Révoqué");
                } else if (text.contains("Réactiver")) {
                    pr.setStatutCompte("Actif");
                }
                sauvegarderProfesseurs(profs);
                QString msg = "Statut du professeur mis à jour.";
                QMessageBox::information(this, "Statut", msg);
                break;
            }
        }
    }
    populateTable();
}
