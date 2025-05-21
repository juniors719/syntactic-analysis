#include "file_reader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

std::string load_grammar(const string& filename,
                         map<string, vector<vector<string>>>& grammar) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo de gramática: " << filename << endl;
        exit(1);
    }

    string line;
    bool first_line = true;
    string start_symbol;

    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string lhs, arrow, token;
        iss >> lhs >> arrow;

        if (first_line) {
            start_symbol = lhs;
            first_line = false;
        }

        vector<string> rhs;
        while (iss >> token) {
            rhs.push_back(token);
        }

        grammar[lhs].push_back(rhs);
    }
    file.close();

    return start_symbol;
}

vector<vector<string>> load_test_cases(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo de casos de teste: " << filename
             << endl;
        exit(1);
    }

    vector<vector<string>> test_cases;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);
        string token;
        vector<string> case_tokens;

        while (iss >> token) {
            case_tokens.push_back(token);
        }

        test_cases.push_back(case_tokens);
    }
    file.close();
    return test_cases;
}