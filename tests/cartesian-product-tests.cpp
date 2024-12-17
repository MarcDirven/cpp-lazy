#include "Lz/algorithm.hpp"
#include "Lz/c_string.hpp"
#include "Lz/cartesian_product.hpp"

#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Is sentinel") {
    const char* str = "Hello,";
    const char* str2 = " World!";
    auto cstr1 = lz::c_string(str);
    auto cstr2 = lz::c_string(str2);
    auto cart = lz::cartesian_product(cstr1, cstr2);

    SECTION("Should be sentinel") {
        static_assert(!std::is_same<decltype(cart.begin()), decltype(cart.end())>::value, "Should not be the same");
    }

    SECTION("Should be correct length") {
        CHECK(static_cast<std::size_t>(lz::distance(cart.begin(), cart.end())) == std::strlen(str) * std::strlen(str2));
    }

    SECTION("Should make permutation") {
        CHECK(*cart.begin() == std::make_tuple('H', ' '));
    }
}

TEST_CASE("Cartesian product changing and creating elements", "[CartesianProduct][Basic functionality]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);

    SECTION("Should not be sentinel") {
        static_assert(std::is_same<decltype(cartesian.begin()), decltype(cartesian.end())>::value, "Should be the same");
    }

    SECTION("Should be by reference") {
        auto& elm_vec = std::get<0>(*cartesian.begin());
        auto& elm_chars = std::get<1>(*cartesian.begin());

        CHECK(&elm_vec == &vec[0]);
        CHECK(&elm_chars == &chars[0]);
    }

    SECTION("Should make combinations") {
        CHECK(*cartesian.begin() == std::make_tuple(1, 'a'));
        CHECK(*++cartesian.begin() == std::make_tuple(1, 'b'));
    }

    SECTION("Should be correct length") {
        CHECK(std::distance(cartesian.begin(), cartesian.end()) == static_cast<std::ptrdiff_t>(vec.size() * chars.size()));
    }
}

TEST_CASE("Cartesian product binary operations", "[CartesianProduct][Binary ops]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);

    SECTION("Operator++") {
        auto begin = cartesian.begin();
        ++begin;
        CHECK(*begin == std::make_tuple(1, 'b'));
        ++begin, ++begin;
        CHECK(*begin == std::make_tuple(2, 'a'));
    }

    SECTION("Operator--") {
        std::vector<int> tmp_vec = { 1, 2, 3 };
        std::vector<char> tmp_chars = { 'a', 'b', 'c' };
        auto cart = lz::cartesian_product(tmp_vec, tmp_chars);
        auto end = cart.end();

        --end;
        CHECK(*end == std::make_tuple(3, 'c'));

        --end;
        CHECK(*end == std::make_tuple(3, 'b'));

        --end;
        CHECK(*end == std::make_tuple(3, 'a'));

        --end;
        CHECK(*end == std::make_tuple(2, 'c'));

        --end /*2, 'b'*/, --end /*2, 'a'*/, --end /*1, 'c'*/, --end, /*1, 'b'*/ --end /*1, 'a'*/;
        CHECK(end == cart.begin());

        auto beg = cart.begin();
        ++beg;
        --beg;
        CHECK(*beg == std::make_tuple(1, 'a'));
    }

    SECTION("Operator== & operator!=") {
        auto it = cartesian.begin();
        CHECK(it != cartesian.end());
        it = cartesian.end();
        CHECK(it == cartesian.end());
    }

    SECTION("Operator+(int), tests += as well") {
        auto iter = cartesian.begin();
        CHECK(*(iter + 1) == std::make_tuple(1, 'b'));
        CHECK(*(iter + 2) == std::make_tuple(1, 'c'));
        CHECK(*(iter + 3) == std::make_tuple(2, 'a'));
        CHECK(*(iter + 6) == std::make_tuple(3, 'a'));
        CHECK(*(iter + 8) == std::make_tuple(3, 'c'));
        CHECK(iter + 9 == cartesian.end());
    }

    SECTION("Operator-(int), tests -= as well") {
        auto iter = cartesian.begin();
        iter += 8;
        CHECK(*(iter - 3) == std::make_tuple(2, 'c'));
        CHECK(*(iter - 2) == std::make_tuple(3, 'a'));
        CHECK(*(iter - 1) == std::make_tuple(3, 'b'));
        CHECK(*(iter - 8) == std::make_tuple(1, 'a'));
        CHECK(iter - 8 == cartesian.begin());
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = cartesian.begin();
        auto end = cartesian.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + 9 - 1 >= end - 1);
    }
}

TEST_CASE("CartesianProduct to containers", "[CartesianProduct][To container]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);

    SECTION("To array") {
        constexpr std::size_t size = 9;
        auto result = cartesian.to<std::array<std::tuple<int, char>, size>>();
        std::array<std::tuple<int, char>, size> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To vector") {
        std::vector<std::tuple<int, char>> result = cartesian.to_vector();
        std::vector<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To other container using to<>()") {
        auto result = cartesian.to<std::list<std::tuple<int, char>>>();
        std::list<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To map") {
        std::map<int, std::tuple<int, char>> result =
            cartesian.to_map([](const typename decltype(cartesian)::iterator::value_type& v) {
                return std::make_pair(std::get<0>(v) + std::get<1>(v), v);
            });
        decltype(result) expected = {
            { 1 + 'a', std::make_tuple(1, 'a') }, { 1 + 'b', std::make_tuple(1, 'b') }, { 1 + 'c', std::make_tuple(1, 'c') },

            { 2 + 'a', std::make_tuple(2, 'a') }, { 2 + 'b', std::make_tuple(2, 'b') }, { 2 + 'c', std::make_tuple(2, 'c') },

            { 3 + 'a', std::make_tuple(3, 'a') }, { 3 + 'b', std::make_tuple(3, 'b') }, { 3 + 'c', std::make_tuple(3, 'c') }
        };
        CHECK(result == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, std::tuple<int, char>> result =
            cartesian.to_unordered_map([](const typename decltype(cartesian)::iterator::value_type& v) {
                return std::make_pair(std::get<0>(v) + std::get<1>(v), v);
            });
        decltype(result) expected = {
            { 1 + 'a', std::make_tuple(1, 'a') }, { 1 + 'b', std::make_tuple(1, 'b') }, { 1 + 'c', std::make_tuple(1, 'c') },

            { 2 + 'a', std::make_tuple(2, 'a') }, { 2 + 'b', std::make_tuple(2, 'b') }, { 2 + 'c', std::make_tuple(2, 'c') },

            { 3 + 'a', std::make_tuple(3, 'a') }, { 3 + 'b', std::make_tuple(3, 'b') }, { 3 + 'c', std::make_tuple(3, 'c') }
        };
        CHECK(result == expected);
    }
}