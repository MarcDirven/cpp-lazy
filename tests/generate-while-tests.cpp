#include <Lz/GenerateWhile.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Generate while changing and creating elements", "[Generate while][Basic functionality]") {
    const auto compileTest1 = lz::generateWhile([](bool) { return std::make_pair(false, false); }, false);
    static_cast<void>(compileTest1);
    const auto compileTest2 = lz::generateWhile([](const int&) { return std::make_pair(false, false); }, 0);
    static_cast<void>(compileTest2);

    auto generatorWithReference = lz::generateWhile([](int& i) -> std::pair<bool, int&> { return { i == 4, i }; }, 0);
    static_assert(std::is_same<decltype(*generatorWithReference.begin()), int&>::value,
                  "int& and decltype(*generator.begin()) are not the same");
                  

    SECTION("Should be 0, 1, 2, 3") {
        auto generator = lz::generateWhile(
            [](int& i) {
                auto copy = i++;
                return std::make_pair(copy != 4, copy);
            },
            0);
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::array<int, expected.size()>>();
        CHECK(expected == actual);
    }
}

TEST_CASE("Generate while binary operations", "[Generate while][Binary ops]") {
    auto generator = lz::generateWhile(
        [](int& i) {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        },
        0);
    static_assert(std::is_same<decltype(*generator.begin()), int>::value,
                  "int and decltype(*generator.begin()) are not the same");
    REQUIRE(*generator.begin() == 0);

    SECTION("Operator++") {
        auto begin = generator.begin();
        ++begin;
        CHECK(*begin == 1);
    }

    SECTION("Operator== & Operator!=") {
        auto begin = generator.begin();
        ++begin;
        CHECK(begin != generator.begin());
        ++begin, ++begin, ++begin;
        CHECK(begin == generator.end());
        begin = generator.begin();
        CHECK(*begin == 0);
        begin = generator.end();
        CHECK(begin == generator.end());
    }
}

TEST_CASE("Generate while to containers", "[GenerateWhile][To container]") {
    auto generator = lz::generateWhile(
        [](int& f) {
            auto copy = f++;
            return std::pair<int, int>{ copy < 4, copy };
        },
        0);

    SECTION("To array") {
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::array<int, 4>>();
        CHECK(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3 };
        auto actual = generator.toVector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::list<int>>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator.toMap([](int i) { return std::make_pair(i, i); });
        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator.toUnorderedMap([](int i) { return std::make_pair(i, i); });
        CHECK(actual == expected);
    }
}