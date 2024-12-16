#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Basic CommonView test") {
    const char* s = "hello, world!";
    auto cstr = lz::c_string(s);
    auto common = lz::common(cstr);
    CHECK(std::find(common.begin(), common.end(), 'h') == common.begin());
    CHECK(std::equal(common.begin(), common.end(), cstr.begin()));
}

TEST_CASE("CommonView binary operations", "[CommonView][Binary ops]") {
    const char* a = "hello ";
    auto cStringView = lz::c_string(a);
    auto common = lz::common(cStringView);

    auto begin = common.begin();
    CHECK(*begin == 'h');
    SECTION("Operator++") {
        ++begin;
        CHECK(*begin == 'e');
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != common.end());
        begin = common.end();
        CHECK(begin == common.end());
    }
}

TEST_CASE("CommonView to containers", "[CommonView][To container]") {
    const char* a = "hello ";
    auto cStringView = lz::c_string(a);
    auto common = lz::common(cStringView);

    SECTION("To array") {
        CHECK(common.to<std::array<char, 6>>() == std::array<char, 6>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To vector") {
        CHECK(common.to_vector() == std::vector<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To other container using to<>()") {
        CHECK(common.to<std::list<char>>() == std::list<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To map") {
        CHECK(common.to_map([](char c) { return std::make_pair(c, c); }) ==
              std::map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }

    SECTION("To unordered map") {
        CHECK(common.to_unordered_map([](char c) { return std::make_pair(c, c); }) ==
              std::unordered_map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }
}