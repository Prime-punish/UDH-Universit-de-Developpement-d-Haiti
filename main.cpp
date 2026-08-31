#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "UDH_Universite_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    a.setStyleSheet(
        "QDialog, QMessageBox, QInputDialog, QFileDialog { background-color: #f8fafc; color: #1a202c; font-family: 'Segoe UI', sans-serif; }"
        "QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel, QFileDialog QLabel { color: #1a202c; font-size: 13px; background: transparent; }"
        "QLineEdit, QSpinBox, QDoubleSpinBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 13px; font-weight: 500; selection-background-color: #2b6cb0; selection-color: #ffffff; }"
        "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border: 1.5px solid #2b6cb0; }"
        "QTextEdit, QPlainTextEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 13px; }"
        "QDateEdit, QTimeEdit, QDateTimeEdit { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 4px 8px; font-size: 13px; }"
        "QCheckBox { color: #1a202c; font-size: 12px; font-weight: 600; }"
        "QComboBox { background-color: #ffffff; color: #1a202c; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 6px 10px; font-size: 13px; font-weight: 500; }"
        "QComboBox:focus, QComboBox:hover { border: 1.5px solid #2b6cb0; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a202c; selection-background-color: #e8ecf8; selection-color: #14166B; border: 1.5px solid #cbd5e0; border-radius: 6px; padding: 4px; outline: none; }"
        "QComboBox QAbstractItemView::item { background-color: #ffffff; color: #1a202c; min-height: 36px; padding: 6px 12px; font-size: 13px; border-radius: 4px; }"
        "QComboBox QAbstractItemView::item:hover { background-color: #e8ecf8; color: #14166B; }"
        "QComboBox QAbstractItemView::item:selected { background-color: #dbe4f9; color: #14166B; font-weight: bold; }"
        "QMessageBox QPushButton, QDialogButtonBox QPushButton, QDialog QPushButton, QInputDialog QPushButton, QFileDialog QPushButton, QPushButton { background-color: #001F3F; color: #ffffff; border-radius: 6px; padding: 7px 18px; font-weight: bold; font-size: 13px; min-width: 80px; min-height: 28px; border: none; outline: none; }"
        "QMessageBox QPushButton:hover, QDialogButtonBox QPushButton:hover, QDialog QPushButton:hover, QInputDialog QPushButton:hover, QFileDialog QPushButton:hover, QPushButton:hover { background-color: #0D3B66; color: #ffffff; }"
        "QPushButton:pressed { background-color: #001226; color: #ffffff; }"
        "QPushButton:disabled { background-color: #a0aec0; color: #e2e8f0; }"
    );

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
