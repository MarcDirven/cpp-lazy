#include <Lz/Enumerate.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> toEnumerate = {1, 2, 3, 4, 5};

    for (std::pair<int, int> pair : lz::enumerate(toEnumerate)) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    // yields:
    // [(index element)by value] [(container element) by reference (if '&' is used)]
    //          0                               1
    //          1                               2
    //          2                               3
    //          3                               4
    //          4                               5
}