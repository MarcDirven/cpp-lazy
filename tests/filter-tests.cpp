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
    auto filter = lz::filter(array, [](int i) { return i != 3; });
    auto it = filter.begin();

    SECTION("Operator++") {
        CHECK(*it == array[0]);
        ++it;
        CHECK(*it == array[1]);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != filter.end());
        it = filter.end();
        CHECK(it == filter.end());
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

    SECTION("To map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });

        std::map<int, int> actual = filtered.toMap([](const int i) {
            return i;
        });

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2)
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });

        std::unordered_map<int, int> actual = filtered.toUnorderedMap([](const int i) {
            return i;
        });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2)
        };

        CHECK(expected == actual);
    }
}
