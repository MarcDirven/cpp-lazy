#include <Lz/Zip.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("zip_iterable changing and creating elements", "[zip_iterable][Basic functionality]") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, 4> c = { 1, 2, 3, 4 };

    SECTION("Unequal lengths") {
        std::vector<int> ints = { 1, 2, 3, 4, 5 };
        std::vector<double> floats = { 1.2, 3.3 };
        auto zipper = lz::zip(ints, floats);
        auto end = zipper.end();
        CHECK(*--end == std::make_tuple(2, 3.3));
        CHECK(*--end == std::make_tuple(1, 1.2));
        CHECK(end == zipper.begin());
        CHECK(std::distance(zipper.begin(), zipper.end()) == 2);
    }

    SECTION("Should compile") {
        std::vector<int> v = {1, 2, 3, 4, 5};
        std::list<int> v2 = {1, 2, 3};
        auto zipper = lz::zip(v, v2);
        CHECK(std::distance(zipper.begin(), zipper.end()) == 3);
    }

    SECTION("Should zip") {
        std::size_t i = 0;
        for (const auto& tup : lz::zip(a, b, c)) {
            CHECK(tup == std::make_tuple(a[i], Approx(b[i]), c[i]));
            ++i;
        }
    }

    SECTION("Should stop at smallest container") {
        std::vector<int> smallest = { 1, 2 };
        std::size_t counter = 0;

        for (const auto& tup : lz::zip(a, b, smallest)) {
            (void)tup;
            counter++;
        }

        CHECK(counter == smallest.size());
    }

    SECTION("Should be by ref") {
        std::size_t i = 0;
        for (auto tup : lz::zip(a, b, c)) {
            auto& a_element = std::get<0>(tup);
            auto& b_element = std::get<1>(tup);
            auto& c_element = std::get<2>(tup);

            CHECK(&a_element == &a[i]);
            CHECK(&b_element == &b[i]);
            CHECK(&c_element == &c[i]);

            ++i;
        }
    }
}

TEST_CASE("zip_iterable binary operations", "[zip_iterable][Binary ops]") {
    constexpr std::size_t size = 4;
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, size> c = { 1, 2, 3, 4 };

    auto zipper = lz::zip(a, b, c);
    auto begin = zipper.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(*begin == std::make_tuple(a[1], Approx(b[1]), c[1]));
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        CHECK(*begin == std::make_tuple(a[0], Approx(b[0]), c[0]));
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != zipper.end());
        begin = zipper.end();
        CHECK(begin == zipper.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(begin + 2) == std::make_tuple(a[2], Approx(b[2]), c[2]));
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++begin;
        CHECK(*(begin - 1) == std::make_tuple(a[0], Approx(b[0]), c[0]));
    }

    SECTION("Operator-(Iterator)") {
        CHECK((zipper.end() - zipper.begin()) == 4);

        std::array<short, 3> shortest = { 1, 2, 3 };
        auto zip = lz::zip(c, shortest);
        CHECK(std::distance(zip.begin(), zip.end()) == 3);
    }

    SECTION("Operator[]()") {
        std::size_t idx = 0;
        CHECK(zipper.begin()[static_cast<std::ptrdiff_t>(idx)] == std::make_tuple(a[idx], Approx(b[idx]), c[idx]));
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(zipper.begin() < zipper.end());
        CHECK(zipper.begin() + 1 > zipper.begin());
        CHECK(zipper.begin() + size <= zipper.end());
        CHECK(zipper.begin() + size >= zipper.end());
    }
}

TEST_CASE("zip_iterable to containers", "[zip_iterable][To container]") {
    constexpr std::size_t size = 4;
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, size> c = { 1, 2, 3, 4 };

    SECTION("To array") {
        auto array = lz::zip(a, b, c).to<std::array<std::tuple<int, float, short>, size>>();

        for (std::size_t i = 0; i < array.size(); i++) {
            auto& a_element = std::get<0>(array[i]);
            auto& b_element = std::get<1>(array[i]);
            auto& c_element = std::get<2>(array[i]);

            CHECK(a_element == a[i]);
            CHECK(b_element == b[i]);
            CHECK(c_element == c[i]);
        }
    }

    SECTION("To vector") {
        auto vector = lz::zip(a, b, c).to_vector();

        for (std::size_t i = 0; i < vector.size(); i++) {
            auto& a_element = std::get<0>(vector[i]);
            auto& b_element = std::get<1>(vector[i]);
            auto& c_element = std::get<2>(vector[i]);

            CHECK(a_element == a[i]);
            CHECK(b_element == b[i]);
            CHECK(c_element == c[i]);
        }
    }

    SECTION("To other container using to<>()") {
        auto list = lz::zip(a, b, c).to<std::list<std::tuple<int, float, short>>>();
        auto listIterator = list.begin();

        for (std::size_t i = 0; i < list.size(); i++) {
            auto& a_element = std::get<0>(*listIterator);
            auto& b_element = std::get<1>(*listIterator);
            auto& c_element = std::get<2>(*listIterator);

            CHECK(a_element == a[i]);
            CHECK(b_element == b[i]);
            CHECK(c_element == c[i]);

            ++listIterator;
        }
    }

    SECTION("To map") {
        using Tuple = std::tuple<int, float, short>;
        std::map<int, Tuple> actual =
            lz::zip(a, b, c).to_map([](const Tuple& tup) { return std::make_pair(std::get<0>(tup), tup); });
        std::map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
            std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
            std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
            std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4)))
        };

        CHECK(actual == expected);
    }

    SECTION("To map") {
        using Tuple = std::tuple<int, float, short>;
        std::unordered_map<int, Tuple> actual =
            lz::zip(a, b, c).to_unordered_map([](const Tuple& tup) { return std::make_pair(std::get<0>(tup), tup); });

        std::unordered_map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
            std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
            std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
            std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4)))

        };

        CHECK(actual == expected);
    }
}
