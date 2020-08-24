#include <Lz/Generate.hpp>
#include <vector>
#include <set>
#include <list>
#include <iostream>


int main() {
    char c = 'a';
    auto generator = lz::generate([&c]() {
        return c++;
    }, 4);

    // To vector:
    auto vec = generator.toVector();
    for (char val : vec) {
        std::cout << val << '\n';
    }
    // Yields:
    // a
    // b
    // c
    // d
    std::cout << '\n';
    c = 'a';

    // To set
    auto set = generator.to<std::set>();
    for (char val : set) {
        std::cout << val << '\n';
    }
    // Yields:
    // a
    // b
    // c
    // d
    std::cout << '\n';
    c = 'a';

    // To list
    auto list = generator.to<std::list>();
    for (char val : set) {
        std::cout << val << '\n';
    }
    // Yields:
    // a
    // b
    // c
    // d
    std::cout << '\n';
    c = 'a';

    // To map
    std::map<char, char> map = generator.toMap([](const char c) { return static_cast<char>(c + 1); });
    for (std::pair<char, char> pair : map) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    // Yields:
    // b a
    // c b
    // d c
    // e d
}