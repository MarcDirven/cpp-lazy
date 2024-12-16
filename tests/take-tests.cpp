#include <Lz/CString.hpp>
#include <Lz/Drop.hpp>
#include <Lz/Slice.hpp>
#include <Lz/Take.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Take with sentinels") {
    const char* str = "Hello, world!";
    auto cString = lz::c_string(str);
    auto take = lz::take(cString, 5);
    static_assert(!std::is_same<decltype(take.begin()), decltype(take.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hello");
    REQUIRE(lz::equal(take, expected));
}

TEST_CASE("Take changing and creating elements", "[Take][Basic functionality]") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(*take.begin() == 1);
    REQUIRE(*std::next(take.begin()) == 2);
    REQUIRE(*std::prev(take.end()) == 4);
    *take.begin() = 0;
    REQUIRE(array[0] == 0);
}

TEST_CASE("Take binary operations", "[Take][Binary ops]") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);

    SECTION("Operator++") {
        auto begin = take.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator--") {
        auto end = take.end();
        --end;
        REQUIRE(*end == 4);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE_FALSE(take.begin() == take.end());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        auto begin = take.begin();
        REQUIRE(*(begin + 2) == 3);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        auto end = take.end();
        REQUIRE(*(end - 1) == 4);
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE((take.end() - take.begin()) == 4);
    }

    SECTION("Operator[]()") {
        REQUIRE(take.begin()[0] == 1);
        REQUIRE(take.begin()[1] == 2);
        REQUIRE(take.begin()[2] == 3);
    }

    SECTION("Operator<, <, <=, >, >=") {
        REQUIRE(take.begin() < take.end());
        REQUIRE(take.begin() + 1 > take.begin());
        REQUIRE(take.begin() + 4 <= take.end());
        REQUIRE(take.begin() + 4 >= take.end());
    }
}

TEST_CASE("Take to containers", "[Take][To container]") {
    constexpr std::size_t size = 8;
    std::array<int, size> array = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto takeEvery = lz::take(array, 4);

    SECTION("To array") {
        auto arr = takeEvery.to<std::array<int, 4>>();
        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("To vector") {
        auto vec = takeEvery.to_vector();
        REQUIRE(std::equal(vec.begin(), vec.end(), takeEvery.begin()));
    }

    SECTION("To other container using to<>()") {
        auto lst = takeEvery.to<std::list<int>>();
        REQUIRE(std::equal(lst.begin(), lst.end(), takeEvery.begin()));
    }

    SECTION("To map") {
        auto map = takeEvery.to_map([](int i) { return std::make_pair(i, i); });
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = takeEvery.to_unordered_map([](int i) { return std::make_pair(i, i); });
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}

TEST_CASE("Drop & slice") {
    SECTION("Drop iterable") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = lz::drop(vec, 4);
        std::vector<int> expected = { 5, 6, 7, 8 };
        REQUIRE(std::equal(drop.begin(), drop.end(), expected.begin()));
    }

    SECTION("Drop iterator") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = lz::drop(vec, 4);
        std::vector<int> expected = { 5, 6, 7, 8 };
        REQUIRE(std::equal(drop.begin(), drop.end(), expected.begin()));
    }

    SECTION("Slice iterable") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto slice = lz::slice(vec, 2, 6);
        std::vector<int> expected = { 3, 4, 5, 6 };
        REQUIRE(std::equal(slice.begin(), slice.end(), expected.begin()));
    }

    SECTION("Slice iterator") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto slice = lz::slice(vec, 2, 6);
        std::vector<int> expected = { 3, 4, 5, 6 };
        REQUIRE(std::equal(slice.begin(), slice.end(), expected.begin()));
    }
}