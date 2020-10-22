#include "Lz/Lz.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Iterator chaining") {
    constexpr int size = 16;
    std::array<int, size> arr = lz::range(size).toArray<size>();
    std::array<int, size> arr2 = lz::range(size).toArray<size>();
    int result = lz::toIter(arr)
        .as<int&>()
        .map([](int& i) -> int& { return i; })
        .forEach([x = 0](int& i) mutable { i = x++;})
        .zip(arr2)
        .map([](std::tuple<int, int> tup) { return std::get<0>(tup); })
        .concat(arr2)
        .foldl(0, [](int acc, int next) { return acc + next; });

    CHECK(result == 240);

    arr = lz::repeat(1, size).toArray<size>();
    bool isAllSame = lz::toIter(arr)
        .affirm(std::string("Sequence is not 1"), [](int i) { return i == 1; })
        .take(size)
        .takeWhile([](int i) { return i == 1; })
        .dropWhile([](int i) { return i != 1; })
        .enumerate()
        .all([](std::pair<int, int> i) { return i.second == 1; });

    CHECK(isAllSame);
}
