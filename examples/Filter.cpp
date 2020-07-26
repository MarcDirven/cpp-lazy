#include <Lz/Filter.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> toFilter = {1, 2, 3, 4, 5, 6};
    auto filter = lz::filter(toFilter, [](const int i) { return i % 2 == 0; });

    for (int i : filter) {
        std::cout << i << '\n';
    }
    // yields (container element by reference if '&' is used):
    // 2
    // 4
    // 6
}