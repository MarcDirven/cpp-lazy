#include <list>

#include <catch.hpp>
#include <Lz/StringSplitter.hpp>


TEST_CASE("String splitter changing and creating elements", "[String splitter][Basic functionality]") {
    std::string toSplit = "Hello world test 123";
    std::string delimiter = " ";
    auto splitter = lz::split(toSplit, std::move(delimiter));
    auto it = splitter.begin();

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = {"Hello", "world", "test", "123"};

        for (size_t i = 0; i < expected.size(); i++, ++it) {
            CHECK(*it == expected[i]);
        }
    }

#if __cplusplus < 201703L
    SECTION("Should be const std::string&") {
        CHECK(std::is_same<decltype(*it), const std::string&>::value);
    }
#else
    SECTION("Should be std::string_view") {
        CHECK(std::is_same<decltype(*it), std::string_view>::value);
    }
#endif
}

TEST_CASE("String splitter binary operations", "[String splitter][Binary ops]") {
    std::string toSplit = "Hello world test 123";
    std::string delimiter = " ";
    auto splitter = lz::split(toSplit, std::move(delimiter));
    auto it = splitter.begin();

    CHECK(*it == "Hello");

    SECTION("Operator++") {
        ++it;
        CHECK(*it == "world");
    }
}


TEST_CASE("String splitter to containers", "[String splitter][To container]") {
    std::string toSplit = "Hello world test 123";
    std::string delimiter = " ";
    auto splitter = lz::split(toSplit, std::move(delimiter));

    SECTION("To array") {
        std::array<std::string, 4> actual = splitter.toArray<4>();
        std::array<std::string, 4> expected = {"Hello", "world", "test", "123"};

        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<std::string> actual = splitter.toVector();
        std::vector<std::string> expected = {"Hello", "world", "test", "123"};

        CHECK(actual == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> actual = splitter.to<std::list>();
        std::list<std::string> expected = {"Hello", "world", "test", "123"};

        CHECK(actual == expected);
    }
}
