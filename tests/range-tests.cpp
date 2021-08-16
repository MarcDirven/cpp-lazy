#include <Lz/Range.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Range changing and creating elements", "[Range][Basic functionality]") {
    SECTION("Looping upwards") {
        int expectedCounter = 0;

        for (int i : lz::range(5)) {
            CHECK(expectedCounter == i);
            expectedCounter++;
        }
    }

    SECTION("Looping backwards") {
        int expectedCounter = 5;

        for (int i : lz::range(5, 0, -1)) {
            CHECK(expectedCounter == i);
            expectedCounter--;
        }
    }

    SECTION("Looping upwards with step") {
        int expectedCounter = 0;

        for (int i : lz::range(0, 5, 2)) {
            CHECK(expectedCounter == i);
            expectedCounter += 2;
        }
    }

    SECTION("Looping backwards with step") {
        int expectedCounter = 5;

        for (int i : lz::range(5, 0, -2)) {
            CHECK(expectedCounter == i);
            expectedCounter -= 2;
        }
    }
}

TEST_CASE("Range binary operations", "[Range][Binary ops]") {
    constexpr int size = 10;
    auto range = lz::range(size);
    auto it = range.begin();

    CHECK(*it == 0);

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != range.end());
        it = range.end();
        CHECK(it == range.end());
    }

    SECTION("Lz distance") {
        CHECK(lz::distance(range.begin(), range.end()) == 10);
        range = lz::range(5, 10, 2);

        CHECK(lz::distance(range.begin(), range.end()) == 3);
        range = lz::range(5, 10, 3);
        CHECK(lz::distance(range.begin(), range.end()) == 2);
    }

    SECTION("Lz next") {
        CHECK(*lz::next(it, 5) == 5);
        CHECK(*lz::next(it, 11) == 11);
        range = lz::range(5, 10, 2);
        CHECK(*lz::next(range.begin(), 2) == 9);
    }

    SECTION("Floating range") {
        auto floatRange = lz::range(0., 10., 3.5);
        auto fltRangeIterator = floatRange.begin();

        CHECK(lz::distance(fltRangeIterator, floatRange.end()) == 3);
        ++fltRangeIterator;
        CHECK(*fltRangeIterator == 3.5);

        floatRange = lz::range(0., 9., 2.);
        CHECK(lz::distance(floatRange.begin(), floatRange.end()) == 5);

    }
}

TEST_CASE("Range to containers", "[Range][To container]") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size)> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.toArray<static_cast<std::size_t>(size)>();

        CHECK(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.toVector();

        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to<std::list>();

        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = range.toMap([](const int i) { return i; });
        std::map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = range.toUnorderedMap([](const int i) { return i; });
        std::unordered_map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        CHECK(expected == actual);
    }
}
