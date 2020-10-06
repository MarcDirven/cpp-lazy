#include <Lz/Unique.hpp>
#include <list>
#include <catch.hpp>

TEST_CASE("Unique changing and creating elements", "[Unique][Basic functionality]") {
    std::array<int, 4> arr = {3, 2, 3, 1};
    auto unique = lz::unique(arr);
    auto beg = unique.begin();
    constexpr std::size_t size = 3;

    REQUIRE(*beg == 1);
    REQUIRE(static_cast<std::size_t>(std::distance(beg, unique.end())) == size);

    SECTION("Should be sorted") {
        CHECK(std::is_sorted(arr.begin(), arr.end()));
    }

    SECTION("Should be unique") {
        std::array<int, size> expected = {1, 2, 3};
        CHECK(expected == unique.toArray<size>());
    }
}

TEST_CASE("Unique binary operations", "[Unique][Binary ops]") {
    std::array<int, 4> arr = {3, 2, 3, 1};
    auto unique = lz::unique(arr);
    auto beg = unique.begin();

    SECTION("Operator++") {
        ++beg;
        CHECK(*beg == 2);
    }

    SECTION("Operator==, operator!=") {
        CHECK(beg != unique.end());
        beg = unique.end();
        CHECK(beg == unique.end());
    }
}

TEST_CASE("Unique to container", "[Unique][To container]") {
    std::array<int, 4> arr = {3, 2, 3, 1};
    constexpr std::size_t size = 3;
    auto unique = lz::unique(arr);

    SECTION("To array") {
        auto uniqueArray = unique.toArray<size>();
        std::array<int, size> expected = {1, 2, 3};
        CHECK(uniqueArray == expected);
    }

    SECTION("To vector") {
        auto uniqueVec = unique.toVector();
        std::vector<int> expected = {1, 2, 3};
        CHECK(uniqueVec == expected);
    }

    SECTION("To other container using to<>()") {
        auto uniqueList = unique.to<std::list>();
        std::list<int> expected = {1, 2, 3};
        CHECK(uniqueList == expected);
    }

    SECTION("To map") {
        std::map<int, int> actual = unique.toMap([](const int i) {
            return i;
        });

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = unique.toUnorderedMap([](const int i) {
            return i;
        });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        CHECK(expected == actual);
    }
}
