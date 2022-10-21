#include <Lz/CString.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Basic test") {
    const char* s = "hello, world!";
    auto cstr = lz::cString(s);
    auto cstrHello = lz::cString(s, s + 5);

    SECTION("Correct size") {
        auto size = cstr.size();
        CHECK(size == std::strlen(s));

        auto helloSize = cstrHello.size();
        CHECK(helloSize == std::strlen("hello"));
    }

    SECTION("Correct string") {
        CHECK(lz::equal(cstrHello, lz::cString("hello")));
        CHECK(lz::equal(cstr, lz::cString("hello, world!")));
    }
}

TEST_CASE("CString binary operations", "[CString][Binary ops]") {
    const char* string = "123 456 789";
    auto cString = lz::cString(string);
    auto cStrRandomAccess = lz::cString(string, string + 3);

    SECTION("Operator++") {
        auto begin = cString.begin();
        CHECK(*begin == '1');
        ++begin;
        CHECK(*begin == '2');
    }

    SECTION("Operator== & Operator!=") {
        auto beg = cString.begin();
        auto begRA = cStrRandomAccess.begin();
        CHECK(beg != cString.end());
        CHECK(begRA != cStrRandomAccess.end());

        beg = cString.end();
        begRA = cStrRandomAccess.end();

        CHECK(beg == cString.end());
        CHECK(begRA == cStrRandomAccess.end());
    }

    SECTION("Operator--") {
        auto end = cStrRandomAccess.end();
        CHECK(*--end == '3');
        --end, --end;
        CHECK(end == cStrRandomAccess.begin());
    }

    SECTION("Operator+(int), tests += as well") {
        auto begRA = cStrRandomAccess.begin();
        begRA += 2;
        CHECK(*begRA == '3');
    }

    SECTION("Operator-(Iterator)") {
        CHECK(cStrRandomAccess.end() - cStrRandomAccess.begin() == 3);
    }

    SECTION("Operator[]()") {
        auto begin = cStrRandomAccess.begin();
        CHECK(begin[0] == '1');
        CHECK(begin[1] == '2');
        CHECK(begin[2] == '3');
    }

    SECTION("Operator<, <, <=, >, >=") {
        const std::ptrdiff_t amount = 3;
        auto begin = cStrRandomAccess.begin();
        CHECK(begin < cStrRandomAccess.end());
        CHECK(begin + amount + 1 > cStrRandomAccess.end());
        CHECK(begin + amount <= cStrRandomAccess.end());
        CHECK(begin + amount >= cStrRandomAccess.end());
    }

    SECTION("Operator bool") {
        CHECK(cString);
        CHECK(cString.begin());
        CHECK(cStrRandomAccess);
        CHECK(cStrRandomAccess.begin());

        CHECK(!cString.end());
        CHECK(cStrRandomAccess.end());

        auto tmp = lz::cString("");
        CHECK(!tmp);
        CHECK(!tmp.begin());
    }
}

TEST_CASE("CString to containers", "[CString][To container]") {
    auto str = lz::cString("Hello, World!");
    auto mapStr = lz::cString("123456789");

    SECTION("To array") {
        std::array<char, 14> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.toArray<14>() == expected);
    }

    SECTION("To vector") {
        std::vector<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.toVector() == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        CHECK(str.to<std::list>() == expected);
    }

    SECTION("To map") {
        std::map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
                                          { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
        CHECK(mapStr.toMap([](char c) { return c; }) == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
                                                    { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
        CHECK(mapStr.toUnorderedMap([](char c) { return c; }) == expected);
    }
}