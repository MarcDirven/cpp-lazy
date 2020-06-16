#include <list>

#include <catch.hpp>

#include <Lz/Filter.hpp>


TEST_CASE("Filter filters and is by reference", "[Filter][Basic functionality]") {
    constexpr size_t size = 3;
    std::array<int, size> array{1, 2, 3};

    SECTION("Should filter out element") {
        auto filter = lz::filter(array, [](int element) { return element != 3; });
        auto it = filter.begin();

        int expected = array[0];
        CHECK(*it == expected);

        ++it;
        expected = array[1];
        CHECK(*it == expected);

        ++it;
        CHECK(it == filter.end());
    }

    SECTION("Should be by reference") {
        auto filter = lz::filter(array, [](int element) { return element != 3; });
        auto it = filter.begin();

        *it = 50;
        CHECK(array[0] == 50);
    }
}


TEST_CASE("Filter binary operations", "[Filter][Binary ops]") {
    constexpr size_t size = 3;
    std::array<int, size> array{1, 2, 3};

    SECTION("Operator++") {
        auto filter = lz::filter(array, [](int i) { return i != 1 && i != 2; });
        auto it = filter.begin();

        CHECK(*it == array[2]);
    }
}


TEST_CASE("Filter to container", "[Filter][To container]") {
    constexpr size_t size = 3;
    std::array<int, size> array{1, 2, 3};

    SECTION("To array") {
        constexpr size_t filterSize = 2;
        auto filtered = lz::filter(array, [](int i) {
            return i != 3;
        }).toArray<filterSize>();

        CHECK(filtered[0] == array[0]);
        CHECK(filtered[1] == array[1]);
    }

    SECTION("To vector") {
        auto filteredVec = lz::filter(array, [](int i) {
            return i != 3;
        }).toVector();

        CHECK(filteredVec.size() == 2);
        CHECK(filteredVec[0] == array[0]);
        CHECK(filteredVec[1] == array[1]);
    }

    SECTION("To other container using to<>()") {
        auto filteredList = lz::filter(array, [](int i) {
            return i != 3;
        }).to<std::list>();

        CHECK(filteredList.size() == 2);
        auto counter = array.begin();

        for (int element : filteredList) {
            CHECK(element == *counter);
            ++counter;
        }
    }
}
