#include <iostream>
#include <string>

#include "automata.hpp"
#include "file_reader.hpp"
#include "first_follow.hpp"
#include "parser.hpp"

using namespace std;

void print_usage() {
    cout << "Uso:" << endl;
    cout << "./main.exe first_follow # Calcula FIRST e FOLLOW" << endl;
    cout << "./main.exe automaton    # Gera o autômato LR(0)" << endl;
    cout << "./main.exe parse        # Executa o parser LR(0)" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    string command = argv[1];

    if (command == "first_follow") {
        run_first_follow();
    } else if (command == "automaton") {
        run_automaton();
    } else if (command == "parse") {
        run_parser();
    } else {
        cout << "Comando inválido: " << command << endl;
        print_usage();
        return 1;
    }

    return 0;
}