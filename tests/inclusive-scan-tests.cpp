#include <Lz/InclusiveScan.hpp>

#include <catch2/catch.hpp>

#include <iostream>
#include <numeric>
#include <list>

TEST_CASE("Inclusive scan changing and creating elements", "[InclusiveScan][Basic functionality]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    auto scan = lz::iScan(arr);
    CHECK(*scan.begin() == 3);

    REQUIRE(std::distance(scan.begin(), scan.end()) == std::distance(std::begin(arr), std::end(arr)));

    int expected[] = { 3, 4, 8, 9, 14, 23, 25, 31 };
    CHECK(std::equal(std::begin(expected), std::end(expected), std::begin(scan)));

    scan = lz::iScanFrom(arr, 2);
    REQUIRE(std::distance(scan.begin(), scan.end()) == std::distance(std::begin(arr), std::end(arr)));
    int expected2[] = { 5, 6, 10, 11, 16, 25, 27, 33 };
    CHECK(std::equal(std::begin(expected2), std::end(expected2), std::begin(scan)));
}

TEST_CASE("Inclusive scan splitter binary operations", "[InclusiveScan][Binary ops]") {
    int arr[] = { 1, 2, 3, 4, 5 };
    auto scan = lz::iScan(arr);

    SECTION("Operator++") {
        auto it = scan.begin();
        CHECK(*it == 1);
        ++it;
        CHECK(*it == 1 + 2);
        ++it;
        CHECK(*it == 1 + 2 + 3);
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

TEST_CASE("Inclusive scan splitter to containers", "[InclusiveScan][To container]") {
    int toScan[] = { 2, 5, 6, 4, 87, 8, 45, 7 };
    auto scanner = lz::iScan(toScan);

    SECTION("To array") {
        std::array<int, 8> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner.toArray<expected.size()>();
        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner.toVector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner.to<std::list>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                        { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner.toMap([](int i) { return i + i; });
        for (auto&& p : actual) {
            UNSCOPED_INFO(fmt::format("({}, {})", p.first, p.second));
        }
        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                                  { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner.toUnorderedMap([](int i) { return i + i; });
        CHECK(expected == actual);
    }
}
