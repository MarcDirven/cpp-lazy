#include <catch.hpp>
#include <Lz/Join.hpp>
#include <iostream>


TEST_CASE("Join should convert to string", "[Join][Basic functionality]") {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::vector<std::string> s = {"h", "e", "l", "l", "o"};
    auto joinInt = lz::join(v, ", ");
    auto joinStr = lz::join(s, ", ");

    SECTION("Should convert to string") {
        CHECK(*joinInt.begin() == "1");
        CHECK(*joinStr.begin() == "h");
    }

    SECTION("Type checking") {
        CHECK(std::is_same<decltype(*joinStr.begin()), std::string&>::value);
        CHECK(std::is_same<decltype(*joinInt.begin()), std::string>::value);
    }

    SECTION("Should be correct size") {
        CHECK(std::distance(joinInt.begin(), joinInt.end()) == 9);
        CHECK(std::distance(joinStr.begin(), joinStr.end()) == 9);
    }
}

TEST_CASE("Join binary operations", "[Join][Binary ops]") {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::vector<std::string> s = {"h", "e", "l", "l", "o"};
    auto joinInt = lz::join(v, ", ");
    auto joinStr = lz::join(s, ", ");

    auto joinIntBegin = joinInt.begin();
    auto joinStrBegin = joinStr.begin();

    CHECK(*joinIntBegin == "1");
    CHECK(*joinStrBegin == "h");

    SECTION("Operator++") {
        ++joinIntBegin;
        CHECK(*joinIntBegin == ", ");

        ++joinStrBegin;
        CHECK(*joinStrBegin == ", ");
    }

    SECTION("Operator--") {
        ++joinIntBegin, ++joinStrBegin;

        --joinIntBegin;
        CHECK(*joinIntBegin == "1");

        --joinStrBegin;
        CHECK(*joinStrBegin == "h");
    }

    SECTION("Operator== & operator!=") {
        CHECK(joinIntBegin != joinInt.end());
        CHECK(joinStrBegin != joinStr.end());

        joinIntBegin = joinInt.end();
        joinStrBegin = joinStr.end();

        CHECK(joinIntBegin == joinInt.end());
        CHECK(joinStrBegin == joinStr.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(joinIntBegin + 2) == "2");
        CHECK(*(joinStrBegin + 2) == "e");

        CHECK(*(joinIntBegin + 3) == ", ");
        CHECK(*(joinStrBegin + 3) == ", ");
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        joinIntBegin = joinInt.end();
        joinStrBegin = joinStr.end();

        CHECK(*(joinIntBegin - 1) == "5");
        CHECK(*(joinStrBegin - 1) == "o");

        CHECK(*(joinIntBegin - 2) == ", ");
        CHECK(*(joinStrBegin - 2) == ", ");
    }

    SECTION("Operator-(Iterator)") {
        CHECK(std::distance(joinIntBegin, joinInt.end()) == 9);
        CHECK(std::distance(joinStrBegin, joinStr.end()) == 9);

        CHECK(joinInt.end() - joinIntBegin == 9);
        CHECK(joinStr.end() - joinStrBegin == 9);
    }

    SECTION("Operator[]()") {
        CHECK(joinIntBegin[2] == "2");
        CHECK(joinIntBegin[1] == ", ");

        CHECK(joinStrBegin[2] == "e");
        CHECK(joinStrBegin[3] == ", ");
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto joinIntDistance = std::distance(joinIntBegin, joinInt.end());
        auto joinIntEnd = joinInt.end();
        CHECK(joinIntBegin < joinIntEnd);
        CHECK(joinIntBegin + joinIntDistance - 1 > joinIntEnd - joinIntDistance);
        CHECK(joinIntBegin + joinIntDistance - 1 <= joinIntEnd);
        CHECK(joinIntBegin + joinIntDistance - 1 >= joinIntEnd - 1);

        auto joinStrDistance = std::distance(joinStrBegin, joinStr.end());
        auto joinStrEnd = joinStr.end();
        CHECK(joinStrBegin < joinStrEnd);
        CHECK(joinStrBegin + joinStrDistance - 1 > joinStrEnd - joinStrDistance);
        CHECK(joinStrBegin + joinStrDistance - 1 <= joinStrEnd);
        CHECK(joinStrBegin + joinStrDistance - 1 >= joinStrEnd - 1);
    }
}
