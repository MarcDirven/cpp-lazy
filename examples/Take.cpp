#include <Lz/Take.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> seq = { 1, 2, 3, 4, 5, 6 };

    auto taken = lz::take(seq, 3);
fmt::print("{}\n", taken); // output: 1, 2, 3

    auto dropped = lz::drop(seq, 3);
    fmt::print("{}\n", dropped); // output: 4, 5, 6

    auto sliced = lz::slice(seq, 1, 4);
    fmt::print("{}\n", sliced); // output: 2, 3, 4
}