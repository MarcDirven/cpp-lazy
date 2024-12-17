#include <Lz/join.hpp>
#include <Lz/map.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
#include <sstream>


TEST_CASE("Join with sentinels") {
    auto c_string = lz::c_string("Hello, World!");
    auto join = lz::join(c_string, ", ");
    CHECK(join.to_string() == "H, e, l, l, o, ,,  , W, o, r, l, d, !");
    static_assert(!std::is_same<decltype(join.begin()), decltype(join.end())>::value, "Should be sentinel");
}

TEST_CASE("Join should convert to string", "[Join][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto join_int = lz::join(v, ", ");
    static_assert(std::is_same<decltype(join_int.begin()), decltype(join_int.end())>::value, "Should not be sentinel");
    auto join_str = lz::join(s, ", ");

    CHECK(join_int.to_string() == "1, 2, 3, 4, 5");
    CHECK(join_str.to_string() == "h, e, l, l, o");

    SECTION("String streams") {
        std::ostringstream ss;
        ss << join_int;
        CHECK(ss.str() == "1, 2, 3, 4, 5");
    }

    SECTION("Should convert to string") {
        CHECK(*join_int.begin() == "1");
        CHECK(*join_str.begin() == "h");
    }

    SECTION("Type checking") {
        static_assert(std::is_same<decltype(*join_str.begin()), std::string&>::value, "String container should be std::string&");
        static_assert(std::is_same<decltype(*join_int.begin()), std::string>::value, "Int container should be std::string");
    }

    SECTION("Should be correct size") {
        CHECK(std::distance(join_int.begin(), join_int.end()) == 9);
        CHECK(std::distance(join_str.begin(), join_str.end()) == 9);
    }

    SECTION("Immediate to string") {
        CHECK(lz::str_join(v) == "12345");
    }
}

TEST_CASE("Join binary operations", "[Join][Binary ops]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto join_int = lz::join(v, ", ");
    auto join_str = lz::join(s, ", ");

    auto join_int_iter = join_int.begin();
    auto join_str_iter = join_str.begin();

    CHECK(*join_int_iter == "1");
    CHECK(*join_str_iter == "h");

    SECTION("Operator++") {
        ++join_int_iter;
        CHECK(*join_int_iter == ", ");

        ++join_str_iter;
        CHECK(*join_str_iter == ", ");
    }

    SECTION("Operator--") {
        ++join_int_iter, ++join_str_iter;

        CHECK(*--join_int_iter == "1");
        auto join_int_end = join_int.end();
        CHECK(*--join_int_end == "5");
        CHECK(*--join_int_end == ", ");

        --join_str_iter;
        CHECK(*join_str_iter == "h");
    }

    SECTION("Operator== & operator!=") {
        CHECK(join_int_iter != join_int.end());
        CHECK(join_str_iter != join_str.end());

        join_int_iter = join_int.end();
        join_str_iter = join_str.end();

        CHECK(join_int_iter == join_int.end());
        CHECK(join_str_iter == join_str.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(join_int_iter + 2) == "2");
        CHECK(*(join_str_iter + 2) == "e");

        CHECK(*(join_int_iter + 3) == ", ");
        CHECK(*(join_str_iter + 3) == ", ");
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        join_int_iter = join_int.end();
        join_str_iter = join_str.end();

        CHECK(*(join_int_iter - 1) == "5");
        CHECK(*(join_str_iter - 1) == "o");

        CHECK(*(join_int_iter - 2) == ", ");
        CHECK(*(join_str_iter - 2) == ", ");

        join_int_iter = join_int.begin();
        CHECK(*(join_int_iter - -3) == ", ");
        CHECK(*(join_int_iter - -2) == "2");

        join_int_iter = join_int.end();
        CHECK(*(join_int_iter + -3) == "4");
        CHECK(*(join_int_iter + -4) == ", ");
    }

    SECTION("Operator-(Iterator)") {
        CHECK(std::distance(join_int_iter, join_int.end()) == 9);
        CHECK(std::distance(join_str_iter, join_str.end()) == 9);

        CHECK(join_int.end() - join_int_iter == 9);
        CHECK(join_str.end() - join_str_iter == 9);
    }

    SECTION("Operator[]()") {
        CHECK(join_int_iter[2] == "2");
        CHECK(join_int_iter[1] == ", ");

        CHECK(join_str_iter[2] == "e");
        CHECK(join_str_iter[3] == ", ");
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto join_int_dist = std::distance(join_int_iter, join_int.end());
        auto join_int_end = join_int.end();
        CHECK(join_int_iter < join_int_end);
        CHECK(join_int_iter + join_int_dist - 1 > join_int_end - join_int_dist);
        CHECK(join_int_iter + join_int_dist - 1 <= join_int_end);
        CHECK(join_int_iter + join_int_dist - 1 >= join_int_end - 1);

        auto join_str_dist = std::distance(join_str_iter, join_str.end());
        auto join_str_end = join_str.end();
        CHECK(join_str_iter < join_str_end);
        CHECK(join_str_iter + join_str_dist - 1 > join_str_end - join_str_dist);
        CHECK(join_str_iter + join_str_dist - 1 <= join_str_end);
        CHECK(join_str_iter + join_str_dist - 1 >= join_str_end - 1);
    }

    SECTION("String join double format") {
        std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
        auto doubles = lz::str_join(vec, ", ", "{:.2f}");
        CHECK(doubles == "1.10, 2.20, 3.30, 4.40");
    }
}
