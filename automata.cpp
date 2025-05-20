#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;

map<string, vector<vector<string>>> grammar;

struct StateItem {
    string lhs;
    vector<string> rhs;
    size_t dot;

    StateItem(string lhs, vector<string> rhs, size_t dot = 0)
        : lhs(lhs), rhs(rhs), dot(dot) {}

    bool operator<(const StateItem& other) const {
        if (lhs != other.lhs) return lhs < other.lhs;
        if (dot != other.dot) return dot < other.dot;
        return rhs < other.rhs;
    }

    bool operator==(const StateItem& other) const {
        return lhs == other.lhs && dot == other.dot && rhs == other.rhs;
    }
};

set<StateItem> closure(const set<StateItem>& items, const map<string, vector<vector<string>>>& grammar) {
    set<StateItem> closure_set = items;
    bool changed = true;

    do {
        changed = false;
        set<StateItem> newItems;

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

set<StateItem> goto_state(const set<StateItem>& items, const string& symbol,
                          const map<string, vector<vector<string>>>& grammar) {
    set<StateItem> new_items;

    for (const auto& item : items) {
        if (item.dot < item.rhs.size() && item.rhs[item.dot] == symbol) {
            StateItem new_item(item.lhs, item.rhs, item.dot + 1);
            new_items.insert(new_item);
        }
    }

    return closure(new_items, grammar);
}

void build_LR0_automaton(const map<string, vector<vector<string>>>& grammar,
                         const string& start_symbol,
                         vector<set<StateItem>>& states,
                         map<pair<int, string>, int>& transitions) {
    vector<string> start_rhs = {start_symbol, "$"};
    set<StateItem> initial_state = closure({StateItem("S'", start_rhs)}, grammar);

    states.push_back(initial_state);
    transitions.clear();

    bool changed;
    do {
        changed = false;
        size_t current_size = states.size();

        for (size_t i = 0; i < current_size; i++) {
            const auto& state = states[i];

            set<string> symbols_after_dots;
            for (const auto& item : state) {
                if (item.dot < item.rhs.size()) {
                    symbols_after_dots.insert(item.rhs[item.dot]);
                }
            }

            for (const string& symbol : symbols_after_dots) {
                if (symbol == "$") continue;
                set<StateItem> new_state = goto_state(state, symbol, grammar);

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
}

int main() {
    grammar["S'"] = {{"S"}};
    grammar["S"] = {
        {"(", "L", ")"},
        {"x"}};
    grammar["L"] = {
        {"S"},
        {"L", ",", "S"}};

    vector<set<StateItem>> states;
    map<pair<int, string>, int> transitions;

    build_LR0_automaton(grammar, "S", states, transitions);

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

    return 0;
}