#include "ui_helpers.h"
#include <iostream>
#include <iomanip>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void effacerEcran() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\n" << Color::DIM << "Appuyez sur [Entrée] pour continuer..." << Color::RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void afficherBannierePrincipale() {
    cout << Color::BRIGHT_BLUE << Color::BOLD;
    cout << "========================================================================================\n";
    cout << "   _    _ _____  _    _   _    _       _                    _ _   __ \n";
    cout << "  | |  | |  __ \\| |  | | | |  | |     (_)                  (_) | / _|\n";
    cout << "  | |  | | |  | | |__| | | |  | |_ __  ___   _____ _ __ ___ _| |_| |_ \n";
    cout << "  | |  | | |  | |  __  | | |  | | '_ \\| \\ \\ / / _ \\ '__/ __| | __|  _|\n";
    cout << "  | |__| | |__| | |  | | | |__| | | | | |\\ V /  __/ |  \\__ \\ | |_| |  \n";
    cout << "   \\____/|_____/|_|  |_|  \\____/|_| |_|_| \\_/ \\___|_|  |___/_|\\__|_|  \n";
    cout << "\n           UNIVERSITÉ DE DÉVELOPPEMENT D'HAÏTI (UDH) — SYSTÈME CONSOLE          \n";
    cout << "========================================================================================\n";
    cout << Color::RESET;
}

void afficherEntetePage(const string &titre, const string &sousTitre) {
    cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "┌──────────────────────────────────────────────────────────────────────────────────────┐\n";
    cout << "│  " << Color::BRIGHT_GOLD << titre << Color::RESET << Color::BRIGHT_CYAN;
    int pad = 82 - (int)titre.length();
    if (pad > 0) cout << string(pad, ' ');
    cout << "│\n";

    if (!sousTitre.empty()) {
        cout << "│  " << Color::DIM << sousTitre << Color::RESET << Color::BRIGHT_CYAN;
        int padSub = 82 - (int)sousTitre.length();
        if (padSub > 0) cout << string(padSub, ' ');
        cout << "│\n";
    }
    cout << "└──────────────────────────────────────────────────────────────────────────────────────┘\n" << Color::RESET;
}

void afficherSucces(const string &message) {
    cout << "\n" << Color::BOLD << Color::BRIGHT_GREEN << " [SUCCÈS] " << Color::RESET << message << "\n";
}

void afficherErreur(const string &message) {
    cout << "\n" << Color::BOLD << Color::RED << " [ERREUR] " << Color::RESET << message << "\n";
}

void afficherInfo(const string &message) {
    cout << "\n" << Color::BOLD << Color::CYAN << " [INFO] " << Color::RESET << message << "\n";
}

void afficherAvertissement(const string &message) {
    cout << "\n" << Color::BOLD << Color::YELLOW << " [ATTENTION] " << Color::RESET << message << "\n";
}

string lireLigne(const string &prompt) {
    if (!prompt.empty()) {
        cout << Color::BOLD << prompt << Color::RESET;
    }
    string s;
    getline(cin, s);
    return s;
}

string lireMotDePasse(const string &prompt) {
    if (!prompt.empty()) {
        cout << Color::BOLD << prompt << Color::RESET;
    }
    string pwd;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == '\b') {
            if (!pwd.empty()) {
                pwd.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            pwd.push_back(ch);
            cout << '*';
        }
    }
    cout << "\n";
#else
    getline(cin, pwd);
#endif

    return pwd;
}

int lireEntier(const string &prompt, int minVal, int maxVal) {
    while (true) {
        if (!prompt.empty()) {
            cout << Color::BOLD << prompt << Color::RESET;
        }
        string s;
        getline(cin, s);
        if (s.empty()) continue;

        try {
            int val = stoi(s);
            if (val >= minVal && val <= maxVal) {
                return val;
            }
            cout << Color::RED << "Veuillez entrer une valeur entre " << minVal << " et " << maxVal << ".\n" << Color::RESET;
        } catch (...) {
            cout << Color::RED << "Entrée numérique invalide. Réessayez.\n" << Color::RESET;
        }
    }
}

