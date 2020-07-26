#include <Lz/Except.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::vector<int> toExcept = {2, 3, 4};

    for (int i : lz::except(values, toExcept)) {
        std::cout << i << '\n';
    }
    // yields (container element by reference if '&' is used):
    // 1
    // 5
}