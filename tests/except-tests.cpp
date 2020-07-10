#include <Lz/Except.hpp>
#include <Lz/Range.hpp>
#include <list>

#include <catch.hpp>


TEST_CASE("Except excepts elements and is by reference", "[Except][Basic functionality]") {
    constexpr size_t size = 5;
    std::vector<int> array{1, 2, 3, 4, 5};
    std::vector<int> toExcept{3, 5};

    auto except = lz::except(array, toExcept);
    auto it = except.begin();
    CHECK(*it == 1);

    SECTION("For-loop") {
        constexpr size_t s = 32;
        constexpr size_t es = 16;

        std::array<int, s> largeArr = lz::range((int)s).toArray<s>();
        std::array<int, es> toLargeExcept = lz::range((int)es).toArray<es>();

        auto ex = lz::except(largeArr, toLargeExcept);
        for (int _ : ex) {
            static_cast<void>(_);
        }
    }

    SECTION("Excepts elements") {
        CHECK(except.toVector() == std::vector<int>{1, 2, 4});
    }

    SECTION("Is by reference") {
        *it = 0;
        CHECK(*it == array[0]);
    }
}

TEST_CASE("Except binary operations", "[Except][Binary ops]") {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<int> b = {2, 3};

    auto except = lz::except(a, b);
    auto it = except.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 4);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != except.end());
        it = except.end();
        CHECK(it == except.end());
    }
}

TEST_CASE("Except to containers", "[Except][To container]") {
    constexpr size_t size = 4;
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<int> b = {1, 3};
    auto except = lz::except(a, b);

    SECTION("To array") {
        auto excepted = except.toArray<2>();
        CHECK(excepted == std::array<int, 2>{2, 4});
    }

    SECTION("To vector") {
        auto excepted = except.toVector();
        CHECK(excepted == std::vector<int>{2, 4});
    }

    SECTION("To other container using to<>()") {
        auto excepted = except.to<std::list>();
        CHECK(excepted == std::list<int>{2, 4});
    }
}
