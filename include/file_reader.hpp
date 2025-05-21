#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <map>
#include <string>
#include <vector>

std::string load_grammar(
    const std::string& filename,
    std::map<std::string, std::vector<std::vector<std::string>>>& grammar);

std::vector<std::vector<std::string>> load_test_cases(
    const std::string& filename);

#endif