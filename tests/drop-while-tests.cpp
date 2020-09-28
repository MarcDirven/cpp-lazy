#include <catch.hpp>

#include <Lz/DropWhile.hpp>
#include <list>


TEST_CASE("DropWhile changing and creating elements", "[DropWhile][Basic functionality]") {
    std::array<int, 4> arr = {1, 1, 2, 1};
    auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });
    auto begin = dropped.begin();

    SECTION("Should skip elements") {
        CHECK(*begin == 2);
    }

    SECTION("Should be by reference") {
        *begin = 0;
        CHECK(arr[2] == 0);
    }

    SECTION("Should not drop last element") {
        ++begin;
        CHECK(*begin == 1);
    }
}


TEST_CASE("DropWhile binary operations", "[DropWhile][Binary ops]") {
    std::vector<int> vec = {1, 1, 2, 3, 4, 1};
    auto dropped = lz::dropWhile(vec, [](const int i) { return i == 1; });
    auto begin = dropped.begin();

    SECTION("Operator++") {
        CHECK(*begin == 2);
        ++begin;
        CHECK(*begin == 3);
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(*begin == 2);
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != dropped.end());
        begin = dropped.end();
        CHECK(begin == dropped.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(begin + 2) == 4);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        begin += 2;
        CHECK(*(begin - 2) == 2);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(dropped.end() - begin == 4);
        CHECK(std::distance(begin, dropped.end()) == 4);
    }

    SECTION("Operator[]()") {
        CHECK(begin[0] == 2);
        CHECK(begin[3] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        const auto end = dropped.end();

        CHECK(begin < end);
        CHECK(begin + 3 > end - 3);
        CHECK(begin + 4 <= end);
        CHECK(begin + 3 >= end - 2);
    }
}

TEST_CASE("DropWhile to containers", "[DropWhile][To container]") {
    constexpr size_t size = 4;
    std::array<int, size> arr = {1, 1, 2, 1};
    auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });

    SECTION("To array") {
        constexpr size_t newSize = 2;
        CHECK(dropped.toArray<newSize>() == std::array<int, newSize>{2, 1});
    }

    SECTION("To vector") {
        CHECK(dropped.toVector() == std::vector<int>{2, 1});
    }

    SECTION("To other container using to<>()") {
        CHECK(dropped.to<std::list>() == std::list<int>{2, 1});
    }

    SECTION("To map") {
        std::map<int, int> map = dropped.toMap([](const int i) { return i; });
        std::map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(1, 1)
        };
        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> map = dropped.toUnorderedMap([](const int i) { return i; });
        std::unordered_map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(1, 1)
        };
        CHECK(map == expected);
    }
}