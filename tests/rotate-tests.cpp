#include "Lz/FunctionTools.hpp"
#include "Lz/Rotate.hpp"

#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Rotate basic functionality", "[Rotate][Basic functionality]") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr, 2);

    SECTION("Should be correct length") {
        CHECK(std::distance(rotate.begin(), rotate.end()) == static_cast<std::ptrdiff_t>(arr.size()));
    }

    SECTION("With bidirectional iterator") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto rotator = lz::rotate(lst, 2);
        CHECK(lz::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
    }
}

TEST_CASE("Rotate binary operations", "[Rotate][Binary ops]") {
    std::array<int, 4> arr = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(arr, 3);
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

    SECTION("Operator+(int), tests += as well") {
        std::array<int, 5> container = { 1, 2, 3, 4, 5 };

        auto rotator = lz::rotate(container, 3);
        auto unEvenBegin = rotator.begin();
        auto unEvenEnd = rotator.end();

        CHECK(*(unEvenBegin + 3) == 2);

        ++unEvenBegin;
        REQUIRE(*unEvenBegin == 5);

        CHECK(*(unEvenBegin + 1) == 1);
        CHECK(*(unEvenBegin + 2) == 2);
        CHECK(*(unEvenBegin + 3) == 3);
        CHECK(unEvenBegin + 4 == rotator.end());

        ++unEvenBegin;
        REQUIRE(*unEvenBegin == 1);
        CHECK(*(unEvenBegin + -2) == 4);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 3);
        CHECK(unEvenEnd + 1 == rotator.end());

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 2);
        CHECK(*(unEvenEnd + 1) == 3);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 1);
        CHECK(*(unEvenEnd + 1) == 2);
        CHECK(*(unEvenEnd + 2) == 3);

        --unEvenEnd;
        REQUIRE(*unEvenEnd == 5);
        CHECK(*(unEvenEnd + 3) == 3);
    }

    SECTION("Operator-(int), tests -= as well") {
        std::array<int, 5> container = { 1, 2, 3, 4, 5 };
        auto rotator = lz::rotate(container, 3);
        auto beg = rotator.begin();
        auto en = rotator.end();

        CHECK(*(en - 3) == 1);
        CHECK(*(en - 5) == 4);

        --en, --en;
        REQUIRE(*en == 2);
        CHECK(*(en - 2) == 5);

        --en;
        REQUIRE(*en == 1);
        CHECK(*(en - 1) == 5);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(rotate.end() - rotate.begin() == 4);
        const auto e = --rotate.end();
        CHECK(e - rotate.begin() == 3);
    }

    SECTION("Operator[]()") {
        CHECK(begin[0] == 4);
        CHECK(begin[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(begin < end);
        CHECK(begin + 4 <= end);
        CHECK(end > begin);
        CHECK(end - 4 >= begin);
    }
}

TEST_CASE("Rotate to containers", "[Rotate][To container]") {
    constexpr std::size_t size = 6;
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto rotator = lz::rotate(vec, 2);

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
