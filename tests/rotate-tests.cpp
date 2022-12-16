#define CATCH_CONFIG_MAIN

#include "Lz/FunctionTools.hpp"
#include "Lz/Rotate.hpp"

#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Rotate basic functionality", "[Rotate][Basic functionality]") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr.begin() + 2, arr.begin(), arr.end());

    SECTION("Should be correct length") {
        auto beg = rotate.begin();
        CHECK(std::distance(beg, rotate.end()) == static_cast<std::ptrdiff_t>(arr.size()));
        ++beg, ++beg;
        CHECK(std::distance(beg, rotate.end()) == 3);
        ++beg;
        CHECK(std::distance(beg, rotate.end()) == 2);
    }

    SECTION("With bidirectional iterator") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto rotator = lz::rotate(std::next(lst.begin(), 2), lst.begin(), lst.end());
        CHECK(std::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
    }
}

TEST_CASE("Rotate binary operations", "[Rotate][Binary ops]") {
    std::array<int, 4> arr = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(arr.begin() + 3, arr.begin(), arr.end());

    auto begin = rotate.begin();
    auto end = rotate.end();

    REQUIRE(*begin == 4);
    REQUIRE(*end == 4);

    SECTION("Operator++") {
        ++begin;
        CHECK(*begin == 1);
        ++end;
        CHECK(*end == 1);
    }

    SECTION("Operator--") {
        --begin;
        CHECK(*begin == 3);
        --end;
        CHECK(*end == 3);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != end);
        begin = end;
        CHECK(begin == end);
    }

    SECTION("Various ++ and -- operators with begin and end") {
        std::array<int, 5> container = { 1, 2, 3, 4, 5 };
        auto rotator = lz::rotate(container.begin() + 3, container.begin(), container.end());

        auto unEvenBegin = rotator.begin();
        auto unEvenEnd = rotator.end();

        ++unEvenBegin;
        REQUIRE(*unEvenBegin == 5);
        --unEvenBegin;
        REQUIRE(*unEvenBegin == 4);
        ++unEvenBegin;

        ++unEvenBegin;
        REQUIRE(*unEvenBegin == 1);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 3);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 2);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 1);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 5);
    }
}

TEST_CASE("Rotate to containers", "[Rotate][To container]") {
    constexpr std::size_t size = 6;
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto rotator = lz::rotate(vec.begin() + 2, vec.begin(), vec.end());

    SECTION("To array") {
        CHECK(rotator.toArray<size>() == std::array<int, size>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To vector") {
        CHECK(rotator.toVector() == std::vector<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To other container using to<>()") {
        CHECK(rotator.to<std::list>() == std::list<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To map") {
        auto map = rotator.toMap([](int i) { return i; });
        CHECK(map == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }

    SECTION("To unordered map") {
        auto map = rotator.toUnorderedMap([](int i) { return i; });
        CHECK(map == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }
}
