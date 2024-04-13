#include "Lz/Lz.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>


template class lz::IterView<decltype(std::declval<std::vector<int>&>().begin())>;

TEST_CASE("Iterator chaining") { 
    constexpr int size = 16;
    std::array<int, size> arr = lz::range(size).toArray<size>();
    std::array<int, size> arr2 = lz::range(size).toArray<size>();

    std::function<int&(int&)> f = [](int& i) -> int& {
        return i;
    };
    std::function<int(std::tuple<int, int> tup)> tupFn = [](std::tuple<int, int> tup) {
        return std::get<0>(tup);
    };
    unsigned x = 0;
    auto result = lz::chain(arr)
                      .as<int&>()
                      .map(std::move(f))
                      .forEach([x](int& i) mutable { i = static_cast<int>(x++); })
                      .zip(arr2)
                      .map(std::move(tupFn))
                      .concat(arr2)
                      .foldl(0, std::plus<int>()); // NOLINT

    CHECK(result == 240);

    arr = lz::repeat(1, size).toArray<size>();
    bool isAllSame = lz::chain(arr)
                         .take(size)
                         .drop(0)
                         .map([](int& i) -> int& { return i; })
                         .takeWhile([](int i) { return i == 1; })
                         .dropWhile([](int i) { return i != 1; })
                         .sort()
                         .enumerate()
                         .all([](std::pair<int, int> i) { return i.second == 1; });

    CHECK(isAllSame);

    arr = lz::range(size).toArray<size>();

    auto excluded = lz::chain(arr).exclude(0, 1).toArray<size - 1>();
    CHECK(excluded == lz::range(1, size).toArray<size - 1>());

    SECTION("Join") {
        auto joined = lz::chain(arr).join(" ").toString();
        CHECK(joined == "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15");
    }

    SECTION("Slice") {
        auto sliced = lz::chain(arr).slice(0, 5).toArray<5>();
        CHECK(sliced == lz::range(0, 5).toArray<5>());
    }

    SECTION("TakeEvery") {
        auto takeEvery = lz::chain(arr).takeEvery(2).toArray<size / 2>();
        CHECK(takeEvery == std::array<int, 8>{ 0, 2, 4, 6, 8, 10, 12, 14 });
    }

    SECTION("Chunks") {
        auto chunks = lz::chain(arr).chunks(4).toArray<size / 4>();
        std::array<std::array<int, 4>, 4> expected = { { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 8, 9, 10, 11 }, { 12, 13, 14, 15 } } };
#ifndef __cpp_structured_bindings
        for (auto&& combined : lz::zip(chunks, expected)) {
            auto&& chunk = std::get<0>(combined);
            auto&& expectedArr = std::get<1>(combined);
            CHECK(lz::equal(chunk, expectedArr));
        }
#else
        for (auto [chunk, expectedArr] : lz::zip(chunks, expected)) {
            CHECK(std::equal(chunk.begin(), chunk.end(), expectedArr.begin(), expectedArr.end()));
        }
#endif
    }

    SECTION("Zip with") {
        auto zippedWith = lz::chain(arr).zipWith([](int a, int b) { return a + b; }, arr2);
        for (auto&& enumerate : lz::enumerate(zippedWith)) {
            CHECK(enumerate.first + enumerate.first == enumerate.second);
        }
    }

    SECTION("Equal, reverse") {
        auto revved = lz::chain(arr).reverse();
        CHECK(lz::equal(revved, lz::range(15, -1, -1)));
    }

    SECTION("Pairwise, enumerate") {
        for (auto&& tup : lz::enumerate(lz::pairwise(arr))) {
            CHECK(tup.first == std::get<0>(tup.second));
            CHECK(tup.first + 1 == std::get<1>(tup.second));
        }
    }

    SECTION("Trim") {
        std::array<int, 4> arrayToTrim = { 1, 1, 3, 4 };
        CHECK(lz::chain(arrayToTrim).trim([](int i) { return i == 1; }, [](int i) { return i == 4; }).toArray<1>() ==
              std::array<int, 1>{ 3 });
        std::string toTrim = "\t  a b c \t";
        CHECK(lz::trimString(toTrim).toString() == "a b c");
    }

    SECTION("Cartesian") {
        std::array<int, 2> a = { 1, 2 };
        std::array<int, 2> b = { 3, 4 };
        auto cart = lz::chain(a).cartesian(b);
        CHECK(*cart.begin() == std::make_tuple(1, 3));
        CHECK(*std::next(cart.begin()) == std::make_tuple(1, 4));
        CHECK(*std::next(cart.begin(), 2) == std::make_tuple(2, 3));
        CHECK(*std::next(cart.begin(), 3) == std::make_tuple(2, 4));
        CHECK(std::next(cart.begin(), 4) == cart.end());
    }

    SECTION("Flatten") {
        std::array<std::array<int, 3>, 3> toFlatten{};
        CHECK(lz::equal(lz::chain(toFlatten).flatten(), lz::repeat(0, 3 * 3)));
    }

    SECTION("First") {
        CHECK(*lz::chain(arr).next(1) == 1);
    }

    SECTION("first, last, empty, many") {
        CHECK(!lz::chain(arr).empty());
        CHECK(!lz::chain(arr).hasOne());
        CHECK(lz::chain(arr).hasMany());
        CHECK(lz::chain(arr).front() == 0);
        CHECK(lz::chain(arr).back() == 15);

        std::vector<int> v;
        CHECK(lz::chain(v).frontOr(20) == 20);
        CHECK(lz::chain(v).backOr(20) == 20);
    }

    SECTION("Filter") {
        CHECK(lz::chain(arr).filter([](int i) { return i % 2 == 0; }).distance() == 8);
    }

    SECTION("Except") {
        CHECK(lz::chain(arr).except(arr2).distance() == 0);
    }

    SECTION("Unique") {
        CHECK(lz::chain(arr).unique().distance() == size);
    }

    SECTION("ChunkIf") {
        CHECK(lz::chain(arr).chunkIf([](int i) { return i % 2 == 0; }).distance() == 9);
    }

    SECTION("FilterMap") {
        CHECK(lz::chain(arr).filterMap([](int i) { return i % 2 == 0; }, [](int i) { return fmt::to_string(i); }).toString(" ") ==
              "0 2 4 6 8 10 12 14");
    }

    SECTION("Select") {
        std::function<bool(int)> selFunc = [](int i) {
            return i % 2 == 0;
        };
        auto selectors = lz::map(arr, std::move(selFunc));
        CHECK(lz::chain(arr).select(selectors).distance() == 8);
    }

    SECTION("DropWhile") {
        CHECK(lz::chain(arr).dropWhile([](int i) { return i >= 0; }).distance() == 0);
    }

    SECTION("JoinWhere") {
        auto joinWhere = lz::chain(arr).joinWhere(
            arr2, [](int a) { return a; }, [](int b) { return b; },
            [](int a, int b) -> std::tuple<int, int> {
                return std::tuple<int, int>{ a, b };
            });
        auto begin = joinWhere.begin();
        CHECK(*begin == std::make_tuple(0, 0));
        CHECK(*++begin == std::make_tuple(1, 1));
    }

    SECTION("Group by") {
        CHECK(lz::chain(arr).groupBy().distance() == size);
    }

    SECTION("Find first[if]/last[if]") {
        CHECK(lz::chain(arr).findFirstOrDefault(16, 16) == 16);
        CHECK(lz::chain(arr).findFirstOrDefaultIf([](int i) { return i == 16; }, 16) == 16);
        CHECK(lz::chain(arr).findLastOrDefault(16, 16) == 16);
        CHECK(lz::chain(arr).findLastOrDefaultIf([](int i) { return i == 16; }, 16) == 16);
    }

    SECTION("Index") {
        CHECK(lz::chain(arr).indexOf(15) == 15);
    }

    SECTION("Contains[if]") {
        CHECK(lz::chain(arr).contains(15));
        CHECK(lz::chain(arr).containsIf([](int i) { return i == 15; }));
    }

    SECTION("Folding") {
        CHECK(lz::chain(arr).distance() == size);
        CHECK(lz::chain(arr).foldr(0, [](int acc, int cur) { return acc + cur; }) == 120);
        CHECK(lz::chain(arr).any([](int i) { return i == 0; }));
        CHECK(lz::chain(arr).none([](int i) { return i == 16; }));
        CHECK(lz::chain(arr).count(0) == 1);
        CHECK(lz::chain(arr).countIf([](int i) { return i == 0; }) == 1);
        CHECK(lz::chain(arr).equal(lz::range(size)));
        CHECK(lz::chain(arr).sum() == 120);
        CHECK(lz::chain(arr).mean() == 7.5);
        CHECK(lz::chain(arr).median() == 7.5);
    }

    SECTION("Max/min") {
        CHECK(lz::chain(arr).max() == 15);
        CHECK(lz::chain(arr).min() == 0);
    }

    SECTION("Sorting") {
        arr[0] = 15;
        arr[15] = 0;
        lz::chain(arr).sort();
        CHECK(std::is_sorted(arr.begin(), arr.end()));
        CHECK(lz::chain(arr).isSorted());
    }

    SECTION("Start/endswith") {
        CHECK(lz::chain(arr).startsWith(std::array<int, 3>{ 0, 1, 2 }));
        CHECK(lz::chain(arr).endsWith(std::array<int, 3>{ 13, 14, 15 }));
        CHECK(!lz::chain(arr).startsWith(std::array<int, 3>{ 0, 1, 3 }));
        CHECK(!lz::chain(arr).endsWith(std::array<int, 3>{ 13, 14, 16 }));
    }
}
