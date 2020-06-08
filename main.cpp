#include <iostream>

#include "RangeIterator.hpp"
#include "FilterIterator.hpp"
#include "SplitIterator.hpp"
#include "MapIterator.hpp"
#include "ZipIterator.h"


struct Test {
    int a{}, b{};
    using value_type = int;
};

int fn(const Test& test) {
    return test.a;
}


int main(int argc, char** argv) {
    std::string string("a  b  ces  d  e");
    std::string delimiter("  ");
    auto splitObject = lz::split(string, std::move(delimiter));

    for (auto& x : splitObject) {
        std::cout << x << '\n';
    }

    std::vector<Test> x{Test{2, 3}, Test{3, 4}, Test{5, 9}};
    auto mapped = lz::map(x, [](auto& x) {
        return x.a;
    }).toVector();


    std::array<int, 2> a{};
    const std::array<float, 3> b{};
    std::array<int, 3> c{};
    for (auto z : lz::zip(a, b, c)) {

    }

}
