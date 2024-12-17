#include <Lz/take_every.hpp>
#include <Lz/c_string.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>


TEST_CASE("take_every_iterable with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto take_every = lz::take_every(cstr, 2);
    static_assert(!std::is_same<decltype(take_every.begin()), decltype(take_every.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hlo");
    CHECK(lz::equal(take_every, expected));
}

TEST_CASE("take_every_iterable changing and creating elements", "[take_every_iterable][Basic functionality]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = lz::take_every(array, 2);
    auto iterator = take_every.begin();

    SECTION("take_every_iterable should be by reference") {
        *iterator = 0;
        CHECK(array[0] == 0);
    }

    SECTION("take_every_iterable should select every amount-th") {
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 3);
        ++iterator;
        CHECK(iterator == take_every.end());
    }

    SECTION("take_every_iterable should select every amount-th with skip first") {
        take_every = lz::take_every(array, 2, array.begin() + 1);
        iterator = take_every.begin();

        CHECK(*iterator == 2);
        ++iterator;
        CHECK(*iterator == 4);
        ++iterator;
        CHECK(iterator == take_every.end());
    }
}

TEST_CASE("take_every_iterable binary operations", "[take_every_iterable][Binary ops]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = lz::take_every(array, 3);
    auto iterator = take_every.begin();

    SECTION("Operator++") {
        CHECK(*iterator == 1);
        ++iterator;
        CHECK(*iterator == 4);
    }

    SECTION("Operator++ (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4 };
        auto te = lz::take_every(lst, 3);
        auto iter = te.begin();
        REQUIRE(*iter == 1);
        ++iter;
        CHECK(*iter == 4);
        ++iter;
        CHECK(iter == te.end());
    }

    SECTION("Operator--") {
        auto end = take_every.end();
        CHECK(*--end == 4);
        ++iterator;
        --iterator;
        CHECK(*iterator == 1);
    }

    SECTION("Operator-- (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4 };
        auto te = lz::take_every(lst, 3);
        auto iter = te.end();
        --iter;
        REQUIRE(*iter == 4);
        --iter;
        CHECK(*iter == 1);
        CHECK(iter == te.begin());
    }

    SECTION("Operator== & Operator!=") {
        CHECK(iterator != take_every.end());
        iterator = take_every.end();
        CHECK(iterator == take_every.end());
    }

    SECTION("Operator+(int), tests += as well") {
        std::array<int, 6> arr = { 1, 2, 3, 4, 5, 6 };
        auto take = lz::take_every(arr, 2);
        auto beg = take.begin();

        CHECK(*(beg + 2) == 5);
        CHECK(beg + 3 == take.end());
        ++beg;
        CHECK(*(beg + -1) == 1);

        auto end = take.end();
        CHECK(*(end + -1) == 6);
        CHECK(*(end + -2) == 4);
        CHECK(*(end + -3) == 2);
        CHECK((end + -4) == take.begin());
        --end;
        REQUIRE(*end == 6);
        --end;
        REQUIRE(*end == 4);

        CHECK(end + 2 == take.end());
    }

    SECTION("Operator-(int), tests -= as well") {
        CHECK(*(take_every.end() - 1) == 4);
        CHECK(*(take_every.end() - 2) == 1);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(take_every.end() - iterator == 2);
        CHECK(take_every.end() - (iterator + 1) == 1);
    }

    SECTION("Operator[]()") {
        CHECK(iterator[0] == 1);
        CHECK(iterator[1] == 4);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto distance = std::distance(take_every.begin(), take_every.end());

        CHECK(iterator < take_every.end());
        CHECK(iterator + distance - 1 > take_every.end() - distance);
        auto d = iterator + distance - 1;
        CHECK(d <= take_every.end());
        CHECK(iterator + distance - 1 <= take_every.end());
        CHECK(iterator + size - 1 >= take_every.end() - 1);
    }
}

TEST_CASE("take_every_iterable to containers", "[take_every_iterable][To container]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    constexpr std::size_t offset = 2;
    auto take_every = lz::take_every(array, offset);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size / offset)> actual = take_every.to<std::array<int, offset>>();
        CHECK(actual == std::array<int, offset>{ 1, 3 });
    }

    SECTION("To vector") {
        std::vector<int> actual = take_every.to_vector();
        CHECK(actual == std::vector<int>{ 1, 3 });
    }

    SECTION("To other container using to<>()") {
        std::list<int> actual = take_every.to<std::list<int>>();
        CHECK(actual == std::list<int>{ 1, 3 });
    }

    SECTION("To map") {
        std::map<int, int> actual = take_every.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = take_every.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        CHECK(actual == expected);
    }
}
