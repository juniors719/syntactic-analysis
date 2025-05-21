#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "file_reader.hpp"

using namespace std;

map<string, set<string>> FIRST, FOLLOW;
map<string, bool> nullable;
map<string, vector<vector<string>>> grammar;
string inicial;
set<string> terminals;

/**
 * @brief Verifica se todos os símbolos em uma sequência são nulos.
 *
 * @param symbols Vetor de símbolos a serem verificados.
 * @param nullable Mapa que indica se um símbolo é nulo ou não.
 * @return True se todos os símbolos são nulos, false caso contrário.
 */
bool allNullable(const vector<string>& symbols, map<string, bool>& nullable) {
    for (const string& s : symbols) {
        if (!nullable[s]) return false;
    }
    return true;
}

/**
 * @brief Adiciona os símbolos de um conjunto a outro e verifica se houve
 * mudança.
 *
 * @param target Conjunto de destino.
 * @param source Conjunto de origem.
 * @return true Se houve mudança, false caso contrário.
 */
bool unionChanged(set<string>& target, const set<string>& source) {
    size_t before = target.size();
    target.insert(source.begin(), source.end());
    return target.size() != before;
}

int main() {
    inicial = load_grammar("grammar.txt", grammar);

    // Coletar não-terminais
    set<string> non_terminals;
    for (const auto& pair : grammar) {
        non_terminals.insert(pair.first);
        nullable[pair.first] = false;
    }

    // Identificar terminais e inicializar FIRST dos terminais
    for (const auto& pair : grammar) {
        const auto& productions = pair.second;
        for (const auto& rhs : productions) {
            for (const string& symbol : rhs) {
                if (non_terminals.find(symbol) == non_terminals.end()) {
                    terminals.insert(symbol);
                    FIRST[symbol].insert(symbol);
                }
            }
        }
    }

    FOLLOW[inicial] = {"$"};

    bool changed{};

    do {
        changed = false;

        for (const auto& pair : grammar) {
            const auto& lhs = pair.first;
            const auto& productions = pair.second;
            for (const auto& rhs : productions) {
                if (!nullable[lhs] && allNullable(rhs, nullable)) {
                    nullable[lhs] = true;
                    changed = true;
                }
                for (size_t i = 0; i < rhs.size(); ++i) {
                    if (allNullable(
                            vector<string>(rhs.begin(), rhs.begin() + i),
                            nullable)) {
                        changed |= unionChanged(FIRST[lhs], FIRST[rhs[i]]);
                    }
                }
                for (size_t i = 0; i < rhs.size(); ++i) {
                    const string& B = rhs[i];
                    if (terminals.count(B)) continue;

                    vector<string> beta(rhs.begin() + i + 1, rhs.end());
                    for (const string& b : beta) {
                        if (b == B) continue;
                        changed |= unionChanged(FOLLOW[B], FIRST[b]);
                        if (!nullable[b]) break;
                    }

                    if (allNullable(beta, nullable)) {
                        changed |= unionChanged(FOLLOW[B], FOLLOW[lhs]);
                    }
                }
            }
        }

    } while (changed);

    cout << "Nullable:\n";
    for (const auto& pair : nullable) {
        const string& symbol = pair.first;
        bool isNullable = pair.second;
        if (isNullable) cout << symbol << "\n";
    }

    cout << "\nFIRST:\n";
    for (const auto& pair : FIRST) {
        const string& symbol = pair.first;
        const set<string>& firstSet = pair.second;
        cout << symbol << " -> { ";
        for (const string& s : firstSet) cout << s << " ";
        cout << "}\n";
    }

    cout << "\nFOLLOW:\n";
    for (const auto& pair : FOLLOW) {
        const string& symbol = pair.first;
        const set<string>& followSet = pair.second;
        cout << symbol << " -> { ";
        for (const string& s : followSet) cout << s << " ";
        cout << "}\n";
    }

    return 0;
}