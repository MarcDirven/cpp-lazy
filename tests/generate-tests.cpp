#include <Lz/Generate.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Generate changing and creating elements", "[Generate][Basic functionality]") {
    auto compileTest = lz::generate([]() { return 0; });
    static_cast<void>(compileTest);

    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
    auto generator = lz::generate(
        [](std::size_t& c) {
            auto tmp{ c++ };
            return tmp;
        },
        amount, counter);

    SECTION("Should be 0, 1, 2, 3") {
        std::size_t expected = 0;
        for (std::size_t i : generator) {
            CHECK(i == expected++);
        }
    }
}

TEST_CASE("Generate binary operations", "[Generate][Binary ops]") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
    auto generator = lz::generate(
        [](std::size_t& c) {
            auto tmp{ c++ };
            return tmp;
        },
        amount, counter);
    auto begin = generator.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(lz::distance(begin, generator.end()) == amount - 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != generator.end());
        while (begin != generator.end()) {
            ++begin;
        }
        CHECK(begin == generator.end());
    }
}

TEST_CASE("Generate to containers", "[Generate][To container]") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;

    auto generator = lz::generate(
        [](std::size_t& c) {
            auto tmp{ c++ };
            return tmp;
        },
        amount, counter);

    SECTION("To array") {
        auto array = generator.to<std::array<std::size_t, amount>>();
        std::array<std::size_t, amount> expected = { 0, 1, 2, 3 };

        CHECK(array == expected);
    }

    SECTION("To vector") {
        std::vector<std::size_t> vector = generator.toVector();
        std::vector<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::size_t> vector = generator.to<std::list<std::size_t>>();
        std::list<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To map") {
        std::map<std::size_t, std::size_t> map =
            generator.toMap([](const std::size_t elm) { return std::make_pair(elm * 10, elm); });

        std::map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::size_t, std::size_t> map =
            generator.toUnorderedMap([](const std::size_t elm) { return std::make_pair(elm * 10, elm); });

        std::unordered_map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }
}