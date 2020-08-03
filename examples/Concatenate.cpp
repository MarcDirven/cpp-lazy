#include <Lz/Concatenate.hpp>
#include <iostream>


int main() {
    std::array<int, 4> a{1, 2, 3, 4};
    std::array<int, 4> b{5, 6, 7, 8};

    for (int i : lz::concat(a, b)) {
        std::cout << i << '\n';
    }

    // Output:
    // 1
    // 2
    // 3
    // 4
    // 5
    // 6
    // 7
    // 8

    std::cout << '\n';

    std::array<int, 4> c{9, 10, 11, 12};
    std::array<int, 4> d{13, 14, 15, 16};

    for (int i : lz::concat(a, b, c, d)) {
        std::cout << i << '\n';
    }

    // Output:
    // 1
    // 2
    // 3
    // 4
    // 5
    // 6
    // 7
    // 8
    // 9
    // 10
    // 11
    // 12
    // 13
    // 14
    // 15
    // 16
}