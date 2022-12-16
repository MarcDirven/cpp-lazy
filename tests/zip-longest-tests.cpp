#define CATCH_CONFIG_MAIN

#include <Lz/ZipLongest.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("ZipLongest changing and creating elements", "[ZipLongest][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    std::list<char> v2 = { 'a', 'b', 'c' };
    std::vector<char> v3 = { 'a', 'b', 'c', 'd' };

    auto fwd = lz::zipLongest(v, v2, v3);
    auto ra = lz::zipLongest(v, v3);
    static_assert(lz::internal::IsForward<decltype(fwd.begin())>::value, "");
    static_assert(lz::internal::IsRandomAccess<decltype(ra.begin())>::value, "");

    SECTION("Unequal lengths") {
        CHECK(std::distance(fwd.begin(), fwd.end()) == 7);
        CHECK(std::distance(ra.begin(), ra.end()) == 7);
    }

    SECTION("Should zip longest") {
        std::size_t counter = 0;
        for (auto&& tup : fwd) {
            auto&& a = std::get<0>(tup);
            auto&& b = std::get<1>(tup);
            auto&& c = std::get<2>(tup);

            CHECK(*a == v[counter]);

            if (counter > 2) {
                CHECK(!b);
            }
            else {
                CHECK(b);
                CHECK(*b == *std::next(v2.begin(), counter));
            }
            if (counter > 3) {
                CHECK(!c);
            }
            else {
                CHECK(c);
                CHECK(*c == v3[counter]);
            }
            counter++;
        }

        counter = 0;

        for (auto&& tup : ra) {
            auto&& a = std::get<0>(tup);
            auto&& b = std::get<1>(tup);
            CHECK(*a == v[counter]);
            if (counter > 3) {
                CHECK(!b);
            }
            else {
                CHECK(b);
                CHECK(*b == v3[counter]);
            }
            counter++;
        }
    }

    SECTION("Should not be by ref") {
        auto begin = ra.begin();
        auto&& first = std::get<0>(*begin);
        first = 2'000;
        CHECK(v[0] != *first);
    }
}

TEST_CASE("ZipLongest binary operations", "[ZipLongest][Binary ops]") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f, 5.f };
    std::array<short, 2> c = { 1, 2 };

    auto zipper = lz::zipLongest(a, b, c);
    auto begin = zipper.begin();

    CHECK(*std::get<0>(*begin) == 1);
    CHECK(*std::get<1>(*begin) == 1.f);
    CHECK(*std::get<2>(*begin) == 1);

    SECTION("Operator++") {
        ++begin;
        CHECK(*std::get<0>(*begin) == 2);
        CHECK(*std::get<1>(*begin) == 2.f);
        CHECK(*std::get<2>(*begin) == 2);
        ++begin;
        CHECK(*std::get<0>(*begin) == 3);
        CHECK(*std::get<1>(*begin) == 3.f);
        CHECK(!std::get<2>(*begin));
    }

    SECTION("Operator--") {
        auto end = zipper.end();
        --end;
        CHECK(!std::get<0>(*end));
        CHECK(*std::get<1>(*end) == 5.f);
        CHECK(!std::get<2>(*end));
        --end;
        CHECK(*std::get<0>(*end) == 4);
        CHECK(*std::get<1>(*end) == 4.f);
        CHECK(!std::get<2>(*end));
        --end, --end;
        CHECK(*std::get<0>(*end) == 2);
        CHECK(*std::get<1>(*end) == 2.f);
        CHECK(*std::get<2>(*end) == 2);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != zipper.end());
        begin = zipper.end();
        CHECK(begin == zipper.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*std::get<0>(*(begin + 1)) == 2);
        CHECK(*std::get<1>(*(begin + 1)) == 2.f);
        CHECK(*std::get<2>(*(begin + 1)) == 2);

        CHECK(*std::get<0>(*(begin + 2)) == 3);
        CHECK(*std::get<1>(*(begin + 2)) == 3.f);
        CHECK(!std::get<2>(*(begin + 2)));

        CHECK(*std::get<0>(*(begin + 3)) == 4);
        CHECK(*std::get<1>(*(begin + 3)) == 4.f);
        CHECK(!std::get<2>(*(begin + 3)));

        CHECK(!std::get<0>(*(begin + 4)));
        CHECK(*std::get<1>(*(begin + 4)) == 5.f);
        CHECK(!std::get<2>(*(begin + 4)));
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        auto end = zipper.end();
        CHECK(!std::get<0>(*(end - 1)));
        CHECK(*std::get<1>(*(end - 1)) == 5.f);
        CHECK(!std::get<2>(*(end - 1)));

        CHECK(*std::get<0>(*(end - 2)) == 4);
        CHECK(*std::get<1>(*(end - 2)) == 4.f);
        CHECK(!std::get<2>(*(end - 2)));

        CHECK(*std::get<0>(*(end - 3)) == 3);
        CHECK(*std::get<1>(*(end - 3)) == 3.f);
        CHECK(!std::get<2>(*(end - 3)));

        CHECK(*std::get<0>(*(end - 4)) == 2);
        CHECK(*std::get<1>(*(end - 4)) == 2.f);
        CHECK(*std::get<2>(*(end - 4)) == 2);

        CHECK(*std::get<0>(*(end - 5)) == 1);
        CHECK(*std::get<1>(*(end - 5)) == 1.f);
        CHECK(*std::get<2>(*(end - 5)) == 1);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size()));
        ++begin;
        CHECK(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 1));
    }

    SECTION("Operator[]()") {
        CHECK(*std::get<1>(begin[4]) == 5.f);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto end = zipper.end();
        CHECK(begin < end);
        CHECK(begin + 5 > end - 1);
        CHECK(begin + 5 >= end);
        CHECK(begin <= end - 5);
    }
}