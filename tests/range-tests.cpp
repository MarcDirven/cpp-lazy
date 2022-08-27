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
    auto fRange = lz::range(0., 10.5, 0.5);
    auto it = range.begin();
    auto fIt = fRange.begin();

    CHECK(*it == 0);
    CHECK(*fIt == 0.);

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 1);
        ++fIt;
        CHECK(*fIt == 0.5);
    }

    SECTION("Operator--") {
        ++it, --it;
        CHECK(*it == 0);
        ++fIt, --fIt;
        CHECK(*fIt == 0);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != range.end());
        it = range.end();
        CHECK(it == range.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 2) == 2);
        CHECK(*(it + 4) == 4);
        CHECK(*(fIt + 2) == 1.);
        CHECK(*(fIt + 3) == 1.5);
        CHECK(*(fIt + 5) == 2.5);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        CHECK(*((it + 2) - 1) == 1);
        CHECK(*((it + 4) - 2) == 2);
        CHECK(*((fIt + 2) - 1) == .5);
        CHECK(*((fIt + 3) - 2) == .5);
        CHECK(*((fIt + 5) - 2) == 1.5);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(range.end() - it == 10);
        CHECK(fRange.end() - fIt == 21);

        CHECK(range.end() - (it + 1) == 9);
        CHECK(fRange.end() - (fIt + 1) == 20);
    }

    SECTION("Operator[]()") {
        CHECK(fIt[1] == .5);
        CHECK(it[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = range.begin();
        auto end = range.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + size - 1 >= end - 1);

        auto fB = range.begin();
        auto fEnd = range.end();

        CHECK(fB < end);
        CHECK(fB + distance - 1 > fEnd - distance);
        CHECK(fB + distance - 1 <= fEnd);
        CHECK(fB + 20 >= fEnd - 1);
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
