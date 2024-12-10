#include <Lz/Algorithm.hpp>
#include <Lz/CString.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Algorithm") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<int> v2;
    std::vector<int> v3 = { 1 };

    SECTION("Empty") {
        CHECK(!lz::empty(v));
        CHECK(lz::empty(v2));
        CHECK(!lz::empty(v3));
    }

    SECTION("Has one") {
        CHECK(lz::hasOne(v3));
        CHECK(!lz::hasOne(v));
        CHECK(!lz::hasOne(v2));
    }

    SECTION("Has many") {
        CHECK(lz::hasMany(v));
        CHECK(!lz::hasMany(v3));
        CHECK(!lz::hasMany(v2));
    }

    SECTION("Front") {
        CHECK(lz::front(v) == 1);
        CHECK(lz::front(v3) == 1);
    }

    SECTION("Back") {
        CHECK(lz::back(v) == 5);
        CHECK(lz::back(v3) == 1);
    }

    SECTION("Front or") {
        CHECK(lz::frontOr(v, 0) == 1);
        CHECK(lz::frontOr(v2, 0) == 0);
        CHECK(lz::frontOr(v3, 0) == 1);
    }

    SECTION("Back or") {
        CHECK(lz::backOr(v, 0) == 5);
        CHECK(lz::backOr(v2, 0) == 0);
        CHECK(lz::backOr(v3, 0) == 1);
    }

    const char* s = "hello, world!";
    auto cstr = lz::cString(s);

    SECTION("Distance") {
        CHECK(static_cast<std::size_t>(lz::distance(cstr.begin(), cstr.end())) == std::strlen(s));
    }

    SECTION("Accumulate") {
        CHECK(lz::accumulate(cstr, std::string{}, [](std::string&& lhs, char rhs) { return lhs + rhs; }) == "hello, world!");
    }

    SECTION("Max/min element") {
        CHECK(*lz::maxElement(cstr) == 'w');
        CHECK(*lz::minElement(cstr) == ' ');
    }

    SECTION("Find") {
        CHECK(*lz::find(cstr, 'w') == 'w');
        CHECK(*lz::findIf(cstr, [](char c) { return c == 'w'; }) == 'w');
        CHECK(lz::find(cstr, 'z') == cstr.end());
        CHECK(lz::findIf(cstr, [](char c) { return c == 'z'; }) == cstr.end());

        CHECK(lz::findIfNot(cstr, [](char c) { return c == 'z'; }) == cstr.begin());
        CHECK(lz::findIfNot(cstr, [](char c) { return c == 'h'; }) == std::next(cstr.begin()));
    }
}