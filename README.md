# SYNTACTIC-ANALYSIS
Trabalho da cadeira de Compiladores na UFC - Campus Quixadá, lecionada pelo professor Lucas Ismaily no semestre de 2025.1. Realizado pelos alunos:
- Francisco Djalma Pereira da Silva Júnior - 554222 
- Francisco Leudes Bezerra Neto - 552478
- Kauan Pablo de Sousa Silva - 556027

## Descrição do Projeto
O trabalho consiste em criar um analisador sintático para resolver 3 problemas:
1. Calcular first e follow de uma gramática.
2. Criar um autômato de parsing LR(0) para uma gramática.
3. Criar um algoritmo de reconhecimento de uma string em uma gramática.

## Como compilar o projeto
Primeiro é necessário ter o compilador g++ instalado com suporte a C++11 ou superior. 

Compile usando o makefile onde o executável `syntactic_analysis` será gerado na raiz do projeto.
```bash
make
```
Ou utilize o comando abaixo para compilar o código diretamente onde o executável `main` será gerado na raiz do projeto.
```bash
g++ -std=c++11 -Iinclude main.cpp src/automata.cpp src/file_reader.cpp src/first_follow.cpp src/globals.cpp src/parser.cpp -o main
```

## Como executar o projeto
Dentro da pasta data, você encontrará um arquivo chamado `grammar.txt`, que contém a gramática a ser analisada e outro chamado `test_cases.txt`, que contém as strings a serem reconhecidas. 

Após compilar o projeto, execute o seguinte comando na raiz do projeto passando o argumento desejado:
```bash
[executável] first_follow # Calcula FIRST e FOLLOW
```
```bash
[executável] automaton    # Gera o autômato LR(0)
```
```bash
[executável] parse        # Executa o parser LR(0)
```

O resultado será impresso no terminal de acordo com o que foi solicitado.