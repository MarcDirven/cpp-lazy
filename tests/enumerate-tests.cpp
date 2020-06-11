#include <list>

#include <catch.hpp>
#include <Lz/Enumerate.hpp>


TEST_CASE("Enumerate changing and creating elements", "[Enumerate][Creating elements]") {
    constexpr size_t size = 2;
    std::array<int, size> array = {1, 2};

    SECTION("Enumerate should create pair with {idx, elm}") {
        auto enumerate = lz::enumerate(array);
        auto element = *enumerate.begin();

        REQUIRE(element.first == 0);  // Idx
        REQUIRE(element.second == 1);  // Element
    }

    SECTION("Enumerate should create pair with {idx, elm} with offset") {
        auto enumerate = lz::enumerate(array, 2);
        auto element = *enumerate.begin();

        REQUIRE(element.first == 2);  // Idx
        REQUIRE(element.second == 1);  // Element
    }

    SECTION("Enumerate should be by reference") {
        auto enumerate = lz::enumerate(array);
        auto element = *enumerate.begin();
        element.second = 500;

        REQUIRE(array[0] == 500);
    }
}


TEST_CASE("Enumerate binary operations", "[Enumerate][Binary ops]") {
    constexpr size_t size = 3;
    std::array<int, size> array = {1, 2, 3};
    auto enumerate = lz::enumerate(array);
    auto begin = enumerate.begin();
    ++begin; // Increment by one

    SECTION("Operator++") {
        REQUIRE((*begin).first == 1); // Index
        REQUIRE((*begin).second == 2); // element
    }

    SECTION("Operator--") {
        --begin;
        // Decrement by one, back at begin()
        REQUIRE((*begin).first == 0); // Index
        REQUIRE((*begin).second == 1); // element
    }

    SECTION("Operator+, tests += as well") {
        REQUIRE((*(begin + 1)).first == 2); // Index
        REQUIRE((*(begin + 1)).second == 3); // element
    }

    SECTION("Operator-, tests -= as well") {
        REQUIRE((*(begin - 1)).first == 0); // Index
        REQUIRE((*(begin - 1)).second == 1); // element
    }
}

TEST_CASE("Enumerate to containers", "[Enumerate][To container]") {
    constexpr size_t size = 3;
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

    SECTION("To other container") {
        auto actualList = lz::enumerate(vec).to<std::list>();
        auto expectedPair = std::make_pair(0, 1);

        for (const auto& actualPair : actualList) {
            CHECK(actualPair == expectedPair);
            expectedPair = std::make_pair(++expectedPair.first, ++expectedPair.second);
        }
    }
}