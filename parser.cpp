#include <iostream>
#include <stack>

#include "automata.hpp"
using namespace std;

map<string, vector<vector<string>>> grammar;
vector<unordered_set<StateItem>> states;
map<pair<int, string>, int> transitions;
int final_state{};

bool parse(const vector<string>& input_tokens, int final_state) {
    stack<int> state_stack;
    stack<string> symbol_stack;
    state_stack.push(0);

    vector<string> tokens = input_tokens;
    tokens.push_back("$");

    size_t index = 0;
    while (index < tokens.size()) {
        int current_state = state_stack.top();
        string current_token = tokens[index];
        stack<int> temp_states = state_stack;
        while (!temp_states.empty()) {
            temp_states.pop();
        }
        stack<string> temp_symbols = symbol_stack;
        while (!temp_symbols.empty()) {
            temp_symbols.pop();
        }

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
                if (item.lhs == "S'" && item.rhs.size() == 1 && item.rhs[0] == "S") {
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

    bool accepted = (state_stack.top() == final_state && symbol_stack.top() == "S'");
    return accepted;
}

int main() {
    grammar["S'"] = {{"S", "$"}};
    grammar["S"] = {
        {"(", "L", ")"},
        {"x"}};
    grammar["L"] = {
        {"S"},
        {"L", ",", "S"}};

    final_state = build_LR0_automaton(grammar, "S", states, transitions);

    vector<vector<string>> test_cases = {
        // Válidos
        {"x"},
        {"(", "x", ")"},
        {"(", "x", ",", "x", ")"},
        {"(", "x", ",", "x", ",", "x", ")"},
        {"(", "(", "x", ")", ")"},
        {"(", "x", ",", "(", "x", ")", ")"},

        // Inválidos
        {"x", "x"},
        {"(", "x"},
        {"x", ")"},
        {"(", ",", "x", ")"},
        {"(", "x", ",", ")"},
        {"(", "x", ",", ",", "x", ")"},
        {"(", ")"},
        {"x", "(", ",", "x", ")"},
        {",", ",", ",", ","}};

    for (const auto& test_case : test_cases) {
        cout << (parse(test_case, final_state) ? "Sucesso" : "Erro Sintático") << "\n";
    }

    return 0;
}