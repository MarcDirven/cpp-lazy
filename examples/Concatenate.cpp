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
}