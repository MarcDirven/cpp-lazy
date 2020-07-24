#include <list>

#include <catch.hpp>
#include <Lz/Zip.hpp>


TEST_CASE("Zip changing and creating elements", "[Zip][Basic functionality]") {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<float> b = {1.f, 2.f, 3.f, 4.f};
    std::array<short, 4> c = {1, 2, 3, 4};

    SECTION("Should zip") {
        size_t i = 0;
        for (auto tup : lz::zip(a, b, c)) {
            CHECK(tup == std::make_tuple(a[i], Approx(b[i]), c[i]));
            ++i;
        }
    }

    SECTION("Should stop at smallest container") {
        std::vector<int> smallest = {1, 2};
        size_t counter = 0;

        for (auto tup : lz::zip(a, b, smallest)) {
            (void) tup;
            counter++;
        }

        CHECK(counter == smallest.size());
    }

    SECTION("Should be by ref") {
        size_t i = 0;
        for (auto tup : lz::zip(a, b, c)) {
            auto& aElement = std::get<0>(tup);
            auto& bElement = std::get<1>(tup);
            auto& cElement = std::get<2>(tup);

            CHECK(&aElement == &a[i]);
            CHECK(&bElement == &b[i]);
            CHECK(&cElement == &c[i]);

            ++i;
        }
    }
}

TEST_CASE("Zip binary operations", "[Zip][Binary ops]") {
    constexpr size_t size = 4;
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<float> b = {1.f, 2.f, 3.f, 4.f};
    std::array<short, size> c = {1, 2, 3, 4};

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

        std::array<short, 3> shortest = {1, 2, 3};
        auto zip = lz::zip(c, shortest);
        CHECK(std::distance(zip.begin(), zip.end()) == 3);
    }

    SECTION("Operator[]()") {
        size_t idx = 0;
        CHECK(zipper.begin()[idx] == std::make_tuple(a[idx], Approx(b[idx]), c[idx]));
    }

    SECTION("Operator<, <, <=, >, >=") {
        CHECK(zipper.begin() < zipper.end());
#if !(defined(_MSC_VER))
        CHECK(zipper.begin() + size + 1 > zipper.end());
#endif
        CHECK(zipper.begin() + size <= zipper.end());
        CHECK(zipper.begin() + size >= zipper.end());
    }
}

TEST_CASE("Zip to containers", "[Zip][To container]") {
    constexpr size_t size = 4;
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<float> b = {1.f, 2.f, 3.f, 4.f};
    std::array<short, size> c = {1, 2, 3, 4};

    SECTION("To array") {
        auto array = lz::zip(a, b, c).toArray<size>();

        for (size_t i = 0; i < array.size(); i++) {
            auto& aElement = std::get<0>(array[i]);
            auto& bElement = std::get<1>(array[i]);
            auto& cElement = std::get<2>(array[i]);

            CHECK(aElement == a[i]);
            CHECK(bElement == b[i]);
            CHECK(cElement == c[i]);
        }
    }

    SECTION("To vector") {
        auto vector = lz::zip(a, b, c).toVector();

        for (size_t i = 0; i < vector.size(); i++) {
            auto& aElement = std::get<0>(vector[i]);
            auto& bElement = std::get<1>(vector[i]);
            auto& cElement = std::get<2>(vector[i]);

            CHECK(aElement == a[i]);
            CHECK(bElement == b[i]);
            CHECK(cElement == c[i]);
        }
    }

    SECTION("To other container using to<>()") {
        auto list = lz::zip(a, b, c).to<std::list>();
        auto listIterator = list.begin();

        for (size_t i = 0; i < list.size(); i++) {
            auto& aElement = std::get<0>(*listIterator);
            auto& bElement = std::get<1>(*listIterator);
            auto& cElement = std::get<2>(*listIterator);

            CHECK(aElement == a[i]);
            CHECK(bElement == b[i]);
            CHECK(cElement == c[i]);

            ++listIterator;
        }
    }

    SECTION("To map") {
        std::map<int, std::tuple<int, float, short>> actual = lz::zip(a, b, c).toMap(
            [](const auto& tup) {
                return std::get<0>(tup);
            });
        std::map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, 1)),
            std::make_pair(2, std::make_tuple(2, 2.f, 2)),
            std::make_pair(3, std::make_tuple(3, 3.f, 3)),
            std::make_pair(4, std::make_tuple(4, 4.f, 4))

        };

        CHECK(actual == expected);
    }

    SECTION("To map") {
        std::unordered_map<int, std::tuple<int, float, short>> actual = lz::zip(a, b, c).toUnorderedMap(
            [](const auto& tup) {
                return std::get<0>(tup);
            });

        std::unordered_map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, 1)),
            std::make_pair(2, std::make_tuple(2, 2.f, 2)),
            std::make_pair(3, std::make_tuple(3, 3.f, 3)),
            std::make_pair(4, std::make_tuple(4, 4.f, 4))

        };

        CHECK(actual == expected);
    }
}
