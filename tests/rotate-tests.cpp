#include <Lz/CString.hpp>
#include <Lz/IterTools.hpp>
#include <Lz/Rotate.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("rotate_iterable with sentinels") {
    auto cStr = lz::c_string("Hello, World!");
    auto rotated = lz::rotate(cStr, std::next(cStr.begin(), 7));
    static_assert(!std::is_same<decltype(rotated.begin()), decltype(rotated.end())>::value, "Should be sentinel");
    CHECK(rotated.toString() == "World!Hello, ");
}

TEST_CASE("rotate_iterable basic functionality", "[rotate_iterable][Basic functionality]") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr, arr.begin() + 2);

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
        auto rotator = lz::rotate(lst, std::next(lst.begin(), 2));
        CHECK(std::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
    }
}

TEST_CASE("rotate_iterable binary operations", "[rotate_iterable][Binary ops]") {
    std::array<int, 4> arr = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(arr, arr.begin() + 3);

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
        auto rotator = lz::rotate(container, container.begin() + 3);

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

TEST_CASE("rotate_iterable to containers", "[rotate_iterable][To container]") {
    constexpr std::size_t size = 6;
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto rotator = lz::rotate(vec, vec.begin() + 2);

    SECTION("To array") {
        CHECK(rotator.to<std::array<int, size>>() == std::array<int, size>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To vector") {
        CHECK(rotator.to_vector() == std::vector<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To other container using to<>()") {
        CHECK(rotator.to<std::list<int>>() == std::list<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To map") {
        auto map = rotator.to_map([](int i) { return std::make_pair(i, i); });
        CHECK(map == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }

    SECTION("To unordered map") {
        auto map = rotator.to_unordered_map([](int i) { return std::make_pair(i, i); });
        CHECK(map == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }
}
