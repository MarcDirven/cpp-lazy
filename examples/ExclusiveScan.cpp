#include <Lz/exclusive_scan.hpp>

#include <iostream>

int main() {
    int array[] = {3, 5, 2, 3, 4, 2, 3};
    // start the scan from 0
    auto scan = lz::exclusive_scan(array, 0);

    for (const int& i : scan) {
        fmt::print("{} ", i);
    }
    // prints 0 3 8 10 13 17 19

    // essentially it's:
    // 0 (0)
    // 0 + 3 (3)
    // 3 + 5 (8)
    // 8 + 2 (10)
    // 10 + 3 (13)
    // 13 + 4 (17)
    // 17 + 2 (19)
}