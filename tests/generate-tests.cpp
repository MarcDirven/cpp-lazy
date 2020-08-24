#include <list>

#include <Lz/Generate.hpp>

#include <catch.hpp>


TEST_CASE("Generate changing and creating elements", "[Generate][Basic functionality]") {
    constexpr size_t amount = 4;
    size_t counter = 0;
    auto generator = lz::generate([&counter]() {
        return counter++;
    }, amount);

    SECTION("Should be 0, 1, 2, 3") {
        size_t expected = 0;
        for (size_t i : generator) {
            CHECK(i == expected++);
        }
    }
}


TEST_CASE("Generate binary operations", "[Generate][Binary ops]") {
    constexpr size_t amount = 4;
    size_t counter = 0;
    auto generator = lz::generate([&counter]() {
        return counter++;
    }, amount);
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
        const size_t toAdd = 2;
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
    constexpr size_t amount = 4;
    size_t counter = 0;
    auto generator = lz::generate([&counter]() {
        return counter++;
    }, amount);

    SECTION("To array") {
        std::array<size_t, amount> array = generator.toArray<amount>();
        std::array<size_t, amount> expected = {0, 1, 2, 3};

        CHECK(array == expected);
    }

    SECTION("To vector") {
        std::vector<size_t> vector = generator.toVector();
        std::vector<size_t> expected = {0, 1, 2, 3};

        CHECK(vector == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<size_t> vector = generator.to<std::list>();
        std::list<size_t> expected = {0, 1, 2, 3};

        CHECK(vector == expected);
    }

    SECTION("To map") {
        std::map<size_t, size_t> map = generator.toMap([](const size_t elm) {
            return elm * 10;
        });

        std::map<size_t, size_t> expected = {
            {0, 0},
            {10, 1},
            {20, 2},
            {30, 3}
        };

        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<size_t, size_t> map = generator.toUnorderedMap([](const size_t elm) {
            return elm * 10;
        });

        std::unordered_map<size_t, size_t> expected = {
            {0, 0},
            {10, 1},
            {20, 2},
            {30, 3}
        };

        CHECK(map == expected);
    }
}