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

unordered_set<StateItem> closure(const unordered_set<StateItem>& items, const map<string, vector<vector<string>>>& grammar_param) {
    unordered_set<StateItem> closure_set = items;
    bool changed = true;

    do {
        changed = false;
        unordered_set<StateItem> newItems;

        for (const auto& item : closure_set) {
            if (item.dot >= item.rhs.size()) continue;

            string nextSymbol = item.rhs[item.dot];
            if (grammar_param.count(nextSymbol)) { 
                for (const auto& production : grammar_param.at(nextSymbol)) { 
                    StateItem newItem(nextSymbol, production);
                    if (closure_set.find(newItem) == closure_set.end() &&
                        newItems.find(newItem) == newItems.end()) {
                        newItems.insert(newItem);
                        changed = true;
                    }
                }
            }
        }
        if (!newItems.empty()) {
            closure_set.insert(newItems.begin(), newItems.end());
        }
    } while (changed);

   return closure_set;
}

unordered_set<StateItem> goto_state(const unordered_set<StateItem>& items, const string& symbol,
                                    const map<string, vector<vector<string>>>& grammar_param) { 
    unordered_set<StateItem> new_items;

    for (const auto& item : items) {
        if (item.dot < item.rhs.size() && item.rhs[item.dot] == symbol) {
            StateItem new_item(item.lhs, item.rhs, item.dot + 1);
            new_items.insert(new_item);
        }
    }
    unordered_set<StateItem> result_closure = closure(new_items, grammar_param);
    return result_closure;
}

int build_LR0_automaton(const map<string, vector<vector<string>>>& grammar_param, 
                        const string& start_symbol,
                        vector<unordered_set<StateItem>>& states_ref,         
                        map<pair<int, string>, int>& transitions_ref) {    
    states_ref.clear();
    transitions_ref.clear(); 

    vector<string> start_rhs = {start_symbol, "$"};
    std::string augmented_lhs = start_symbol + "'";
    StateItem temp_item_for_closure(augmented_lhs, start_rhs);
    std::unordered_set<StateItem> items_for_closure = {temp_item_for_closure};
    unordered_set<StateItem> initial_state = closure(items_for_closure, grammar_param); 

    states_ref.push_back(initial_state);

    bool changed; 
    do {
        changed = false;
        size_t current_states_count = states_ref.size(); 

        for (size_t i = 0; i < current_states_count; i++) {
            const auto& current_processing_state = states_ref[i];

            unordered_set<string> symbols_after_dots;
            for (const auto& item : current_processing_state) {
                if (item.dot < item.rhs.size()) symbols_after_dots.insert(item.rhs[item.dot]);
            }
            for (const string& symbol_to_goto : symbols_after_dots) {
                if (symbol_to_goto == "$") continue;
                
                unordered_set<StateItem> new_potential_state = goto_state(current_processing_state, symbol_to_goto, grammar_param);

                if (new_potential_state.empty()) continue;

                auto it = find(states_ref.begin(), states_ref.end(), new_potential_state);
                int target_state_idx;

                if (it == states_ref.end()) {
                    states_ref.push_back(new_potential_state);
                    target_state_idx = states_ref.size() - 1;
                    changed = true;
                } else {
                    target_state_idx = distance(states_ref.begin(), it);
                }

                pair<int, string> transition_key = {static_cast<int>(i), symbol_to_goto};
                if (transitions_ref.find(transition_key) == transitions_ref.end()) {
                    transitions_ref[transition_key] = target_state_idx;
                }
            }
        }
    } while (changed);

    for (size_t i = 0; i < states_ref.size(); i++) {
        for (const auto& item : states_ref[i]) {
            if (item.lhs == start_symbol + "'" &&
                item.dot == item.rhs.size() - 1 &&
                !item.rhs.empty() && item.rhs.back() == "$") {
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
            cout << "   " << item.lhs << " -> ";
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