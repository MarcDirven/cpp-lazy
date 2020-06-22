#include <list>

#include <catch.hpp>
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
}

TEST_CASE("Range binary operations", "[Range][Binary ops]") {
    auto range = lz::range(10);
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

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 2) == 2);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++it;
        CHECK(*(it - 1) == 0);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(std::distance(range.begin(), range.end()) == 10);
    }

    SECTION("Operator[]()") {
        CHECK(range.begin()[1] == 1);
    }

    SECTION("Operator<, checks all '<, <=, >, >=' operators") {
        CHECK(range.begin() < range.end());
    }
}

TEST_CASE("Range to containers", "[Range][To container]") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<size_t>(size)> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto actual = range.toArray<static_cast<size_t>(size)>();

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
}
