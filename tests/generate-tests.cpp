#define CATCH_CONFIG_MAIN

#include <Lz/Generate.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Generate changing and creating elements", "[Generate][Basic functionality]") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
    auto generator = lz::generate([&counter] { return counter++; }, amount);

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
    std::function<std::size_t()> f = [&counter] {
        return counter++;
    };
    auto generator = lz::generate(std::move(f), amount);
    auto begin = generator.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(std::distance(begin, generator.end()) == amount - 1);
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(std::distance(begin, generator.end()) == amount);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != generator.end());
        begin = generator.end();
        CHECK(begin == generator.end());
    }

    SECTION("Operator+(int), tests += as well") {
        const std::size_t toAdd = 2;
        begin += toAdd;
        CHECK(std::distance(begin, generator.end()) == amount - toAdd);
    }

    SECTION("Operator-(int), tests -= as well") {
        ++begin;
        begin -= 1;
        CHECK(std::distance(begin, generator.end()) == amount);
    }

    SECTION("Operator-(Iterator)") {
        CHECK((generator.end() - begin) == amount);
        CHECK(std::distance(begin, generator.end()) == amount);
    }

    SECTION("Operator[]()") {
        CHECK(begin[0] == 0);
        CHECK(begin[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(begin < generator.end());
        CHECK(begin + amount + 1 > generator.end());
        CHECK(begin + amount <= generator.end());
        CHECK(begin + amount >= generator.end());
    }
}

TEST_CASE("Generate to containers", "[Generate][To container]") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
#if defined(__llvm__) || defined(__clang__)
    auto generator = lz::generate(static_cast<std::function<std::size_t()>>([&counter] { return counter++; }), amount);
#else
    auto generator = lz::generate([&counter] { return counter++; }, amount);
#endif

    SECTION("To array") {
        std::array<std::size_t, amount> array = generator.toArray<amount>();
        std::array<std::size_t, amount> expected = { 0, 1, 2, 3 };

        CHECK(array == expected);
    }

    SECTION("To vector") {
        std::vector<std::size_t> vector = generator.toVector();
        std::vector<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::size_t> vector = generator.to<std::list>();
        std::list<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To map") {
        std::map<std::size_t, std::size_t> map = generator.toMap([](const std::size_t elm) { return elm * 10; });

        std::map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::size_t, std::size_t> map =
            generator.toUnorderedMap([](const std::size_t elm) { return elm * 10; });

        std::unordered_map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }
}