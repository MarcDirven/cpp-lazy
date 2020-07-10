#include <list>
#include <array>

#include <catch.hpp>

#include <Lz/Repeat.hpp>


TEST_CASE("Repeat changing and creating elements", "[Repeat][Basic functionality]") {
    int toRepeat = 20;
    auto repeater = lz::repeat(toRepeat, 5);

    SECTION("Should be 5 times 20") {
        size_t counter = 0;
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
        CHECK(std::distance(begin, repeater.end()) == amount - 1);
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(std::distance(begin, repeater.end()) == amount);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != repeater.end());
        begin = repeater.end();
        CHECK(begin == repeater.end());
    }

    SECTION("Operator+(int), tests += as well") {
        const int toAdd = 2;
        begin += toAdd;
        CHECK(std::distance(begin, repeater.end()) == amount - toAdd);
    }

    SECTION("Operator-(int), tests -= as well") {
        ++begin;
        begin -= 1;
        CHECK(std::distance(begin, repeater.end()) == amount);
    }

    SECTION("Operator-(Iterator)") {
        CHECK((repeater.end() - begin) == 5);
    }

    SECTION("Operator[]()") {
        CHECK(begin[0] == 20);
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(begin < repeater.end());
        CHECK(begin + amount + 1 > repeater.end());
        CHECK(begin + amount <= repeater.end());
        CHECK(begin + amount >= repeater.end());
    }
}

TEST_CASE("Repeat to containers", "[Repeat][To container]") {
    constexpr auto times = 5;
    const int toRepeat = 20;
    auto repeater = lz::repeat(toRepeat, times);

    SECTION("To array") {
        std::array<int, times> array = repeater.toArray<times>();
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
        std::list<int> lst = repeater.to<std::list>();
        for (int i : lst) {
            CHECK(i == toRepeat);
        }
        CHECK(lst.size() == times);
    }
}
