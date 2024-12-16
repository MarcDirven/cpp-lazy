#include <Lz/c_string.hpp>
#include <Lz/concatenate.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Concatenate with sentinels") {
    const char* str = "hello, world!";
    auto cstr = lz::c_string(str);
    std::vector<char> vec = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    auto concat = lz::concat(cstr, vec);
    static_assert(std::is_same<lz::DefaultSentinel, decltype(concat.end())>::value, "Sentinel type should be DefaultSentinel");
    std::vector<char> expected = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!',
                                   'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    CHECK(lz::equal(concat, expected));
}

TEST_CASE("Concat changing and creating elements", "[Concat][Basic functionality]") {
    std::string a = "hello ";
    std::string b = "world";

    auto concat = lz::concat(a, b);
    static_assert(std::is_same<decltype(concat.begin()), decltype(concat.end())>::value, "Should not be sentinel");

    SECTION("Should be by reference") {
        *concat.begin() = 'd';
        CHECK(a[0] == 'd');
    }

    SECTION("Should concat") {
        constexpr const char* expected = "hello world";
        CHECK(concat.to<std::string>() == expected);
    }

    SECTION("Length should be correct") {
        auto dist = static_cast<std::size_t>(std::distance(concat.begin(), concat.end()));
        CHECK(dist == a.size() + b.size());
    }

    SECTION("Should (be) sort(ed)") {
        std::array<int, 10> arr1 = { 5, 2, 67, 1235, 654, 23, 6, 1324, 6, 34 };
        std::array<int, 10> arr2 = { 756, 23, 465, 1, 6, 4, 1234, 65, 567, 2 };
        auto concatted = lz::concat(arr1, arr2);
        std::sort(concatted.begin(), concatted.end());
        CHECK(std::is_sorted(concatted.begin(), concatted.end()));
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
        --begin;
        CHECK(*begin == 'o');
    }

    SECTION("Operator== & operator!=") {
        CHECK(begin != concat.end());
        begin = concat.end();
        CHECK(begin == concat.end());
    }

    SECTION("Operator+(int), tests += as well") {
        CHECK(*(begin + static_cast<std::ptrdiff_t>(a.size())) == 'w');
    }

    SECTION("Operator-(int), tests -= as well") {
        begin += static_cast<std::ptrdiff_t>(a.size());
        CHECK(*begin == 'w');
    }

    SECTION("Operator-(Iterator)") {
        CHECK(static_cast<std::size_t>(concat.end() - begin) == a.size() + b.size());
        CHECK(static_cast<std::size_t>(std::distance(concat.begin(), concat.end())) == a.size() + b.size());
    }

    SECTION("Operator[]()") {
        CHECK(begin[static_cast<std::ptrdiff_t>(a.size())] == 'w');
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
    std::vector<int> v1 = { 1, 2, 3 };
    std::vector<int> v2 = { 4, 5, 6 };
    auto concat = lz::concat(v1, v2);

    SECTION("To array") {
        constexpr std::size_t size = 3 + 3;
        CHECK(concat.to<std::array<int, size>>() == std::array<int, size>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To vector") {
        CHECK(concat.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To other container using to<>()") {
        CHECK(concat.to<std::list<int>>() == std::list<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To map") {
        std::map<int, int> map = concat.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                        std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> map = concat.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                                  std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        CHECK(map == expected);
    }
}