#include <Lz/Choose.hpp>

#include <catch.hpp>

#include <cctype>
#include <iostream>
#include <list>
#include <set>


TEST_CASE("Choose changing and creating elements", "[Choose][Basic functionality]") {
    std::string s = "1q9";
    auto chooser = lz::choose(s, [](const char s) {
        return std::make_pair(static_cast<bool>(std::isdigit(s)), static_cast<int>(s - '0'));
    });
    auto begin = chooser.begin();

    SECTION("Should convert to second pair argument") {
        CHECK(*begin == 1);
    }

    SECTION("Should not be a letter and should be last element") {
        ++begin;
        CHECK(*begin != static_cast<int>(s[1] - '0'));
        CHECK(*begin == 9);
    }
}

TEST_CASE("Choose binary operations", "[Choose][Binary ops]") {
    std::string s = "12qw45";
    auto chooser = lz::choose(s, [](const char s) {
        return std::make_pair(static_cast<bool>(std::isdigit(s)), static_cast<int>(s - '0'));
    });
    auto iterator = chooser.begin();

    CHECK(*iterator == 1);

    SECTION("Operator++") {
        ++iterator;
        CHECK(*iterator == 2);
        ++iterator;
        CHECK(*iterator == 4);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(iterator != chooser.end());
        iterator = chooser.end();
        CHECK(iterator == chooser.end());
    }
}

TEST_CASE("Choose to container", "[Choose][To container]") {
    constexpr size_t nInts = 4;
    std::string s = "12qw45s";
    auto chooser = lz::choose(s, [](const char s) {
        return std::make_pair(static_cast<bool>(std::isdigit(s)), static_cast<int>(s - '0'));
    });

    SECTION("To array") {
        CHECK(chooser.toArray<nInts>() == std::array<int, nInts>{1, 2, 4, 5});
    }

    SECTION("To vector") {
        CHECK(chooser.toVector() == std::vector<int>{1, 2, 4, 5});
    }

    SECTION("To other container using to<>()") {
        CHECK(chooser.to<std::list>() == std::list<int>{1, 2, 4, 5});
    }

    SECTION("To map") {
        int x = 0;
        std::map<int, int> actual = chooser.toMap([&x](const char s) {
           return x++;
        });

        std::map<int, int> expected = {
            std::make_pair(0, 1),
            std::make_pair(1, 2),
            std::make_pair(2, 4),
            std::make_pair(3, 5)
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        int x = 0;
        std::unordered_map<int, int> actual = chooser.toUnorderedMap([&x] (const char s) {
            return x++;
        });

        std::unordered_map<int, int> expected = {
            std::make_pair(0, 1),
            std::make_pair(1, 2),
            std::make_pair(2, 4),
            std::make_pair(3, 5)
        };

        CHECK(expected == actual);
    }
}