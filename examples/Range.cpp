#include <Lz/Range.hpp>
#include <vector>
#include <iostream>


int main() {
    for (int i : lz::range(3)) {
        std::cout << i << '\n';
    }
    // Yields: (by value)
    // 0
    // 1
    // 2
}