double lireDouble(const string &prompt, double minVal) {
    while (true) {
        if (!prompt.empty()) {
            cout << Color::BOLD << prompt << Color::RESET;
        }
        string s;
        getline(cin, s);
        if (s.empty()) continue;

        try {
            double val = stod(s);
            if (val >= minVal) {
                return val;
            }
            cout << Color::RED << "Veuillez entrer un montant supérieur ou égal à " << minVal << ".\n" << Color::RESET;
        } catch (...) {
            cout << Color::RED << "Montant invalide. Réessayez.\n" << Color::RESET;
        }
    }
}

bool lireConfirmation(const string &prompt) {
    while (true) {
        cout << Color::BOLD << Color::YELLOW << prompt << Color::RESET;
        string s;
        getline(cin, s);
        if (s.empty()) continue;
        char c = tolower(s[0]);
        if (c == 'o' || c == 'y') return true;
        if (c == 'n') return false;
        cout << Color::RED << "Répondez par 'o' (oui) ou 'n' (non).\n" << Color::RESET;
    }
}

void afficherTableau(const vector<string> &entetes,
                     const vector<vector<string>> &lignes,
                     const vector<int> &largeursColonnes) {
    if (entetes.empty()) return;

    // Ligne supérieure
    cout << Color::BRIGHT_BLUE << "+";
    for (int w : largeursColonnes) {
        cout << string(w + 2, '-') << "+";
    }
    cout << "\n";

    // En-têtes
    cout << "|";
    for (size_t i = 0; i < entetes.size(); ++i) {
        int w = (i < largeursColonnes.size()) ? largeursColonnes[i] : 15;
        string tit = entetes[i];
        if ((int)tit.length() > w) tit = tit.substr(0, w - 1) + ".";
        cout << Color::BOLD << Color::WHITE << " " << left << setw(w) << tit << Color::RESET << Color::BRIGHT_BLUE << " |";
    }
    cout << "\n";

    // Séparateur
    cout << "+";
    for (int w : largeursColonnes) {
        cout << string(w + 2, '=') << "+";
    }
    cout << "\n" << Color::RESET;

    // Lignes de données
    if (lignes.empty()) {
        int totalWidth = 0;
        for (int w : largeursColonnes) totalWidth += w + 3;
        cout << "| " << Color::DIM << left << setw(totalWidth - 2) << "Aucune donnée disponible" << Color::RESET << "|\n";
    } else {
        for (const auto &ligne : lignes) {
            cout << "|";
            for (size_t i = 0; i < entetes.size(); ++i) {
                int w = (i < largeursColonnes.size()) ? largeursColonnes[i] : 15;
                string val = (i < ligne.size()) ? ligne[i] : "";
                if ((int)val.length() > w) val = val.substr(0, w - 1) + ".";

                // Colorisation selon statut si applicable
                string colorStr = Color::WHITE;
                if (val == "Approuvé" || val == "Validé" || val == "Validée" || val == "Actif" || val == "Payé" || val == "Oui") {
                    colorStr = Color::BRIGHT_GREEN;
                } else if (val == "Rejeté" || val == "Rejetée" || val == "Non payé" || val == "Révoqué" || val == "Non") {
                    colorStr = Color::RED;
                } else if (val == "En attente" || val == "En congé" || val == "Non réglé") {
                    colorStr = Color::YELLOW;
                }

                cout << " " << colorStr << left << setw(w) << val << Color::RESET << " |";
            }
            cout << "\n";
        }
    }

    // Ligne inférieure
    cout << Color::BRIGHT_BLUE << "+";
    for (int w : largeursColonnes) {
        cout << string(w + 2, '-') << "+";
    }
    cout << Color::RESET << "\n";
}
