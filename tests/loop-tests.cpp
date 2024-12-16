#include <Lz/CString.hpp>
#include <Lz/Loop.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Loop tests with sentinels") {
    auto cstr = lz::cString("Hello");
    auto looper = lz::loop(cstr);
    static_assert(!std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should be sentinel");
    CHECK(*looper.begin() == 'H');
    auto looperIt = looper.begin();
    auto cStrIt = cstr.begin();
    for (std::size_t i = 0; i < 100; ++i) {
        REQUIRE(*looperIt == *cStrIt);
        ++looperIt;
        ++cStrIt;
        if (cStrIt == cstr.end()) {
            cStrIt = cstr.begin();
        }
    }
}

TEST_CASE("Basic functionality loop", "[Loop][Basic functionality]") {
    std::vector<int> vec = { 1, 2, 3, 4 };
    auto looper = lz::loop(vec);
    static_assert(std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should not be sentinel");

    SECTION("Distance") {
        CHECK(std::distance(looper.begin(), looper.end()) ==
              (std::numeric_limits<decltype(vec)::iterator::difference_type>::max)());
    }

    SECTION("Going a circle") {
        CHECK(*(looper.begin() + static_cast<std::ptrdiff_t>(vec.size())) == 1);
        CHECK(*(looper.begin() + (static_cast<std::ptrdiff_t>(vec.size()) - 1)) == 4);

        CHECK(*(looper.end() - static_cast<std::ptrdiff_t>(vec.size())) == 1);
        CHECK(*(looper.end() - (static_cast<std::ptrdiff_t>(vec.size()) + 1)) == 4);
    }

    SECTION("Always true") {
        CHECK(looper.begin() != looper.end());
        CHECK(!(looper.begin() == looper.begin()));
        CHECK(looper.begin() + static_cast<std::ptrdiff_t>(vec.size()) != looper.end());

        CHECK(looper.begin() < looper.end());
        CHECK(!(looper.begin() > looper.end()));
        CHECK(!(looper.begin() >= looper.end()));
        CHECK(looper.begin() <= looper.end());

        CHECK(!(looper.end() < looper.begin()));
        CHECK(looper.end() > looper.begin());
        CHECK(looper.end() >= looper.begin());
        CHECK(!(looper.end() <= looper.begin()));
    }
}