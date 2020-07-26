#include <Lz/Generate.hpp>
#include <vector>
#include <iostream>


int main() {
    int myIncreasingCounter = 0;
    constexpr int amount = 4;
    auto generator = [&myIncreasingCounter]() {
        return myIncreasingCounter++;
    };

    for (int incrementer : lz::generate(generator, amount)) {
        std::cout << incrementer << '\n';
    }
    // yields (by value):
    // 0
    // 1
    // 2
    // 3
}