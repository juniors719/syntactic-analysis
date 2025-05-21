#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

// Definição da estrutura StateItem
struct StateItem {
    std::string lhs;
    std::vector<std::string> rhs;
    size_t dot;

    StateItem(std::string lhs, std::vector<std::string> rhs, size_t dot = 0)
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

// Hash personalizado para usar StateItem em unordered_set
namespace std {
template <>
struct hash<StateItem> {
    size_t operator()(const StateItem& item) const {
        size_t h1 = hash<std::string>()(item.lhs);
        size_t h2 = item.rhs.size();
        for (const std::string& s : item.rhs) {
            h2 ^= hash<std::string>()(s) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
        }
        size_t h3 = hash<size_t>()(item.dot);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
}  // namespace std

// Declarações das funções do autômato
std::unordered_set<StateItem> closure(
    const std::unordered_set<StateItem>& items,
    const std::map<std::string, std::vector<std::vector<std::string>>>&
        grammar);

std::unordered_set<StateItem> goto_state(
    const std::unordered_set<StateItem>& items, const std::string& symbol,
    const std::map<std::string, std::vector<std::vector<std::string>>>&
        grammar);

int build_LR0_automaton(
    const std::map<std::string, std::vector<std::vector<std::string>>>& grammar,
    const std::string& start_symbol,
    std::vector<std::unordered_set<StateItem>>& states,
    std::map<std::pair<int, std::string>, int>& transitions);

void run_automaton();

#endif