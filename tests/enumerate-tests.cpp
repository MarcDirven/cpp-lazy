#include <list>

#include <catch.hpp>
#include <Lz/Enumerate.hpp>


TEST_CASE("Enumerate changing and creating elements", "[Enumerate][Basic functionality]") {
    constexpr std::size_t size = 2;
    std::array<int, size> array = {1, 2};


    SECTION("Enumerate should create pair with {idx, elm}") {
        auto enumerate = lz::enumerate(array);
        auto element = *enumerate.begin();

        CHECK(element.first == 0);  // Idx
        CHECK(element.second == 1);  // Element
    }

    SECTION("Enumerate should create pair with {idx, elm} with offset") {
        auto enumerate = lz::enumerate(array, 2);
        auto element = *enumerate.begin();

        CHECK(element.first == 2);  // Idx
        CHECK(element.second == 1);  // Element
    }

    SECTION("Enumerate should be by reference") {
        auto enumerate = lz::enumerate(array);
        auto element = *enumerate.begin();
        element.second = 500;

        CHECK(array[0] == 500);
    }
}


TEST_CASE("Enumerate binary operations", "[Enumerate][Binary ops]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = {1, 2, 3};
    auto enumerate = lz::enumerate(array);
    auto begin = enumerate.begin();
    ++begin; // Increment by one

    SECTION("Operator++") {
        CHECK(begin->first == 1); // Index
        CHECK(begin->second == 2); // element
    }

    SECTION("Operator--") {
        --begin;
        // Decrement by one, back at begin()
        CHECK(begin->first == 0); // Index
        CHECK(begin->second == 1); // element
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != enumerate.end());
        begin = enumerate.end();
        CHECK(begin == enumerate.end());
    }

    SECTION("Operator+(int), tests += as well") {
        CHECK((begin + 1)->first == 2); // Index
        CHECK((begin + 1)->second == 3); // element
    }

    SECTION("Operator-(int), tests -= as well") {
        CHECK((begin - 1)->first == 0); // Index
        CHECK((begin - 1)->second == 1); // element
    }

    SECTION("Operator-(Iterator)") {
        CHECK(enumerate.end() - begin == 2);
    }

    SECTION("Operator[]()") {
        CHECK(enumerate.begin()[1].first == 1);
        CHECK(enumerate.begin()[1].second == 2);
    }

    SECTION("Operator<, '<, <=, >, >='") {
        const auto b = enumerate.begin();
        const auto end = enumerate.end();
        const auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + size - 1 >= end - 1);
    }
}

TEST_CASE("Enumerate to containers", "[Enumerate][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = {1, 2, 3};
    std::vector<int> vec = {1, 2, 3};

    SECTION("To array") {
        std::array<std::pair<int, int>, size> actualArray = lz::enumerate(array).toArray<size>();
        auto expectedPair = std::make_pair(0, 1);

        for (auto actualPair : actualArray) {
            CHECK(actualPair == expectedPair);
            expectedPair = std::make_pair(++expectedPair.first, ++expectedPair.second);
        }
    }

    SECTION("To vector") {
        std::vector<std::pair<int, int>> actualArray = lz::enumerate(vec).toVector();
        auto expectedPair = std::make_pair(0, 1);

        for (const auto& actualPair : actualArray) {
            CHECK(actualPair == expectedPair);
            expectedPair = std::make_pair(++expectedPair.first, ++expectedPair.second);
        }
    }

    SECTION("To other container using to<>()") {
        std::list<std::pair<int, int>> actualList = lz::enumerate(vec).to<std::list>();
        auto expectedPair = std::make_pair(0, 1);

        for (const auto& actualPair : actualList) {
            CHECK(actualPair == expectedPair);
            expectedPair = std::make_pair(++expectedPair.first, ++expectedPair.second);
        }
    }

    SECTION("To map") {
        auto enumerator = lz::enumerate(array);
        std::map<int, std::pair<int, int>> actual = enumerator.toMap([](const std::pair<int, int> pair) {
            return pair.second;
        });

        std::map<int, std::pair<int, int>> expected = {
            std::make_pair(1, std::make_pair(0, 1)),
            std::make_pair(2, std::make_pair(1, 2)),
            std::make_pair(3, std::make_pair(2, 3)),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        auto enumerator = lz::enumerate(array);
        std::unordered_map<int, std::pair<int, int>> actual = enumerator.toUnorderedMap(
            [](const std::pair<int, int> pair) {
                return pair.second;
            });

        std::unordered_map<int, std::pair<int, int>> expected = {
            std::make_pair(1, std::make_pair(0, 1)),
            std::make_pair(2, std::make_pair(1, 2)),
            std::make_pair(3, std::make_pair(2, 3)),
        };

        CHECK(actual == expected);
    }
}
