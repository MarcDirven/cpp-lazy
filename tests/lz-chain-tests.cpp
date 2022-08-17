#include "Lz/Lz.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>

template class lz::IterView<lz::internal::BasicIteratorView<std::vector<int>::iterator>::iterator>;

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

    auto result = lz::toIter(arr)
                      .as<int&>()
                      .map(std::move(f))
                      .forEach([x = 0](int& i) mutable { i = x++; })
                      .zip(arr2)
                      .map(std::move(tupFn))
                      .concat(arr2)
                      .foldl(0, std::plus<int>()); // NOLINT

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

    arr = lz::range(size).toArray<size>();

    auto excluded = lz::toIter(arr).exclude(0, 1).toArray<size - 1>();
    CHECK(excluded == lz::range(1, size).toArray<size - 1>());

    SECTION("Join") {
        auto joined = lz::toIter(arr).join(" ").toString();
        CHECK(joined == "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15");
    }

    SECTION("Slice") {
        auto sliced = lz::toIter(arr).slice(0, 5).toArray<5>();
        CHECK(sliced == lz::range(0, 5).toArray<5>());
    }

    SECTION("TakeEvery") {
        auto takeEvery = lz::toIter(arr).takeEvery(2).toArray<size / 2>();
        CHECK(takeEvery == std::array<int, 8>{ 0, 2, 4, 6, 8, 10, 12, 14 });
    }

    SECTION("Chunks") {
        auto chunks = lz::toIter(arr).chunks(4).toArray<size / 4>();
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
        auto zippedWith = lz::toIter(arr).zipWith([](int a, int b) { return a + b; }, arr2);
        for (auto&& enumerate : lz::enumerate(zippedWith)) {
            CHECK(enumerate.first + enumerate.first == enumerate.second);
        }
    }

    SECTION("Equal, reverse") {
        auto revved = lz::toIter(arr).reverse();
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
        CHECK(lz::toIter(arrayToTrim).trim([](int i) { return i == 1; }, [](int i) { return i == 4; }).toArray<1>() ==
              std::array<int, 1>{ 3 });
        std::string toTrim = "\t  a b c \t";
        CHECK(lz::trimString(toTrim).toString() == "a b c");
    }

    SECTION("Cartesian") {
        std::array<int, 2> a = { 1, 2 };
        std::array<int, 2> b = { 3, 4 };
        auto cart = lz::toIter(a).cartesian(b);
        CHECK(*cart.begin() == std::make_tuple(1, 3));
        CHECK(*std::next(cart.begin()) == std::make_tuple(1, 4));
        CHECK(*std::next(cart.begin(), 2) == std::make_tuple(2, 3));
        CHECK(*std::next(cart.begin(), 3) == std::make_tuple(2, 4));
        CHECK(std::next(cart.begin(), 4) == cart.end());
    }

    SECTION("Flatten") {
        std::array<std::array<int, 3>, 3> toFlatten{};
        CHECK(lz::equal(lz::toIter(toFlatten).flatten(), lz::repeat(0, 3 * 3)));
    }

    SECTION("First") {
        CHECK(*lz::toIter(arr).next(1) == 1);
    }

    SECTION("first, last, empty, many") {
        CHECK(!lz::toIter(arr).empty());
        CHECK(!lz::toIter(arr).hasOne());
        CHECK(lz::toIter(arr).hasMany());
        CHECK(lz::toIter(arr).front() == 0);
        CHECK(lz::toIter(arr).back() == 15);

        std::vector<int> v;
        CHECK(lz::toIter(v).frontOr(20) == 20);
        CHECK(lz::toIter(v).backOr(20) == 20);
    }

    SECTION("Filter") {
        CHECK(lz::toIter(arr).filter([](int i) { return i % 2 == 0; }).distance() == 8);
    }

    SECTION("Except") {
        CHECK(lz::toIter(arr).except(arr2).distance() == 0);
    }

    SECTION("Unique") {
        CHECK(lz::toIter(arr).unique().distance() == size);
    }

    SECTION("ChunkIf") {
        CHECK(lz::toIter(arr).chunkIf([](int i) { return i % 2 == 0; }).distance() == 9);
    }

    SECTION("FilterMap") {
        CHECK(
            lz::toIter(arr).filterMap([](int i) { return i % 2 == 0; }, [](int i) { return fmt::to_string(i); }).toString(" ") ==
            "0 2 4 6 8 10 12 14");
    }

    SECTION("Select") {
        std::function<bool(int)> selFunc = [](int i) {
            return i % 2 == 0;
        };
        auto selectors = lz::map(arr, std::move(selFunc));
        CHECK(lz::toIter(arr).select(selectors).distance() == 8);
    }

    SECTION("DropWhile") {
        CHECK(lz::toIter(arr).dropWhile([](int i) { return i >= 0; }).distance() == 0);
    }

    SECTION("JoinWhere") {
        auto joinWhere = lz::toIter(arr).joinWhere(
            arr2, [](int a) { return a; }, [](int b) { return b; },
            [](int a, int b) -> std::tuple<int, int> {
                return { a, b };
            });
        CHECK(*joinWhere.begin() == std::make_tuple(0, 0));
        CHECK(lz::back(joinWhere) == std::make_tuple(15, 15));
    }

    SECTION("Group by") {
        CHECK(lz::toIter(arr).groupBy().distance() == size);
    }

    SECTION("Find first[if]/last[if]") {
        CHECK(lz::toIter(arr).findFirstOrDefault(16, 16) == 16);
        CHECK(lz::toIter(arr).findFirstOrDefaultIf([](int i) { return i == 16; }, 16) == 16);
        CHECK(lz::toIter(arr).findLastOrDefault(16, 16) == 16);
        CHECK(lz::toIter(arr).findLastOrDefaultIf([](int i) { return i == 16; }, 16) == 16);
    }

    SECTION("Index") {
        CHECK(lz::toIter(arr).indexOf(15) == 15);
    }

    SECTION("Contains[if]") {
        CHECK(lz::toIter(arr).contains(15));
        CHECK(lz::toIter(arr).containsIf([](int i) { return i == 15; }));
    }

    SECTION("Folding") {
        CHECK(lz::toIter(arr).distance() == size);
        CHECK(lz::toIter(arr).foldr(0, [](int acc, int cur) { return acc + cur; }) == 120);
        CHECK(lz::toIter(arr).any([](int i) { return i == 0; }));
        CHECK(lz::toIter(arr).none([](int i) { return i == 16; }));
        CHECK(lz::toIter(arr).count(0) == 1);
        CHECK(lz::toIter(arr).countIf([](int i) { return i == 0; }) == 1);
        CHECK(lz::toIter(arr).equal(lz::range(size)));
        CHECK(lz::toIter(arr).sum() == 120);
        CHECK(lz::toIter(arr).mean() == 7.5);
        CHECK(lz::toIter(arr).median() == 7.5);
    }

    SECTION("Max/min") {
        CHECK(lz::toIter(arr).max() == 15);
        CHECK(lz::toIter(arr).min() == 0);
    }

    SECTION("Sorting") {
        arr[0] = 15;
        arr[15] = 0;
        lz::toIter(arr).sort();
        CHECK(std::is_sorted(arr.begin(), arr.end()));
        CHECK(lz::toIter(arr).isSorted());
    }

    SECTION("Start/endswith") {
        CHECK(lz::toIter(arr).startsWith(std::array<int, 3>{ 0, 1, 2 }));
        CHECK(lz::toIter(arr).endsWith(std::array<int, 3>{ 13, 14, 15 }));
    }
}
