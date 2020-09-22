#include <Lz/Take.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> seq = {1, 2, 3, 4, 5, 6};
    auto takeWhile = lz::takeWhile(seq, [](const int i) { return i != 4; });
    for (int i : takeWhile) {
        std::cout << i << '\n';
    }
    // Yields (by reference if '& is used):
    // 1
    // 2
    // 3
    std::cout << '\n';

    size_t amount = 2;
    auto take = lz::take(seq, amount);
    for (int i : take) {
        std::cout << i << '\n';
    }
    // Yields (by reference if '& is used):
    // 1
    // 2
    std::cout << '\n';

    auto slice = lz::slice(seq, 1, 4);
    for (int i : slice) {
        std::cout << i << '\n';
    }
    // Yields (by reference if '& is used):
    // 2
    // 3
    // 4
}