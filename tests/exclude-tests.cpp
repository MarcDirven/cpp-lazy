#include "Lz/Exclude.hpp"

#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Exclude changing and creating elements", "[Exclude][Basic functionality]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto excluded1 = lz::exclude(arr, 3, 5);
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 8, 10);

    SECTION("Should Exclude out element") {
        constexpr auto signedSize = static_cast<std::ptrdiff_t>(arr.size() - 2);
        CHECK(std::distance(excluded1.begin(), excluded1.end()) == signedSize);
        CHECK(std::distance(excluded2.begin(), excluded2.end()) == signedSize);
        CHECK(std::distance(excluded3.begin(), excluded3.end()) == signedSize);
    }

    SECTION("Should be by reference") {
        *excluded2.begin() = 0;
        CHECK(arr[2] == 0);
    }
}

TEST_CASE("Exclude binary operations", "[Exclude][Binary ops]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = lz::exclude(arr, 3, 5);
    auto exBeg1 = excluded1.begin();
    auto exEnd1 = excluded1.end();

    auto excluded2 = lz::exclude(arr, 0, 2);
    auto exBeg2 = excluded2.begin();
    auto exEnd2 = excluded2.end();

    auto excluded3 = lz::exclude(arr, 7, 10);
    auto exBeg3 = excluded3.begin();
    auto exEnd3 = excluded3.end();

    SECTION("Operator++") {
        CHECK(*exBeg1 == 1);
        ++exBeg1, ++exBeg1, ++exBeg1;
        CHECK(*exBeg1 == 6);
        CHECK(*++exBeg1 == 7);

        CHECK(*exBeg2 == 3);
        CHECK(++*exBeg2 == 4);

        CHECK(*exBeg3 == 1);
        ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3;
        CHECK(exBeg3 == excluded3.end());
    }

    SECTION("Operator== & operator!=") {
        CHECK(exBeg1 != exEnd1);
        CHECK(exBeg2 != exEnd2);
        CHECK(exBeg3 != exEnd3);
    }

    SECTION("Lz next") {
        CHECK(*lz::next(exBeg1, 0) == 1);
        CHECK(*lz::next(exBeg1, 3) == 6);
        CHECK(*lz::next(exBeg1, 2) == 3);
        CHECK(*lz::next(exBeg1, 5) == 8);
        CHECK(*lz::next(exBeg1, 6) == 9);

        CHECK(*lz::next(exBeg2, 0) == 3);
        CHECK(*lz::next(exBeg2, 1) == 4);
        CHECK(*lz::next(exBeg2, 2) == 5);
        CHECK(*lz::next(exBeg2, 3) == 6);

        CHECK(*lz::next(exBeg3, 0) == 1);
        CHECK(*lz::next(exBeg3, 5) == 6);
        CHECK(*lz::next(exBeg3, 6) == 7);
        CHECK(lz::next(exBeg3, 7) == exEnd3);
    }

    SECTION("Lz distance") {
        CHECK(lz::distance(exBeg1, exEnd1) == 8);
        CHECK(lz::distance(lz::next(exBeg1, 3), exEnd1) == 5);

        CHECK(lz::distance(exBeg2, exEnd2) == 8);
        CHECK(lz::distance(lz::next(exBeg2, 1), exEnd2) == 7);

        CHECK(lz::distance(exBeg3, exEnd3) == 7);
        CHECK(lz::distance(lz::next(exBeg3, 7), exEnd3) == 0);
    }
}

TEST_CASE("Exclude to containers", "[Exclude][To container]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = lz::exclude(arr, 3, 5);
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 7, 10);

    SECTION("To array") {
        CHECK(excluded1.toArray<8>() == std::array<int, 8>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        CHECK(excluded2.toArray<8>() == std::array<int, 8>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        CHECK(excluded3.toArray<7>() == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        CHECK(excluded1.toVector() == std::vector<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        CHECK(excluded2.toVector() == std::vector<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        CHECK(excluded3.toVector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        CHECK(excluded1.to<std::list>() == std::list<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        CHECK(excluded2.to<std::list>() == std::list<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        CHECK(excluded3.to<std::list>() == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        CHECK(excluded1.toMap([](int i) {
            return i;
        }) == std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        CHECK(excluded2.toMap([](int i) {
            return i;
        }) == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        CHECK(excluded3.toMap([](int i) {
            return i;
        }) == std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }

    SECTION("To unordered map") {
        CHECK(excluded1.toUnorderedMap([](int i) {
            return i;
        }) == std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        CHECK(excluded2.toUnorderedMap([](int i) {
            return i;
        }) == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        CHECK(excluded3.toUnorderedMap([](int i) {
            return i;
        }) == std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }
}