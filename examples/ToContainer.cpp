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

    std::vector<char> copyToVec;
    generator.copyTo(std::back_inserter(copyToVec));
    // copyToVec = {a, b, c, d }

    std::vector<int> transformToVec;
    generator.transformTo(std::back_inserter(transformToVec), [](const char i) { return i + 1; });
    // transformToVec = {b, c, d, e}

    // Must match a constructor of template parameter Container (in this case std::vector)
    // C++17
    // auto cpy = generator.to<std::vector<long>>(std::execution::seq, 100); // cpy = std::vector<long> {97, 98, 99, 100, 0, 0,
    // ...} with size() = 100 pre c++17
#ifdef LZ_HAS_EXECUTION
    auto cpy = generator.to<std::vector<long>>(std::execution::seq,
                                               100); // cpy = std::vector<long> {97, 98, 99, 100, 0, 0, ...} with size() = 100
#else
    auto cpy = generator.to<std::vector<long>>(100); // cpy = std::vector<long> {97, 98, 99, 100, 0, 0, ...} with size() = 100
#endif
}