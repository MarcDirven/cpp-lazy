#include <Lz/CString.hpp>
#include <Lz/Join.hpp>
#include <Lz/Map.hpp>
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("Join with sentinels") {
    auto cString = lz::c_string("Hello, World!");
    auto join = lz::join(cString, ", ");
    CHECK(join.toString() == "H, e, l, l, o, ,,  , W, o, r, l, d, !");
    static_assert(!std::is_same<decltype(join.begin()), decltype(join.end())>::value, "Should be sentinel");
}

TEST_CASE("Join should convert to string", "[Join][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto joinInt = lz::join(v, ", ");
    static_assert(std::is_same<decltype(joinInt.begin()), decltype(joinInt.end())>::value, "Should not be sentinel");
    auto joinStr = lz::join(s, ", ");

    CHECK(joinInt.toString() == "1, 2, 3, 4, 5");
    CHECK(joinStr.toString() == "h, e, l, l, o");

    SECTION("String streams") {
        std::ostringstream ss;
        ss << joinInt;
        CHECK(ss.str() == "1, 2, 3, 4, 5");
    }

    SECTION("Should convert to string") {
        CHECK(*joinInt.begin() == "1");
        CHECK(*joinStr.begin() == "h");
    }

    SECTION("Type checking") {
        static_assert(std::is_same<decltype(*joinStr.begin()), std::string&>::value, "String container should be std::string&");
        static_assert(std::is_same<decltype(*joinInt.begin()), std::string>::value, "Int container should be std::string");
    }

    SECTION("Should be correct size") {
        CHECK(std::distance(joinInt.begin(), joinInt.end()) == 9);
        CHECK(std::distance(joinStr.begin(), joinStr.end()) == 9);
    }

    SECTION("Immediate to string") {
        CHECK(lz::str_join(v) == "12345");
    }
}

TEST_CASE("Join binary operations", "[Join][Binary ops]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto joinInt = lz::join(v, ", ");
    auto joinStr = lz::join(s, ", ");

    auto joinIntIter = joinInt.begin();
    auto joinStrIter = joinStr.begin();

    CHECK(*joinIntIter == "1");
    CHECK(*joinStrIter == "h");

    SECTION("Operator++") {
        ++joinIntIter;
        CHECK(*joinIntIter == ", ");

        ++joinStrIter;
        CHECK(*joinStrIter == ", ");
    }

    SECTION("Operator--") {
        ++joinIntIter, ++joinStrIter;

        CHECK(*--joinIntIter == "1");
        auto joinIntEnd = joinInt.end();
        CHECK(*--joinIntEnd == "5");
        CHECK(*--joinIntEnd == ", ");

        --joinStrIter;
        CHECK(*joinStrIter == "h");
    }

    SECTION("Operator== & operator!=") {
        CHECK(joinIntIter != joinInt.end());
        CHECK(joinStrIter != joinStr.end());

        joinIntIter = joinInt.end();
        joinStrIter = joinStr.end();

        CHECK(joinIntIter == joinInt.end());
        CHECK(joinStrIter == joinStr.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(joinIntIter + 2) == "2");
        CHECK(*(joinStrIter + 2) == "e");

        CHECK(*(joinIntIter + 3) == ", ");
        CHECK(*(joinStrIter + 3) == ", ");
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        joinIntIter = joinInt.end();
        joinStrIter = joinStr.end();

        CHECK(*(joinIntIter - 1) == "5");
        CHECK(*(joinStrIter - 1) == "o");

        CHECK(*(joinIntIter - 2) == ", ");
        CHECK(*(joinStrIter - 2) == ", ");

        joinIntIter = joinInt.begin();
        CHECK(*(joinIntIter - -3) == ", ");
        CHECK(*(joinIntIter - -2) == "2");

        joinIntIter = joinInt.end();
        CHECK(*(joinIntIter + -3) == "4");
        CHECK(*(joinIntIter + -4) == ", ");
    }

    SECTION("Operator-(Iterator)") {
        CHECK(std::distance(joinIntIter, joinInt.end()) == 9);
        CHECK(std::distance(joinStrIter, joinStr.end()) == 9);

        CHECK(joinInt.end() - joinIntIter == 9);
        CHECK(joinStr.end() - joinStrIter == 9);
    }

    SECTION("Operator[]()") {
        CHECK(joinIntIter[2] == "2");
        CHECK(joinIntIter[1] == ", ");

        CHECK(joinStrIter[2] == "e");
        CHECK(joinStrIter[3] == ", ");
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto joinIntDistance = std::distance(joinIntIter, joinInt.end());
        auto joinIntEnd = joinInt.end();
        CHECK(joinIntIter < joinIntEnd);
        CHECK(joinIntIter + joinIntDistance - 1 > joinIntEnd - joinIntDistance);
        CHECK(joinIntIter + joinIntDistance - 1 <= joinIntEnd);
        CHECK(joinIntIter + joinIntDistance - 1 >= joinIntEnd - 1);

        auto joinStrDistance = std::distance(joinStrIter, joinStr.end());
        auto joinStrEnd = joinStr.end();
        CHECK(joinStrIter < joinStrEnd);
        CHECK(joinStrIter + joinStrDistance - 1 > joinStrEnd - joinStrDistance);
        CHECK(joinStrIter + joinStrDistance - 1 <= joinStrEnd);
        CHECK(joinStrIter + joinStrDistance - 1 >= joinStrEnd - 1);
    }

    SECTION("String join double format") {
        std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
        auto doubles = lz::str_join(vec, ", ", "{:.2f}");
        CHECK(doubles == "1.10, 2.20, 3.30, 4.40");
    }
}
