#include <Lz/Random.hpp>
#include <vector>
#include <iostream>


int main() {
    float min = 0;
    float max = 1;
    size_t amount = 4;
    auto rng = lz::random(min, max, amount);

    for (float f : rng) {
        std::cout << f << '\n';
    }
    // Yields (by value):
    // random number between [0, 1]
    // random number between [0, 1]
    // random number between [0, 1]
    // random number between [0, 1]
}