#include <catch2/catch.hpp>

#include <Lz/Concatenate.hpp>
#include <list>


TEST_CASE("Concat changing and creating elements", "[Concat][Basic functionality]") {
    std::string a = "hello ";
    std::string b = "world";

    auto concat = lz::concat(a, b);

    SECTION("Should be by reference") {
        *concat.begin() = 'd';
        CHECK(a[0] == 'd');
    }

    SECTION("Should concat") {
        constexpr const char* expected = "hello world";
        CHECK(concat.to<std::basic_string>() == expected);
    }

    SECTION("Length should be correct") {
        auto dist = static_cast<std::size_t>(std::distance(concat.begin(), concat.end()));
        CHECK(dist == a.size() + b.size());
    }
}

TEST_CASE("Concat binary operations", "[Concat][Binary ops]") {
    std::string a = "hello ", b = "world";
    auto concat = lz::concat(a, b);
    auto begin = concat.begin();

    CHECK(*begin == 'h');

    SECTION("Operator++") {
        ++begin;
        CHECK(*begin == 'e');
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(*begin == 'h');
        ++begin, ++begin, ++begin, ++begin, ++begin, ++begin;

        --begin;
        CHECK(*begin == ' ');
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != concat.end());
        begin = concat.end();
        CHECK(begin == concat.end());
    }

    SECTION("Operator+(int), tests += as well") {
        CHECK(*(begin + a.size()) == 'w');
    }

    SECTION("Operator-(int), tests -= as well") {
        begin += a.size();
        CHECK(*begin == 'w');
    }

    SECTION("Operator-(Iterator)") {
        CHECK(static_cast<std::size_t>(concat.end() - begin) == a.size() + b.size());
        CHECK(static_cast<std::size_t>(std::distance(concat.begin(), concat.end())) == a.size() + b.size());
    }

    SECTION("Operator[]()") {
        CHECK(begin[a.size()] == 'w');
    }

    SECTION("Operator<, '<, <=, >, >='") {
        auto end = concat.end();
        const auto distance = std::distance(begin, end) - 1;

        CHECK(begin < end);
        begin += distance;
        CHECK(begin <= end);
        end -= distance;

        CHECK(begin > end);
        CHECK(begin >= end);
    }
}


TEST_CASE("Concatenate to containers", "[Concatenate][To container]") {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {4, 5, 6};
    auto concat = lz::concat(v1, v2);

    SECTION("To array") {
        constexpr std::size_t size = 3 + 3;
        CHECK(concat.toArray<size>() == std::array<int, size>{1, 2, 3, 4, 5, 6});
    }

    SECTION("To vector") {
        CHECK(concat.toVector() == std::vector<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("To other container using to<>()") {
        CHECK(concat.to<std::list>() == std::list<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("To map") {
        std::map<int, int> map = concat.toMap([](const int i) { return i; });
        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
            std::make_pair(4, 4),
            std::make_pair(5, 5),
            std::make_pair(6, 6)
        };
        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> map = concat.toUnorderedMap([](const int i) { return i; });
        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
            std::make_pair(4, 4),
            std::make_pair(5, 5),
            std::make_pair(6, 6)
        };
        CHECK(map == expected);
    }
}