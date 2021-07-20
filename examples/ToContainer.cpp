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

    std::vector<char> copyToVec;
    using lz::distance; using std::distance; // optional
    copyToVec.reserve(distance(generator.begin(), generator.end())); // optional
    generator.copyTo(std::back_inserter(copyToVec));
    // copyToVec = {a, b, c, d }

    std::vector<int> transformToVec;
    using lz::distance; using std::distance; // optional
    transformToVec.reserve(distance(generator.begin(), generator.end())); // optional
    generator.transformTo(std::back_inserter(transformToVec), [](const char i) {
        return i + 1;
    });
    // transformToVec = {b, c, d, e}

    // Must match a constructor of template parameter Container (in this case std::vector)
    c = 'a';
#ifdef LZ_HAS_CXX_17
    // C++17
    auto cpy = generator.to<std::vector<long>>(std::execution::seq, 100); // cpy = std::vector<long> {97, 98, 99, 100, 0, 0, ...} with size() = 100
#else
    // pre c++17
    auto cpy = generator.to<std::vector<long>>(100); // cpy = std::vector<long> {97, 98, 99, 100, 0, 0, ...} with size() = 100
#endif
}