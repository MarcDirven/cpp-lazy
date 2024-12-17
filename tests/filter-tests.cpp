#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Filter with sentinels") {
    const char* str = "Hello, World!";
    auto c_str = lz::c_string(str);
    auto filter = lz::filter(c_str, [](char c) { return c != 'o'; });
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
        auto rev_it = filter.end();
        CHECK(rev_it != filter.begin());
        --rev_it;
        CHECK(*rev_it == array[1]);
        CHECK(rev_it != filter.begin());
        ++rev_it;
        CHECK(rev_it == filter.end());
        --rev_it;
        CHECK(*rev_it == array[1]);
        --rev_it;
        CHECK(*rev_it == array[0]);
        CHECK(rev_it == filter.begin());
    }
}


TEST_CASE("Filter to container", "[Filter][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SECTION("To array") {
        constexpr std::size_t filter_size = 2;
        auto filtered = lz::filter(array, [](int i) { return i != 3; }).to<std::array<int, filter_size>>();

        CHECK(filtered[0] == array[0]);
        CHECK(filtered[1] == array[1]);
    }

    SECTION("To vector") {
        auto filtered_vec = lz::filter(array, [](int i) { return i != 3; }).to_vector();

        CHECK(filtered_vec.size() == 2);
        CHECK(filtered_vec[0] == array[0]);
        CHECK(filtered_vec[1] == array[1]);
    }

    SECTION("To other container using to<>()") {
        auto filtered_list = lz::filter(array, [](int i) { return i != 3; }).to<std::list<int>>();

        CHECK(filtered_list.size() == 2);
        auto counter = array.begin();

        for (int element : filtered_list) {
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

        auto it_rev = std::make_reverse_iterator(filtered.end());
        auto end_rev = std::make_reverse_iterator(filtered.begin());
        std::vector<int> reversed(it_rev, end_rev);
        std::vector<int> expected = { 2, 1 };
        CHECK(reversed == expected);
    }
}
