#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

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

namespace std {
template <>
struct hash<StateItem> {
    size_t operator()(const StateItem& item) const {
        size_t h1 = hash<string>()(item.lhs);
        size_t h2 = item.rhs.size();
        for (const string& s : item.rhs) {
            h2 ^= hash<string>()(s) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
        }
        size_t h3 = hash<size_t>()(item.dot);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
}  // namespace std

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