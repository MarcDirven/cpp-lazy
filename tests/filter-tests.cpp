#include <Lz/CString.hpp>
#include <Lz/Filter.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Filter with sentinels") {
    const char* str = "Hello, World!";
    auto cStr = lz::c_string(str);
    auto filter = lz::filter(cStr, [](char c) { return c != 'o'; });
    static_assert(!std::is_same<decltype(filter.begin()), decltype(filter.end())>::value, "Must be sentinel");
    std::vector<char> expected = { 'H', 'e', 'l', 'l', ',', ' ', 'W', 'r', 'l', 'd', '!' };
    CHECK(filter.to_vector() == expected);
}

TEST_CASE("Filter filters and is by reference", "[Filter][Basic functionality]") {
    constexpr size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SECTION("Should filter out element") {
        std::function<bool(int)> f = [](int element) {
            return element != 3;
        };
        auto filter = lz::filter(array, std::move(f));
        static_assert(std::is_same<decltype(filter.begin()), decltype(filter.end())>::value, "Must not be sentinel");

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

    SECTION("Operator== & operator!=") {
        CHECK(it != filter.end());
        it = filter.end();
        CHECK(it == filter.end());
    }

    SECTION("Operator--") {
        auto revIter = filter.end();
        CHECK(revIter != filter.begin());
        --revIter;
        CHECK(*revIter == array[1]);
        CHECK(revIter != filter.begin());
        ++revIter;
        CHECK(revIter == filter.end());
        --revIter;
        CHECK(*revIter == array[1]);
        --revIter;
        CHECK(*revIter == array[0]);
        CHECK(revIter == filter.begin());
    }
}


TEST_CASE("Filter to container", "[Filter][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SECTION("To array") {
        constexpr std::size_t filterSize = 2;
        auto filtered = lz::filter(array, [](int i) { return i != 3; }).to<std::array<int, filterSize>>();

        CHECK(filtered[0] == array[0]);
        CHECK(filtered[1] == array[1]);
    }

    SECTION("To vector") {
        auto filteredVec = lz::filter(array, [](int i) { return i != 3; }).to_vector();

        CHECK(filteredVec.size() == 2);
        CHECK(filteredVec[0] == array[0]);
        CHECK(filteredVec[1] == array[1]);
    }

    SECTION("To other container using to<>()") {
        auto filteredList = lz::filter(array, [](int i) { return i != 3; }).to<std::list<int>>();

        CHECK(filteredList.size() == 2);
        auto counter = array.begin();

        for (int element : filteredList) {
            CHECK(element == *counter);
            ++counter;
        }
    }

    SECTION("To map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });
        std::map<int, int> actual = filtered.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };
        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });
        std::unordered_map<int, int> actual = filtered.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };
        CHECK(expected == actual);
    }

    SECTION("To reverse container") {
        auto filtered = lz::filter(array, [](int i) { return i != 3; });

        auto itRev = std::make_reverse_iterator(filtered.end());
        auto endRev = std::make_reverse_iterator(filtered.begin());
        std::vector<int> reversed(itRev, endRev);
        std::vector<int> expected = { 2, 1 };
        CHECK(reversed == expected);
    }
}
