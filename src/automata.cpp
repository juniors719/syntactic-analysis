#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "file_reader.hpp"
#include "globals.hpp"
#include "automata.hpp"

using namespace std;

unordered_set<StateItem> closure(const unordered_set<StateItem>& items, const map<string, vector<vector<string>>>& grammar) {
    unordered_set<StateItem> closure_set = items;
    bool changed = true;

    do {
        changed = false;
        unordered_set<StateItem> newItems;

        for (const auto& item : closure_set) {
            if (item.dot >= item.rhs.size()) continue;

            string nextSymbol = item.rhs[item.dot];
            if (grammar.count(nextSymbol)) {
                for (const auto& production : grammar.at(nextSymbol)) {
                    StateItem newItem(nextSymbol, production);
                    if (closure_set.find(newItem) == closure_set.end() &&
                        newItems.find(newItem) == newItems.end()) {
                        newItems.insert(newItem);
                        changed = true;
                    }
                }
            }
        }
        closure_set.insert(newItems.begin(), newItems.end());
    } while (changed);
    return closure_set;
}

unordered_set<StateItem> goto_state(const unordered_set<StateItem>& items, const string& symbol,
                                    const map<string, vector<vector<string>>>& grammar) {
    unordered_set<StateItem> new_items;

    for (const auto& item : items) {
        if (item.dot < item.rhs.size() && item.rhs[item.dot] == symbol) {
            StateItem new_item(item.lhs, item.rhs, item.dot + 1);
            new_items.insert(new_item);
        }
    }

    return closure(new_items, grammar);
}

int build_LR0_automaton(const map<string, vector<vector<string>>>& grammar,
                        const string& start_symbol,
                        vector<unordered_set<StateItem>>& states,
                        map<pair<int, string>, int>& transitions) {
    vector<string> start_rhs = {start_symbol, "$"};
    unordered_set<StateItem> initial_state = closure({StateItem(start_symbol + "'", start_rhs)}, grammar);

    states.push_back(initial_state);
    transitions.clear();

    bool changed;
    do {
        changed = false;
        size_t current_size = states.size();

        for (size_t i = 0; i < current_size; i++) {
            const auto& state = states[i];

            unordered_set<string> symbols_after_dots;
            for (const auto& item : state) {
                if (item.dot < item.rhs.size()) {
                    symbols_after_dots.insert(item.rhs[item.dot]);
                }
            }

            for (const string& symbol : symbols_after_dots) {
                if (symbol == "$") continue;
                unordered_set<StateItem> new_state = goto_state(state, symbol, grammar);

                if (new_state.empty()) continue;

                auto it = find(states.begin(), states.end(), new_state);
                int target_state;

                if (it == states.end()) {
                    states.push_back(new_state);
                    target_state = states.size() - 1;
                    changed = true;
                } else {
                    target_state = distance(states.begin(), it);
                }

                pair<int, string> key = {static_cast<int>(i), symbol};
                if (transitions.find(key) == transitions.end()) {
                    transitions[key] = target_state;
                }
            }
        }

    } while (changed);
    for (size_t i = 0; i < states.size(); i++) {
        for (const auto& item : states[i]) {
            if (item.lhs == start_symbol + "'" &&
                item.dot == item.rhs.size() - 1 &&
                !item.rhs.empty() && item.rhs.back() == "$") {
                // cout << "Estado de aceitação encontrado: " << i << "\n";
                return i;
            }
        }
    }
    return -1;
}

void run_automaton() {
    load_grammar("grammar.txt", grammar);

    vector<unordered_set<StateItem>> states;
    map<pair<int, string>, int> transitions;

    int last = build_LR0_automaton(grammar, "S", states, transitions);

    cout << "Estados LR(0):\n";
    for (size_t i = 0; i < states.size(); i++) {
        cout << "Estado " << i << ":\n";
        for (const auto& item : states[i]) {
            cout << "  " << item.lhs << " -> ";
            for (size_t j = 0; j < item.rhs.size(); j++) {
                if (j == item.dot) cout << ". ";
                cout << item.rhs[j] << " ";
            }
            if (item.dot == item.rhs.size()) cout << ". ";
            cout << "\n";
        }
        cout << "\n";
    }

    cout << "Transições:\n";
    for (const auto& trans : transitions) {
        cout << "δ(" << trans.first.first << ", " << trans.first.second
             << ") = " << trans.second << "\n";
    }

    cout << "Estado de aceitação: " << last << "\n";
}