#include <catch.hpp>

#include <Lz/Concatenate.hpp>
#include <iostream>


TEST_CASE("Concat changing and creating elements", "[Concat][Basic functionality]") {
    std::string a = "hello ";
    std::string b = "world";

    auto concat = lz::concat(a, b);

    SECTION("Should be by reference") {
        *concat.begin() = 'd';
        CHECK(a[0] == 'd');
    }

    SECTION("Should concat") {
        constexpr const char* expected = "hello world";
        CHECK(concat.to<std::basic_string>() == expected);
    }

    SECTION("Length should be correct") {
        auto dist = static_cast<size_t>(std::distance(concat.begin(), concat.end()));
        CHECK(dist == a.size() + b.size());
    }
}

TEST_CASE("Concat binary operations", "[Concat][Binary ops]") {
    std::string a = "hello ", b = "world";
    auto concat = lz::concat(a, b);
    auto begin = concat.begin();

    CHECK(*begin == 'h');

    SECTION("Operator++") {
        ++begin;
        CHECK(*begin == 'e');
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(*begin == 'h');
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != concat.end());
        CHECK(begin + (a.size() + b.size()) == concat.end());
    }

    SECTION("Operator+(int), tests += as well") {
        CHECK(*(begin + a.size()) == 'w');
    }

    SECTION("Operator-(int), tests -= as well") {
        begin += a.size();
        CHECK(*begin == 'w');
    }

    SECTION("Operator-(Iterator)") {
        CHECK(static_cast<size_t>(concat.end() - begin) == a.size() + b.size());
        CHECK(static_cast<size_t>(std::distance(concat.begin(), concat.end())) == a.size() + b.size());
    }

    SECTION("Operator[]()") {
        CHECK(begin[a.size()] == 'w');
    }

    SECTION("Operator<, '<, <=, >, >='") {
        const auto end = concat.end();
        const auto distance = std::distance(begin, end);

        CHECK(begin < end);
        CHECK(begin + distance - 1 > end - distance);
        CHECK(begin + distance - 1 <= end);
        CHECK(begin + 11 >= end - 1);
    }
}