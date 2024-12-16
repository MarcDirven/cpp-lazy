#include <Lz/Except.hpp>
#include <Lz/Range.hpp>
#include <Lz/CString.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Except tests with sentinels") {
    const char* str = "Hello, World!";
    const char* toExcept = "eo";
    auto cStr = lz::cString(str);
    auto cStrToExcept = lz::cString(toExcept);
    auto except = lz::except(cStr, cStrToExcept);
    static_assert(!std::is_same<decltype(except.begin()), decltype(except.end())>::value, "Must be sentinel");
    CHECK(except.to<std::string>() == "Hll, Wrld!");
}

TEST_CASE("Except excepts elements and is by reference", "[Except][Basic functionality]") {
    std::vector<int> array{ 1, 2, 3, 4, 5 };
    std::vector<int> toExcept{ 3, 5 };

    auto except = lz::except(array, toExcept);
    auto it = except.begin();
    CHECK(*it == 1);

    SECTION("For-loop") {
        constexpr std::size_t s = 32;
        constexpr std::size_t es = 16;

        std::array<int, s> largeArr = lz::range(static_cast<int>(s)).to<std::array<int, s>>();
        std::array<int, es> toLargeExcept = lz::range(static_cast<int>(es)).to<std::array<int, es>>();

        auto ex = lz::except(largeArr, toLargeExcept);
        for (int i : ex) {
            static_cast<void>(i);
        }
    }

    SECTION("Excepts elements") {
        CHECK(except.toVector() == std::vector<int>{ 1, 2, 4 });
    }

    SECTION("Is by reference") {
        *it = 0;
        CHECK(*it == array[0]);
    }

    SECTION("Excepted with >") {
        std::sort(toExcept.begin(), toExcept.end(), std::greater<int>());
        auto exceptGreater = lz::except(array, toExcept, std::greater<int>());
        CHECK(exceptGreater.to<std::array<int, 3>>() == std::array<int, 3>{ 1, 2, 4 });
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
        auto excepted = except.toVector();
        CHECK(excepted == std::vector<int>{ 2, 4 });
    }

    SECTION("To other container using to<>()") {
        auto excepted = except.to<std::list<int>>();
        CHECK(excepted == std::list<int>{ 2, 4 });
    }

    SECTION("To map") {
        std::map<int, int> actual = except.toMap([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = except.toUnorderedMap([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        CHECK(actual == expected);
    }
}
