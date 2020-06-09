#include <iostream>

#include "EnumerateIterator.hpp"
#include "RangeIterator.hpp"
#include "FilterIterator.hpp"
#include "SplitIterator.hpp"
#include "MapIterator.hpp"
#include "ZipIterator.hpp"
#include "TakeIterator.hpp"


bool is(int i) {
    return i != 10;
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);

    constexpr int size = 32;
    auto array = lz::range(size).toArray<static_cast<size_t>(size)>();

    auto filtered = lz::takewhile(array, is);
    auto take = lz::take(filtered, 5);

    for (int i : take) {
        std::cout << i << '\n';
    }
}
