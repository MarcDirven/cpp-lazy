#include <Lz/c_string.hpp>
#include <Lz/unique.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Unique using sentinels") {
    auto str = lz::c_string("aabbcccddefgghhj");
    auto unique = lz::unique(str);
    static_assert(!std::is_same<decltype(unique.begin()), decltype(unique.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("abcdefghj");
    CHECK(lz::equal(unique, expected));
}

TEST_CASE("Unique changing and creating elements", "[Unique][Basic functionality]") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    auto unique = lz::unique(arr);
    auto beg = unique.begin();
    constexpr std::size_t size = 3;

    REQUIRE(*beg == 1);
    REQUIRE(static_cast<std::size_t>(std::distance(beg, unique.end())) == size);

    SECTION("Should be unique") {
        std::array<int, size> expected = { 1, 2, 3 };
        CHECK(expected == unique.to<std::array<int, size>>());
    }

    SECTION("Should be unique too, using >") {
        std::array<int, size> expected = { 3, 2, 1 };
        auto uniqueGreater = lz::unique_iterable(expected, std::greater<int>());
        CHECK(expected == uniqueGreater.to<std::array<int, size>>());
        CHECK(std::is_sorted(expected.begin(), expected.end(), std::greater<int>()));
    }
}

TEST_CASE("Unique binary operations", "[Unique][Binary ops]") {
    SECTION("Operator++") {
        std::array<int, 4> arr = { 3, 2, 3, 1 };
        std::sort(arr.begin(), arr.end());
        auto unique = lz::unique(arr);
        auto beg = unique.begin();
        ++beg;
        CHECK(*beg == 2);
    }

    SECTION("Operator==, operator!=") {
        std::array<int, 4> arr = { 3, 2, 3, 1 };
        std::sort(arr.begin(), arr.end());
        auto unique = lz::unique(arr);
        auto beg = unique.begin();
        CHECK(beg != unique.end());
        beg = unique.end();
        CHECK(beg == unique.end());
    }

    SECTION("Operator--") {
        // 3, 3, 3, 2, 2, 1
        std::array<int, 6> arr = { 3, 2, 3, 1, 3, 2 };
        std::sort(arr.begin(), arr.end(), std::greater<int>());
        auto unique = lz::unique(arr);
        auto iter = unique.end();
        --iter;
        CHECK(*iter == 1);
        --iter;
        CHECK(*iter == 2);
        CHECK(&*iter == &arr[3]);
        --iter;
        CHECK(*iter == 3);
        CHECK(iter == unique.begin());
    }
}

TEST_CASE("Unique to container", "[Unique][To container]") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    constexpr std::size_t size = 3;
    auto unique = lz::unique(arr);

    SECTION("To array") {
        auto uniqueArray = unique.to<std::array<int, size>>();
        std::array<int, size> expected = { 1, 2, 3 };
        CHECK(uniqueArray == expected);
    }

    SECTION("To vector") {
        auto uniqueVec = unique.to_vector();
        std::vector<int> expected = { 1, 2, 3 };
        CHECK(uniqueVec == expected);
    }

    SECTION("To other container using to<>()") {
        auto uniqueList = unique.to<std::list<int>>();
        std::list<int> expected = { 1, 2, 3 };
        CHECK(uniqueList == expected);
    }

    SECTION("To map") {
        std::map<int, int> actual = unique.to_map([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = unique.to_unordered_map([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        CHECK(expected == actual);
    }
}
