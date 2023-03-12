#include "Lz/ExclusiveScan.hpp"

#include <catch2/catch.hpp>
#include <iostream>
#include <list>
#include <numeric>


TEST_CASE("ExclusiveScan basic functionality", "[ExclusiveScan][Basic functionality]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    auto scan = lz::eScan(arr);

    CHECK(*scan.begin() == 0);
    CHECK(std::distance(std::begin(scan), std::end(scan)) == std::distance(std::begin(arr), std::end(arr)));
    constexpr auto isSame = std::is_same<int&, decltype(*scan.begin())>::value;
    CHECK(isSame);
}

TEST_CASE("Exclusive scan binary operations", "[ExclusiveScan][Binary ops]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2 };
    auto scan = lz::eScan(arr, 0);

    REQUIRE(*scan.begin() == 0);

    SECTION("Operator++") {
        auto begin = scan.begin();
        ++begin;
        CHECK(*begin == 0 + 3);
        ++begin;
        CHECK(*begin == 0 + 3 + 1);
        ++begin;
        CHECK(*begin == 0 + 3 + 1 + 4);
    }

    SECTION("Operator== & operator!=") {
        CHECK(scan.begin() != scan.end());
        auto begin = scan.begin();
        begin = scan.end();
        CHECK(begin == scan.end());
        begin = scan.begin();
        ++begin;
        CHECK(begin != scan.begin());
        CHECK(begin != scan.end());
    }
}

TEST_CASE("Exclusive scan to container", "[ExclusiveScan][To container]") {
    int toScan[] = { 2, 5, 6, 4, 87, 8, 45, 7 };
    auto scanner = lz::eScan(toScan, 0);

    SECTION("To array") {
        std::array<int, 8> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.toArray<expected.size()>();
        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.toVector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to<std::list>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                        { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.toMap([](int i) { return i + i; });
        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                                  { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.toUnorderedMap([](int i) { return i + i; });
        CHECK(expected == actual);
    }
}