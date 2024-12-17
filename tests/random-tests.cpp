#include <Lz/random.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("random_iterable should be random", "[random_iterable][Basic functionality]") {
    constexpr std::size_t size = 5;

    SECTION("random_iterable doubles") {
        const auto random_array = lz::random<long double>(0., 1., size).to<std::array<double, size>>();
        auto randomArray2 = lz::random<long double>(0., 1., size).to<std::array<double, size>>();
        while (random_array == randomArray2) {
            randomArray2 = lz::random<long double>(0., 1., size).to<std::array<double, size>>();
        }
        REQUIRE(random_array != randomArray2);
    }

    SECTION("random_iterable ints") {
        const auto random_array =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size).to<std::array<int, size>>();
        const auto randomArray2 =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size).to<std::array<int, size>>();
        REQUIRE(random_array != randomArray2);
    }
}

TEST_CASE("random_iterable with custom distro's and custom engine") {
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    std::poisson_distribution<> d(500000);
    auto r = lz::random(d, gen, 3);
    static_assert(!std::is_same<decltype(r.begin()), decltype(r.end())>::value, "Should not be the same");
    CHECK(lz::distance(r.begin(), r.end()) == 3);

    const auto current_rand = r.next_random();
    auto next_rand = r.next_random();
    while (current_rand == next_rand) {
        next_rand = r.next_random();
    }
    REQUIRE(current_rand != next_rand);
}

TEST_CASE("random_iterable binary operations", "[random_iterable][Binary ops]") {
    constexpr std::ptrdiff_t size = 5;
    auto random = lz::random(0., 1., size);
    auto it = random.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(lz::distance(it, random.end()) == 4);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != random.end());
        while (it != random.end()) {
            ++it;
        }
        CHECK(it == random.end());
    }
}

TEST_CASE("random_iterable to containers", "[random_iterable][To container]") {
    constexpr std::size_t size = 10;
    auto range = lz::random(0., 1., size);

    SECTION("To array") {
        CHECK(range.to<std::array<double, size>>().size() == size);
        CHECK(lz::all_of(range, [](double val) { return val >= 0. && val <= 1.; }));
    }

    SECTION("To vector") {
        CHECK(range.to_vector().size() == size);
    }

    SECTION("To other container using to<>()") {
        CHECK(range.to<std::list<double>>().size() == size);
    }

    SECTION("To map") {
        std::map<double, double> actual = range.to_map([](const double i) { return std::make_pair(i, i); });
        CHECK(actual.size() == size);
    }

    SECTION("To unordered map") {
        std::unordered_map<double, double> actual = range.to_unordered_map([](const double i) { return std::make_pair(i, i); });
        CHECK(actual.size() == size);
    }
}
