#include <Lz/Repeat.hpp>
#include <vector>
#include <iostream>


int main() {
    auto toRepeat = 155;
    auto amount = 4;

    for (int i : lz::repeat(toRepeat, amount)) {
        std::cout << i << '\n';
    }
    // Yields (by reference if '&' is used):
    // 155
    // 155
    // 155
    // 155
}