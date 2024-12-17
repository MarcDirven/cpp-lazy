#include <Lz/TakeEvery.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> sequence = {1, 2, 3, 4, 5};
    const auto take_every = lz::take_every(sequence, 2);
    std::cout << take_every << '\n';
    // Output: 1 3 5

    for (int i : take_every) {
        // process i...
    }
}