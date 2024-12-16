#include <Lz/CString.hpp>
#include <Lz/DropWhile.hpp>
#include <Lz/take_while.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Take while with sentinels") {
    auto cstr = lz::c_string("Hello, World!");
    auto take_while = lz::take_while(cstr, [](char c) { return c != 'W'; });
    static_assert(!std::is_same<decltype(take_while.begin()), decltype(take_while.end())>::value, "Should be sentinel");
    auto cStrExpected = lz::c_string("Hello, ");
    CHECK(lz::equal(take_while, cStrExpected));
}

TEST_CASE("take_while_iterable takes elements and is by reference", "[take_while_iterable][Basic functionality]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SECTION("Should take elements") {
        auto take_while = lz::take_while(array, [](int element) { return element < 5; });
        auto it = take_while.begin();
        CHECK(std::distance(it, take_while.end()) == 4);
    }

    SECTION("Should be by reference") {
        auto take_while = lz::take_while(array, [](int element) { return element < 5; });
        auto it = take_while.begin();
        *it = 50;
        CHECK(array[0] == 50);
    }
}

TEST_CASE("Drop while takes elements and is by reference", "[take_while_iterable][Basic functionality]") {
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

TEST_CASE("take_while_iterable binary operations", "[take_while_iterable][Binary ops]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto begin = take_while.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take_while.begin() != take_while.end());
        REQUIRE_FALSE(take_while.begin() == take_while.end());
        auto it = take_while.begin();
        it = take_while.end();
        REQUIRE(it == take_while.end());
    }

    SECTION("Operator--") {
        auto end = take_while.end();
        --end;
        REQUIRE(*end == 4);
        --end;
        REQUIRE(*end == 3);
        --end;
        REQUIRE(*end == 2);
        --end;
        REQUIRE(*end == 1);
    }
}

TEST_CASE("take_while_iterable to containers", "[take_while_iterable][To container]") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("To array") {
        auto arr = take_while.to<std::array<int, 4>>();
        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("To vector") {
        auto vec = take_while.to_vector();
        REQUIRE(std::equal(vec.begin(), vec.end(), take_while.begin()));
    }

    SECTION("To other container using to<>()") {
        auto lst = take_while.to<std::list<int>>();
        REQUIRE(std::equal(lst.begin(), lst.end(), take_while.begin()));
    }

    SECTION("To map") {
        auto map = take_while.to_map([](int i) { return std::make_pair(i, i); });
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = take_while.to_unordered_map([](int i) { return std::make_pair(i, i); });
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}