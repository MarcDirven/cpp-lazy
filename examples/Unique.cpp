#include <Lz/Unique.hpp>
#include <iostream>

int main() {
    std::vector<int> vector = {5, 3, 2, 5, 6, 42, 2, 3, 56, 3, 1, 12, 3};
    // Operator== and operator< are required
    auto unique = lz::unique(vector);

    for (int i : vector) {
        std::cout << i << '\n';
    }
    // prints
    // 1
    // 2
    // 3
    // 4
    // 5
    // 6
    // 12
    // 42
    // 56
}