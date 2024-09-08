#include <Lz/TakeWhile.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("TakeWhile takes elements and is by reference", "[TakeWhile][Basic functionality]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SECTION("Should take elements") {
        auto takeWhile = lz::takeWhile(array, [](int element) { return element < 5; });
        auto it = takeWhile.begin();
        CHECK(std::distance(it, takeWhile.end()) == 4);
    }

    SECTION("Should be by reference") {
        auto takeWhile = lz::takeWhile(array, [](int element) { return element < 5; });
        auto it = takeWhile.begin();
        *it = 50;
        CHECK(array[0] == 50);
    }
}

TEST_CASE("Drop while takes elements and is by reference", "[TakeWhile][Basic functionality]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SECTION("Should drop elements") {
        auto dropWhile = lz::dropWhile(array, [](int element) { return element < 5; });
        auto it = dropWhile.begin();
        CHECK(std::distance(it, dropWhile.end()) == 6);
    }

    SECTION("Should be by reference") {
        auto dropWhile = lz::dropWhile(array, [](int element) { return element < 5; });
        auto it = dropWhile.begin();
        *it = 50;
        CHECK(array[4] == 50);
        CHECK(array[4] != 6);
        CHECK(array[0] == 1);
    }
}

TEST_CASE("TakeWhile binary operations", "[TakeWhile][Binary ops]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto takeWhile = lz::takeWhile(array, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto begin = takeWhile.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(takeWhile.begin() != takeWhile.end());
        REQUIRE_FALSE(takeWhile.begin() == takeWhile.end());
        auto it = takeWhile.begin();
        it = takeWhile.end();
        REQUIRE(it == takeWhile.end());
    }
}

TEST_CASE("TakeWhile to containers", "[TakeWhile][To container]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto takeWhile = lz::takeWhile(array, [](int element) { return element < 5; });

    SECTION("To array") {
        auto arr = takeWhile.toArray<4>();
        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("To vector") {
        auto vec = takeWhile.toVector();
        REQUIRE(std::equal(vec.begin(), vec.end(), takeWhile.begin()));
    }

    SECTION("To other container using to<>()") {
        auto lst = takeWhile.to<std::list>();
        REQUIRE(std::equal(lst.begin(), lst.end(), takeWhile.begin()));
    }

    SECTION("To map") {
        auto map = takeWhile.toMap([](int i) { return i; });
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = takeWhile.toUnorderedMap([](int i) { return i; });
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}