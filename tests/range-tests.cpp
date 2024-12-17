#include <Lz/range.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Range changing and creating elements", "[Range][Basic functionality]") {
    SECTION("Looping upwards") {
        int expected_counter = 0;

        for (int i : lz::range(5)) {
            CHECK(expected_counter == i);
            expected_counter++;
        }
    }

    SECTION("Looping backwards") {
        int expected_counter = 5;

        for (int i : lz::range(5, 0, -1)) {
            CHECK(expected_counter == i);
            expected_counter--;
        }
    }

    SECTION("Looping upwards with step") {
        int expected_counter = 0;

        for (int i : lz::range(0, 5, 2)) {
            CHECK(expected_counter == i);
            expected_counter += 2;
        }
    }

    SECTION("Looping backwards with step") {
        int expected_counter = 5;

        for (int i : lz::range(5, 0, -2)) {
            CHECK(expected_counter == i);
            expected_counter -= 2;
        }
    }
}

TEST_CASE("Range binary operations", "[Range][Binary ops]") {
    constexpr int size = 10;
    auto range = lz::range(size);
    auto f_range = lz::range(0., 10.5, 0.5);
    auto it = range.begin();
    auto f_it = f_range.begin();

    CHECK(*it == 0);
    CHECK(*f_it == 0.);

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 1);
        ++f_it;
        CHECK(*f_it == 0.5);
    }

    SECTION("Operator--") {
        ++it, --it;
        CHECK(*it == 0);
        ++f_it, --f_it;
        CHECK(*f_it == 0);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != range.end());
        it = range.end();
        CHECK(it == range.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 2) == 2);
        CHECK(*(it + 4) == 4);
        CHECK(*(f_it + 2) == 1.);
        CHECK(*(f_it + 3) == 1.5);
        CHECK(*(f_it + 5) == 2.5);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        CHECK(*((it + 2) - 1) == 1);
        CHECK(*((it + 4) - 2) == 2);
        CHECK(*((f_it + 2) - 1) == .5);
        CHECK(*((f_it + 3) - 2) == .5);
        CHECK(*((f_it + 5) - 2) == 1.5);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(range.end() - it == 10);
        CHECK(f_range.end() - f_it == 21);

        CHECK(range.end() - (it + 1) == 9);
        CHECK(f_range.end() - (f_it + 1) == 20);
    }

    SECTION("Operator[]()") {
        CHECK(f_it[1] == .5);
        CHECK(it[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = range.begin();
        auto end = range.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + size - 1 >= end - 1);

        auto f_b = range.begin();
        auto f_end = range.end();

        CHECK(f_b < end);
        CHECK(f_b + distance - 1 > f_end - distance);
        CHECK(f_b + distance - 1 <= f_end);
        CHECK(f_b + 20 >= f_end - 1);
    }
}

TEST_CASE("Range to containers", "[Range][To container]") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size)> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to<std::array<int, static_cast<std::size_t>(size)>>();

        CHECK(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to_vector();

        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to<std::list<int>>();

        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = range.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = range.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        CHECK(expected == actual);
    }
}
