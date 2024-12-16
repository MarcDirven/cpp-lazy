#include <Lz/CString.hpp>
#include <Lz/Map.hpp>
#include <catch2/catch.hpp>
#include <list>

struct TestStruct {
    std::string testFieldStr;
    int testFieldInt;
};

TEST_CASE("Map with sentinels") {
    auto cstr = lz::c_string("Hello, World!");
    auto map = lz::map(cstr, [](char c) { return std::toupper(c); });
    static_assert(!std::is_same<decltype(map.end()), decltype(map.begin())>::value, "Should be sentinels");
    auto cstrExpected = lz::c_string("HELLO, WORLD!");
    CHECK(lz::equal(map, cstrExpected));
}

TEST_CASE("Map changing and creating elements", "[Map][Basic functionality]") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };

    SECTION("Should map out element") {
        auto map = lz::map(array, [](const TestStruct& t) { return t.testFieldStr; });
        static_assert(std::is_same<decltype(map.end()), decltype(map.begin())>::value, "Should not be sentinels");
        static_assert(std::is_same<decltype(*map.begin()), std::string>::value, "Types to not match (decltype(*map.begin()) and std::string)");

        auto it = map.begin();
        CHECK(*it == "FieldA");
        CHECK(*(++it) == "FieldB");
        CHECK(*(++it) == "FieldC");
    }

    SECTION("Should be by reference") {
        std::size_t count = 0;
        std::function<std::string&(TestStruct&)> f = [&count, &array](TestStruct& t) -> std::string& {
            CHECK(&t == &array[count++]);
            return t.testFieldStr;
        };
        auto map = lz::map(array, std::move(f));

        for (auto&& _ : map) {
            static_cast<void>(_);
        }
    }
}

TEST_CASE("Map binary operations", "[Map][Binary ops]") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };

    std::function<std::string(TestStruct)> f = [](const TestStruct& t) {
        return t.testFieldStr;
    };
    auto map = lz::map(array, std::move(f));
    auto it = map.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(*it == array[1].testFieldStr);
    }

    SECTION("Operator--") {
        ++it;
        --it;
        CHECK(*it == array[0].testFieldStr);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != map.end());
        it = map.end();
        CHECK(it == map.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 1) == array[1].testFieldStr);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++it;
        CHECK(*(it - 1) == array[0].testFieldStr);
    }

    SECTION("Operator-(Iterator)") {
        CHECK((map.end() - map.begin()) == 3);
    }

    SECTION("Operator[]()") {
        CHECK(it[1] == "FieldB");
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = map.begin();
        auto end = map.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + size - 1 >= end - 1);
    }
}

TEST_CASE("Map to containers", "[Map][To container]") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };
    auto map = lz::map(array, [](const TestStruct& t) { return t.testFieldStr; });

    SECTION("To array") {
        auto stringArray = map.to<std::array<std::string, size>>();

        for (std::size_t i = 0; i < array.size(); i++) {
            CHECK(stringArray[i] == array[i].testFieldStr);
        }
    }

    SECTION("To vector") {
        auto stringVector = map.to_vector();

        for (std::size_t i = 0; i < array.size(); i++) {
            CHECK(stringVector[i] == array[i].testFieldStr);
        }
    }

    SECTION("To other container using to<>()") {
        auto stringList = map.to<std::list<std::string>>();
        auto listIterator = stringList.begin();

        for (std::size_t i = 0; i < array.size(); i++, ++listIterator) {
            CHECK(*listIterator == array[i].testFieldStr);
        }
    }

    SECTION("To map") {
        std::map<std::string, std::string> actual = map.to_map([](const std::string& s) { return std::make_pair(s, s); });

        std::map<std::string, std::string> expected = {
            std::make_pair("FieldA", "FieldA"),
            std::make_pair("FieldB", "FieldB"),
            std::make_pair("FieldC", "FieldC"),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::string, std::string> actual =
            map.to_unordered_map([](const std::string& s) { return std::make_pair(s, s); });

        std::unordered_map<std::string, std::string> expected = {
            std::make_pair("FieldA", "FieldA"),
            std::make_pair("FieldB", "FieldB"),
            std::make_pair("FieldC", "FieldC"),
        };

        CHECK(actual == expected);
    }
}
