#include <Lz/TakeEvery.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> sequence = {1, 2, 3, 4, 5};

    for (int i : lz::takeEvery(sequence, 2)) {
        std::cout << i << '\n';
    }
    // Yields (by reference if '& is used):
    // 1
    // 3
    // 5
}