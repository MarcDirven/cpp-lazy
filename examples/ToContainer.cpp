#include <Lz/Generate.hpp>
#include <iostream>
#include <list>
#include <set>
#include <vector>

#ifdef LZ_HAS_EXECUTION
#    include <execution>
#endif

int main() {
    auto generator = lz::generate(
        [](char& c) {
            auto tmp = c;
            ++c;
            return tmp;
        },
        4, 'a');

    // To vector:
    auto vec = generator.to_vector();
    for (char val : vec) {
        std::cout << val << '\n';
    }
    // Yields:
    // a
    // b
    // c
    // d
    std::cout << '\n';

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

    // To list
    auto list = generator.to<std::list<char>>();
    for (char val : set) {
        std::cout << val << '\n';
    }
    // Yields:
    // a
    // b
    // c
    // d
    std::cout << '\n';

    // To map
    std::map<char, char> map = generator.to_map([](const char c) { return std::make_pair(static_cast<char>(c + 1), c); });
    for (std::pair<char, char> pair : map) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    // Yields:
    // b a
    // c b
    // d c
    // e d

    std::vector<char> copy_to_vec;
    generator.copy_to(std::back_inserter(copy_to_vec));
    // copy_to_vec = {a, b, c, d }

    std::vector<int> transform_to_vec;
    generator.transform_to(std::back_inserter(transform_to_vec), [](const char i) { return i + 1; });
    // transform_to_vec = {b, c, d, e}

    auto out = generator.transform_as<std::vector<char>>([](char c) -> char { return c + 1; });
    for (char val : out) {
        std::cout << val << '\n';
    }
    // yields:
    // b
    // c
    // d
    // e
    
}