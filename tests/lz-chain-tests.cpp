#include "Lz/Lz.hpp"
#include <catch2/catch.hpp>

template class lz::IterView<lz::Take<std::vector<int>::iterator>::iterator>;

TEST_CASE("Iterator chaining") {
    constexpr int size = 16;
    std::array<int, size> arr = lz::range(size).toArray<size>();
    std::array<int, size> arr2 = lz::range(size).toArray<size>();

    std::function<int&(int&)> f = [](int& i) -> int& { return i; };
    std::function<int(std::tuple<int, int> tup)> tupFn = [](std::tuple<int, int> tup) { return std::get<0>(tup); };

    int result = lz::toIter(arr)
        .as<int&>()
        .map(std::move(f))
        .forEach([x = 0](int& i) mutable { i = x++;})
        .zip(arr2)
        .map(std::move(tupFn))
        .concat(arr2)
        .foldl(0, [](int acc, int next) { return acc + next; });

    CHECK(result == 240);

    arr = lz::repeat(1, size).toArray<size>();
    bool isAllSame = lz::toIter(arr)
        .take(size)
        .drop(0)
        .takeWhile([](int i) { return i == 1; })
        .dropWhile([](int i) { return i != 1; })
        .sort()
        .enumerate()
        .all([](auto&& i) { return i.second == 1; });

    CHECK(isAllSame);

}
