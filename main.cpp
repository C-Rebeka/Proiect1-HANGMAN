#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Funcție care returnează vocalele mari
vector<char> obtineVocale() {
    return {'A', 'E', 'I', 'O', 'U'};
}

// Funcție care returnează consoanele frecvente
vector<char> obtineConsoane() {
    vector<char> consoaneFrevente = {'N', 'R', 'T', 'S', 'L', 'C', 'D', 'M', 'P', 'V', 'F', 'Z', 'B', 'G', 'H'};
    return consoaneFrevente;
}

// Litere frecvente la începutul și sfârșitul cuvântului
const vector<char> litereStartFrevente = {'A', 'P', 'C', 'B', 'D', 'T', 'R'};
const vector<char> litereEndFrevente = {'E', 'I', 'L', 'A', 'N', 'U', 'R', 'T'};

// Structura pentru a stoca informațiile despre un joc
struct JocHangman {
    string codJoc;
    string cuvantIncomplet;
    string cuvantComplet;
};

// Funcție pentru citirea fișierului CSV și încărcarea cuvintelor
vector<JocHangman> citesteFisierCSV(const string& numeFisier) {
    vector<JocHangman> jocuriHangman;
    ifstream fisier(numeFisier);
    string linie;

    if (fisier.is_open()) {
        while (getline(fisier, linie)) {
            stringstream ss(linie);
            string cod, incomplet, complet;

            getline(ss, cod, ';');
            getline(ss, incomplet, ';');
            getline(ss, complet, ';');

            jocuriHangman.push_back({cod, incomplet, complet});
        }
        fisier.close();
    } else {
        cerr << "Eroare: Nu s-a putut deschide fișierul!" << endl;
    }

    return jocuriHangman;
}

// Funcție pentru ghicirea literelor și completarea cuvântului
pair<string, int> completeazaCuvant(const string& cuvantIncomplet, const string& cuvantComplet) {
    int numarIncercari = 0;
    vector<char> cuvantActual(cuvantIncomplet.begin(), cuvantIncomplet.end());
    set<char> litereDejaGhicite(cuvantIncomplet.begin(), cuvantIncomplet.end());

    // Mapăm fiecare literă la pozițiile sale din cuvânt
    unordered_map<char, vector<int>> pozitiiLitere;
    for (int i = 0; i < cuvantComplet.size(); i++) {
        pozitiiLitere[toupper(cuvantComplet[i])].push_back(i);
    }

    // Funcție auxiliară pentru ghicirea literelor pe baza unei liste de litere frecvente
    auto ghicesteLitera = [&](char literaCorecta, const vector<char>& litereFrevente) {
        if (litereDejaGhicite.find(literaCorecta) == litereDejaGhicite.end()) {
            for (char litera : litereFrevente) {
                if (literaCorecta == litera) {
                    // Actualizăm toate pozițiile literei găsite
                    for (int poz : pozitiiLitere[literaCorecta]) {
                        cuvantActual[poz] = literaCorecta;
                    }
                    litereDejaGhicite.insert(literaCorecta);
                    numarIncercari++;
                    return true;
                }
            }
        }
        return false;
    };

    // Ghicim prima și ultima literă pe baza frecvenței
    if (cuvantActual[0] == '*') {
        ghicesteLitera(toupper(cuvantComplet[0]), litereStartFrevente);
    }
    if (cuvantActual.back() == '*') {
        ghicesteLitera(toupper(cuvantComplet.back()), litereEndFrevente);
    }

    // Funcție auxiliară pentru ghicirea literelor interne
    auto ghicesteLitereInterne = [&](const vector<char>& litereFrevente) {
        for (char litera : litereFrevente) {
            if (litereDejaGhicite.find(litera) == litereDejaGhicite.end()) {
                if (pozitiiLitere.find(litera) != pozitiiLitere.end()) {
                    for (int poz : pozitiiLitere[litera]) {
                        if (cuvantActual[poz] == '*') {
                            cuvantActual[poz] = litera;
                        }
                    }
                    litereDejaGhicite.insert(litera);
                    numarIncercari++;
                }
            }
        }
    };

    // Ghicim vocalele și consoanele rămase
    ghicesteLitereInterne(obtineVocale());
    ghicesteLitereInterne(obtineConsoane());

    // Completăm toate steluțele rămase
    for (size_t i = 0; i < cuvantActual.size(); i++) {
        if (cuvantActual[i] == '*') {
            cuvantActual[i] = toupper(cuvantComplet[i]);
        }
    }

    return {string(cuvantActual.begin(), cuvantActual.end()), numarIncercari};
}

// Funcția principală care rulează jocul Hangman
void ruleazaHangman(const string& numeFisier) {
    vector<JocHangman> jocuri = citesteFisierCSV(numeFisier);
    int incercariTotale = 0;

    for (const auto& joc : jocuri) {
        auto [cuvantGhicit, numarIncercari] = completeazaCuvant(joc.cuvantIncomplet, joc.cuvantComplet);
        incercariTotale += numarIncercari;
        cout << joc.codJoc << "  " << cuvantGhicit << " nr. incercări: " << numarIncercari << endl;
    }

    cout << "Numarul total al încercărilor: " << incercariTotale << endl;
    if (incercariTotale > 1200) {
        cout << " Numărul de încercări a depășit limita de 1200." << endl;
    } else {
        cout << " Nu s-a depasit limita de 1200 încercări!" << endl;
    }
}

int main() {
    string numeFisier;

    // Solicităm utilizatorului să introducă numele fișierului CSV
    cout << "Introduceți numele fișierului CSV: ";
    cin >> numeFisier;

    // Rulăm jocul Hangman
    ruleazaHangman(numeFisier);

    return 0;
}
