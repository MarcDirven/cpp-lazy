#include <Lz/InclusiveScan.hpp>

#include <catch2/catch.hpp>

#include <iostream>
#include <numeric>
#include <list>

TEST_CASE("Inclusive scan changing and creating elements", "[InclusiveScan][Basic functionality]") {
    int arr[32]{};
    std::iota(std::begin(arr), std::end(arr), 0);
    auto scan = lz::iScan(arr);
    auto begin = scan.begin();

    std::ptrdiff_t sum = 0;
    for (std::ptrdiff_t i = 0; i < std::distance(std::begin(arr), std::end(arr)); ++i) {
        sum += i;
        CHECK(*begin == sum);
        ++begin;
    }

    REQUIRE(std::distance(scan.begin(), scan.end()) == std::distance(std::begin(arr), std::end(arr)));

    constexpr static int expected[] = { 0,   1,   3,   6,   10,  15,  21,  28,  36,  45,  55,  66,  78,  91,  105, 120,
                                        136, 153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465, 496 };
    CHECK(std::equal(std::begin(expected), std::end(expected), std::begin(scan)));
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
        auto actual = scanner.to<std::array<int, expected.size()>>();
        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner.toVector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner.to<std::list<int>>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                        { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner.toMap([](int i) { return std::make_pair(i + i, i); });
        for (auto&& p : actual) {
            UNSCOPED_INFO(fmt::format("({}, {})", p.first, p.second));
        }
        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                                  { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner.toUnorderedMap([](int i) { return std::make_pair(i + i, i); });
        CHECK(expected == actual);
    }
}
