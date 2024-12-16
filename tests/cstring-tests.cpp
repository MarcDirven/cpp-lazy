#include <Lz/c_string.hpp>
#include <Lz/Common.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Basic test") {
    const char* s = "hello, world!";
    auto cstr = lz::c_string(s);
    auto cstr_hello = lz::c_string(s, s + 5);

    SECTION("Correct size") {
        auto size = cstr.size();
        CHECK(size == std::strlen(s));

        auto hello_size = cstr_hello.size();
        CHECK(hello_size == std::strlen("hello"));
    }

    SECTION("Correct string") {
        CHECK(lz::equal(cstr_hello, lz::c_string("hello")));
        CHECK(lz::equal(cstr, lz::c_string("hello, world!")));
    }
}

TEST_CASE("CString binary operations", "[CString][Binary ops]") {
    const char* string = "123 456 789";
    auto c_string = lz::c_string(string);
    auto c_str_random_access = lz::c_string(string, string + 3);

    SECTION("Operator++") {
        auto begin = c_string.begin();
        CHECK(*begin == '1');
        ++begin;
        CHECK(*begin == '2');
    }

    SECTION("Operator== & Operator!=") {
        auto beg = c_string.begin();
        auto beg_ra = c_str_random_access.begin();
        CHECK(beg != c_string.end());
        CHECK(beg_ra != c_str_random_access.end());

        auto common = lz::common(c_string);
        auto begin_common = common.begin();
        auto end_common = common.end();
        CHECK(begin_common != end_common);
        begin_common = common.end();
        CHECK(begin_common == end_common);

        beg_ra = c_str_random_access.end();
        CHECK(beg_ra == c_str_random_access.end());
    }

    SECTION("Operator--") {
        auto end = c_str_random_access.end();
        CHECK(*--end == '3');
        --end, --end;
        CHECK(end == c_str_random_access.begin());
    }

    SECTION("Operator+(int), tests += as well") {
        auto beg_ra = c_str_random_access.begin();
        beg_ra += 2;
        CHECK(*beg_ra == '3');
    }

    SECTION("Operator-(Iterator)") {
        CHECK(c_str_random_access.end() - c_str_random_access.begin() == 3);
    }

    SECTION("Operator[]()") {
        auto begin = c_str_random_access.begin();
        CHECK(begin[0] == '1');
        CHECK(begin[1] == '2');
        CHECK(begin[2] == '3');
    }

    SECTION("Operator<, <, <=, >, >=") {
        const std::ptrdiff_t amount = 3;
        auto begin = c_str_random_access.begin();
        CHECK(begin < c_str_random_access.end());
        CHECK(begin + amount + 1 > c_str_random_access.end());
        CHECK(begin + amount <= c_str_random_access.end());
        CHECK(begin + amount >= c_str_random_access.end());
    }

    SECTION("Operator bool") {
        CHECK(c_string);
        CHECK(c_string.begin());
        CHECK(c_str_random_access);
        CHECK(c_str_random_access.begin());

        CHECK(c_str_random_access.end());

        auto tmp = lz::c_string("");
        CHECK(!tmp);
        CHECK(!tmp.begin());
    }
}

TEST_CASE("CString to containers", "[CString][To container]") {
    auto str = lz::c_string("Hello, World!");
    auto map_str = lz::c_string("123456789");

    SECTION("To array") {
        std::array<char, 14> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.to<std::array<char, 14>>() == expected);
    }

    SECTION("To vector") {
        std::vector<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.to_vector() == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.to<std::list<char>>() == expected);
    }

    SECTION("To map") {
        std::map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
                                          { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
        CHECK(map_str.to_map([](char c) { return std::make_pair(c, c); }) == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
                                                    { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
        CHECK(map_str.to_unordered_map([](char c) { return std::make_pair(c, c); }) == expected);
    }
}