#include <list>

#include <catch2/catch.hpp>
#include <Lz/Take.hpp>


TEST_CASE("Take changing and creating elements", "[Take][Basic functionality]") {
    std::array<int, 10> array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("Should take 3 elements, 0 offset") {
        auto taken = lz::take(array, 3);
        auto it = taken.begin();

        CHECK(*it == 1);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 3);
        ++it;
        CHECK(it == taken.end());
    }

    SECTION("Should be by ref") {
        auto taken = lz::take(array, 3);
        auto actual = taken.begin();
        *actual = 0;
        CHECK(array[0] == 0);
    }

    SECTION("Should slice 2 elements, 2 offset") {
        auto taken = lz::slice(array, 2, 4);
        auto it = taken.begin();

        CHECK(*it == 3);
        ++it;
        CHECK(*it == 4);
        ++it;
        CHECK(it == taken.end());
    }

    SECTION("Should take while range") {
        auto taken = lz::takeWhile(array, [](int i) { return i != 5; });

        int expected = 1;
        for (int actual : taken) {
            CHECK(expected == actual);
            ++expected;
        }
    }

    SECTION("Should drop n elements") {
        auto dropped = lz::drop(array, 2);
        CHECK(dropped.toVector() == std::vector<int>{3, 4, 5, 6, 7, 8, 9, 10});
    }
}


TEST_CASE("Take binary operations", "[Take][Binary ops]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = {
        1, 2, 3
    };
    constexpr std::size_t takeAmount = 2;
    auto taken = lz::take(array, takeAmount);
    auto it = taken.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 2);
    }

    SECTION("Operator--") {
        ++it;
        --it;
        CHECK(*it == 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != taken.end());
        it = taken.end();
        CHECK(it == taken.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 1) == 2);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++it;
        CHECK(*(it - 1) == 1);
    }

    SECTION("Operator-(Iterator)") {
        CHECK((taken.end() - taken.begin()) == 2);
    }

    SECTION("Operator[]()") {
        CHECK(taken.begin()[0] == 1);
    }

    SECTION("Operator<, '<, <=, >, >='") {
        auto b = taken.begin();
        auto end = taken.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + size - 1 >= end - 1);
    }
}


TEST_CASE("Take to containers", "[Take][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = {
        1, 2, 3
    };


    SECTION("To array") {
        constexpr std::size_t newSize = 2;
        auto actual = lz::take(array, newSize).toArray<newSize>();
        std::array<int, 2> expected = {1, 2};

        CHECK(actual == expected);
    }

    SECTION("To vector") {
        auto actual = lz::take(array, 2).toVector();
        std::vector<int> expected = {1, 2};

        CHECK(actual == expected);
    }

    SECTION("To other container using to<>()") {
        auto actual = lz::take(array, 2).to<std::list>();
        std::list<int> expected = {1, 2};

        CHECK(expected == actual);
    }

    SECTION("To map") {
        constexpr std::size_t newSize = 2;
        std::map<int, int> actual = lz::take(array, newSize).toMap([](const int i) { return i; });
        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2)
        };
        CHECK(actual == expected);
    }

    SECTION("To map") {
        constexpr std::size_t newSize = 2;
        std::unordered_map<int, int> actual = lz::take(array, newSize).toUnorderedMap([](const int i) { return i; });
        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2)
        };
        CHECK(actual == expected);
    }
}


TEST_CASE("DropWhile changing and creating elements", "[DropWhile][Basic functionality]") {
    std::array<int, 4> arr = { 1, 1, 2, 1 };
    const auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });
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
    std::vector<int> vec = { 1, 1, 2, 3, 4, 1 };
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
    constexpr std::size_t size = 4;
    std::array<int, size> arr = { 1, 1, 2, 1 };
    auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });

    SECTION("To array") {
        constexpr std::size_t newSize = 2;
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