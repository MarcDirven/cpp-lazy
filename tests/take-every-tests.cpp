#include <array>
#include <list>

#include <catch.hpp>

#include <Lz/TakeEvery.hpp>


TEST_CASE("TakeEvery changing and creating elements", "[TakeEvery][Basic functionality]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = {1, 2, 3, 4};
    auto takeEvery = lz::takeEvery(array, 2);
    auto iterator = takeEvery.begin();

    SECTION("TakeEvery should be by reference") {
        *iterator = 0;
        CHECK(array[0] == 0);
    }

    SECTION("TakeEvery should select every amount-th") {
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(iterator == takeEvery.end());
    }

    SECTION("TakeEvery should select every amount-th with skip first") {
        takeEvery = lz::takeEvery(array, 2, 1);
        iterator = takeEvery.begin();

        CHECK(*iterator == 2);
        ++iterator;
        CHECK(*iterator == 4);
        ++iterator;
        CHECK(iterator == takeEvery.end());
    }
}


TEST_CASE("TakeEvery binary operations", "[TakeEvery][Binary ops]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = {1, 2, 3, 4};
    auto takeEvery = lz::takeEvery(array, 3);
    auto iterator = takeEvery.begin();

    SECTION("Operator++") {
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 4);
    }

    SECTION("Operator--") {
        ++iterator;
        --iterator;
        CHECK(*iterator == 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(iterator != takeEvery.end());
        iterator = takeEvery.end();
        CHECK(iterator == takeEvery.end());
    }

    SECTION("Operator+(int), tests += as well") {
        CHECK(*(iterator + 1) == 4);
    }

    SECTION("Operator-(int), tests -= as well") {
        ++iterator;
        CHECK(*(iterator - 1) == 1);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(takeEvery.end() - takeEvery.begin() == 2);
    }

    SECTION("Operator[]()") {
        CHECK(iterator[1] == array[3]);
    }

    SECTION("Operator<, '<, <=, >, >='") {
        auto b = takeEvery.begin();
        auto end = takeEvery.end();

        CHECK(b < end);
        CHECK(b + 1 > end - 1);
        CHECK(b + 1 <= end);
        CHECK(b + 1  >= end - 1);
    }
}

TEST_CASE("TakeEvery to containers", "[TakeEvery][To container]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = {1, 2, 3, 4};
    constexpr std::size_t offset = 2;
    auto takeEvery = lz::takeEvery(array, offset);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size / offset)> actual = takeEvery.toArray<offset>();
        CHECK(actual == std::array<int, offset>{1, 3});
    }

    SECTION("To vector") {
        std::vector<int> actual = takeEvery.toVector();
        CHECK(actual == std::vector<int>{1, 3});
    }

    SECTION("To other container using to<>()") {
        std::list<int> actual = takeEvery.to<std::list>();
        CHECK(actual == std::list<int>{1, 3});
    }

    SECTION("To map") {
        std::map<int, int> actual = takeEvery.toMap([](const int i) { return i; });
        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(3, 3)
        };
        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = takeEvery.toUnorderedMap([](const int i) { return i; });
        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(3, 3)
        };
        CHECK(actual == expected);
    }

}
