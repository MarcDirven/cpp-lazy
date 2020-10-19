#include <list>

#include <catch2/catch.hpp>
#include <Lz/Range.hpp>


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

        for (int i: lz::range(5, 0, -1)) {
            CHECK(expectedCounter == i);
            expectedCounter--;
        }
    }

    SECTION("Looping upwards with step") {
        int expectedCounter = 0;

        for (int i: lz::range(0, 5, 2)) {
            CHECK(expectedCounter == i);
            expectedCounter += 2;
        }
    }

    SECTION("Looping backwards with step") {
        int expectedCounter = 5;

        for (int i: lz::range(5, 0, -2)) {
            CHECK(expectedCounter == i);
            expectedCounter -= 2;
        }
    }

    SECTION("Exceptions") {
        CHECK_THROWS(lz::range(0, 10, -1));
        CHECK_THROWS(lz::range(-10, -20, 1));
        CHECK_THROWS(lz::range(10, 20, 0));

        CHECK_NOTHROW(lz::range(-10, -20, -1));
        CHECK_NOTHROW(lz::range(50, 0, -1));
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

    SECTION("Operator--") {
        ++it;
        --it;
        CHECK(*it == 0);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != range.end());
        it = range.end();
        CHECK(it == range.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 2) == 2);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++it;
        CHECK(*(it - 1) == 0);
    }

    SECTION("Operator-(Iterator)") {
        CHECK((range.end() - range.begin()) == 10);

        auto step = 2;
        auto otherRange = lz::range(0, size, step);
        CHECK(std::distance(otherRange.begin(), otherRange.end()) == static_cast<std::ptrdiff_t>(size / step));
        CHECK(otherRange.end() - otherRange.begin() == static_cast<std::ptrdiff_t>(size / step));
    }

    SECTION("Operator[]()") {
        CHECK(range.begin()[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(range.begin() < range.end());
        CHECK(range.begin() + size + 1 > range.end());
        CHECK(range.begin() + size <= range.end());
        CHECK(range.begin() + size >= range.end());
    }
}

TEST_CASE("Range to containers", "[Range][To container]") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size)> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto actual = range.toArray<static_cast<std::size_t>(size)>();

        CHECK(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto actual = range.toVector();

        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
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
