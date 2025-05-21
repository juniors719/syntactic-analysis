#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <unordered_set>

#include "automata.hpp"
#include "file_reader.hpp"
#include "parser.hpp"
#include "globals.hpp"

using namespace std;

vector<unordered_set<StateItem>> states;
map<pair<int, string>, int> transitions;
int final_state{};

// Função parse
bool parse(const vector<string>& input_tokens, int final_state, const string& start_symbol) {
    stack<int> state_stack;
    stack<string> symbol_stack;
    state_stack.push(0);

    vector<string> tokens = input_tokens;
    tokens.push_back("$");

    size_t index = 0;
    while (index < tokens.size()) {
        int current_state = state_stack.top();
        string current_token = tokens[index];

        if (current_token == "$" && current_state == final_state) {
            return true;
        }

        auto shift_key = make_pair(current_state, current_token);
        if (transitions.find(shift_key) != transitions.end()) {
            int next_state = transitions.at(shift_key);
            state_stack.push(next_state);
            symbol_stack.push(current_token);
            index++;
            continue;
        }

        bool reduced = false;
        for (const auto& item : states[current_state]) {
            if (item.dot == item.rhs.size()) {
                if (item.lhs == start_symbol + "'" && item.rhs.size() == 1 && item.rhs[0] == start_symbol) {
                    return true;
                }

                size_t reduce_count = item.rhs.size();
                for (size_t i = 0; i < reduce_count; i++) {
                    if (state_stack.empty() || symbol_stack.empty()) {
                        return false;
                    }
                    state_stack.pop();
                    symbol_stack.pop();
                }

                current_state = state_stack.top();
                string lhs = item.lhs;
                auto goto_key = make_pair(current_state, lhs);

                if (transitions.find(goto_key) != transitions.end()) {
                    int goto_state = transitions.at(goto_key);
                    state_stack.push(goto_state);
                    symbol_stack.push(lhs);
                    reduced = true;
                    break;
                } else {
                    return false;
                }
            }
        }

        if (!reduced) {
            return false;
        }
    }

    return (state_stack.top() == final_state && symbol_stack.top() == start_symbol + "'");
}

void run_parser() {
    load_grammar("grammar.txt", grammar);
    final_state = build_LR0_automaton(grammar, "S", states, transitions);

    auto test_cases = load_test_cases("test_cases.txt");

    for (const auto& test_case : test_cases) {
        cout << (parse(test_case, final_state, "S") ? "Sucesso" : "Erro Sintático") << "\n";
    }
}