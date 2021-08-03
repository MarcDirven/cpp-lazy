#include <Lz/Filter.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Filter filters and is by reference", "[Filter][Basic functionality]") {
    constexpr size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SECTION("Should filter out element") {
        std::function<bool(int)> f = [](int element) {
            return element != 3;
        };
        auto filter = lz::filter(array, std::move(f));
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
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };
    std::function<bool(int)> f = [](int i) {
        return i != 3;
    };
    auto filter = lz::filter(array, std::move(f));
    auto it = filter.begin();

    SECTION("Operator++") {
        CHECK(*it == array[0]);
        ++it;
        CHECK(*it == array[1]);
    }

    SECTION("Operator--") {
        std::array<int, 5> ar = {2, 2, 3, 4, 2};
        auto filter2 = lz::filter(ar, [](int i) { return i != 2; });
        auto itEnd = filter2.end();
        --itEnd;
        CHECK(*itEnd == 4);
        --itEnd;
        CHECK(*itEnd == 3);
        CHECK(itEnd == filter2.begin());
        CHECK(std::distance(std::make_reverse_iterator(filter2.end()), std::make_reverse_iterator(filter2.begin())) == 2);

        std::array<int, 1> arr = {1};
        auto filter3 = lz::filter(arr, [](int i) { return i != 1; });
        CHECK(std::distance(std::make_reverse_iterator(filter3.end()), std::make_reverse_iterator(filter3.begin())) == 0);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != filter.end());
        it = filter.end();
        CHECK(it == filter.end());
    }
}

#ifdef LZ_HAS_EXECUTION
#define LZ_PAR std::execution::par
#else
#define LZ_PAR
#endif

TEST_CASE("Filter to container", "[Filter][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SECTION("To array") {
        constexpr std::size_t filterSize = 2;
        auto filtered = lz::filter(array, [](int i) { return i != 3; }).toArray<filterSize>();

        CHECK(filtered[0] == array[0]);
        CHECK(filtered[1] == array[1]);
    }

    SECTION("To vector") {
        auto filteredVec = lz::filter(array, [](int i) { return i != 3; }).toVector(LZ_PAR);

        CHECK(filteredVec.size() == 2);
        CHECK(filteredVec[0] == array[0]);
        CHECK(filteredVec[1] == array[1]);
    }

    SECTION("To other container using to<>()") {
        auto filteredList = lz::filter(array, [](int i) { return i != 3; }).to<std::list>();

        CHECK(filteredList.size() == 2);
        auto counter = array.begin();

        for (int element : filteredList) {
            CHECK(element == *counter);
            ++counter;
        }
    }

    SECTION("To map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });

        std::map<int, int> actual = filtered.toMap([](const int i) { return i; });

        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });

        std::unordered_map<int, int> actual = filtered.toUnorderedMap([](const int i) { return i; });

        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };

        CHECK(expected == actual);
    }
}
