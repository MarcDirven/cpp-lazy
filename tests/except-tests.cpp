#include <Lz/except.hpp>
#include <Lz/range.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Except tests with sentinels") {
    const char* str = "Hello, World!";
    const char* to_except = "eo";
    auto c_str = lz::c_string(str);
    auto c_str_to_except = lz::c_string(to_except);
    auto except = lz::except(c_str, c_str_to_except);
    static_assert(!std::is_same<decltype(except.begin()), decltype(except.end())>::value, "Must be sentinel");
    CHECK(except.to<std::string>() == "Hll, Wrld!");
}

TEST_CASE("Except excepts elements and is by reference", "[Except][Basic functionality]") {
    std::vector<int> array{ 1, 2, 3, 4, 5 };
    std::vector<int> to_except{ 3, 5 };

    auto except = lz::except(array, to_except);
    auto it = except.begin();
    CHECK(*it == 1);

    SECTION("For-loop") {
        constexpr std::size_t s = 32;
        constexpr std::size_t es = 16;

        std::array<int, s> large_arr = lz::range(static_cast<int>(s)).to<std::array<int, s>>();
        std::array<int, es> to_large_except = lz::range(static_cast<int>(es)).to<std::array<int, es>>();

        auto ex = lz::except(large_arr, to_large_except);
        for (int i : ex) {
            static_cast<void>(i);
        }
    }

    SECTION("Excepts elements") {
        CHECK(except.to_vector() == std::vector<int>{ 1, 2, 4 });
    }

    SECTION("Is by reference") {
        *it = 0;
        CHECK(*it == array[0]);
    }

    SECTION("Excepted with >") {
        std::sort(to_except.begin(), to_except.end(), std::greater<int>());
        auto except_greater = lz::except(array, to_except, std::greater<int>());
        CHECK(except_greater.to<std::array<int, 3>>() == std::array<int, 3>{ 1, 2, 4 });
    }
}

TEST_CASE("Except binary operations", "[Except][Binary ops]") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<int> b = { 2, 3 };

    auto except = lz::except(a, b);
    auto it = except.begin();
    CHECK(*it == 1);

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 4);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != except.end());
        while (it != except.end()) {
            ++it;
        }
        CHECK(it == except.end());
    }
}

TEST_CASE("Except to containers", "[Except][To container]") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<int> b = { 1, 3 };
    auto except = lz::except(a, b);

    SECTION("To array") {
        auto excepted = except.to<std::array<int, 2>>();
        CHECK(excepted == std::array<int, 2>{ 2, 4 });
    }

    SECTION("To vector") {
        auto excepted = except.to_vector();
        CHECK(excepted == std::vector<int>{ 2, 4 });
    }

    SECTION("To other container using to<>()") {
        auto excepted = except.to<std::list<int>>();
        CHECK(excepted == std::list<int>{ 2, 4 });
    }

    SECTION("To map") {
        std::map<int, int> actual = except.to_map([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = except.to_unordered_map([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        CHECK(actual == expected);
    }
}
