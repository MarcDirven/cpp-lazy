#include <Lz/Repeat.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Repeat changing and creating elements", "[Repeat][Basic functionality]") {
    int toRepeat = 20;
    auto repeater = lz::repeat(toRepeat, 5);

    SECTION("Should be 5 times 20") {
        std::size_t counter = 0;
        for (int i : repeater) {
            CHECK(i == 20);
            ++counter;
        }
        CHECK(counter == 5);
    }

    SECTION("Should not be by reference") {
        auto start = repeater.begin();
        CHECK(&(*start) != &toRepeat);
    }
}

TEST_CASE("Repeat binary operations", "[Repeat][Binary ops]") {
    const int amount = 5;
    auto repeater = lz::repeat(20, amount);
    auto begin = repeater.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(lz::distance(begin, repeater.end()) == amount - 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != repeater.end());
        while (begin != repeater.end()) {
            ++begin;
        }
        CHECK(begin == repeater.end());
    }
}

TEST_CASE("Repeat to containers", "[Repeat][To container]") {
    constexpr auto times = 5;
    const int toRepeat = 20;
    auto repeater = lz::repeat(toRepeat, times);

    SECTION("To array") {
        std::array<int, times> array = repeater.to<std::array<int, times>>();
        for (int i : array) {
            CHECK(i == toRepeat);
        }
    }

    SECTION("To vector") {
        std::vector<int> vec = repeater.toVector();
        for (int i : vec) {
            CHECK(i == toRepeat);
        }
        CHECK(vec.size() == times);
    }

    SECTION("To other container using to<>()") {
        std::list<int> lst = repeater.to<std::list<int>>();
        for (int i : lst) {
            CHECK(i == toRepeat);
        }
        CHECK(lst.size() == times);
    }

    SECTION("To map") {
        std::map<int, int> actual = repeater.toMap([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = repeater.toUnorderedMap([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        CHECK(actual == expected);
    }
}
