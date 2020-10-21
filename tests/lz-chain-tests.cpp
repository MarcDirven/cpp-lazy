#include "Lz/Lz.hpp"
#include <catch2/catch.hpp>



TEST_CASE("Iterator chaining") {
    constexpr int size = 1024;
    std::array<int, size> arr = lz::range(size).toArray<size>();
    std::array<int, size> arr2 = lz::range(size).toArray<size>();
//
//    auto mapper = lz::map(arr, [=](int i)  { return i;});
//    auto mapMany = lz::mapMany([](int, int) { return 1; }, mapper, arr2);
//    auto filter = lz::filter(mapper, [=](int) { return true; });
    lz::toIter(arr)
        .map([](int i){ return i; })
        /*.foldl(0, [](int acc, int next) { return acc + next; })*/;

    fmt::print("{}\n", "h");
}
