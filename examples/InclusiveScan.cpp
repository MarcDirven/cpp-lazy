#include <Lz/InclusiveScan.hpp>

#include <iostream>

int main() {
    int array[] = {3, 5, 2, 3, 4, 2, 3};
    // start the scan from arr[0] (3), with init 0
    auto scan = lz::iScan(array);

    for (const int& i : scan) {
        fmt::print("{} ", i);
    }
    // prints 3 8 10 13 17 19 22

    // essentially it's:
    // 3 + 0 (3)
    // 3 + 5 (8)
    // 8 + 2 (10)
    // 10 + 3 (13)
    // 13 + 4 (17)
    // 17 + 2 (19)
    // 19 + 3 (22)
}