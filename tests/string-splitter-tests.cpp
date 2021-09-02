#include <Lz/StringSplitter.hpp>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <list>

TEST_CASE("String splitter changing and creating elements", "[String splitter][Basic functionality]") {
    const std::string toSplit = "Hello  world  test  123  ";
    std::string delimiter = "  ";
    auto splitter = lz::split(toSplit, std::move(delimiter));

    auto it = splitter.begin();

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };

        for (std::size_t i = 0; i < expected.size(); i++, ++it) {
            CHECK(*it == expected[i]);
        }
    }

#ifndef LZ_HAS_STRING_VIEW
    SECTION("Should be std::string") {
        CHECK(std::is_same<decltype(*it), fmt::string_view>::value);
    }
#else
    SECTION("Should be std::string_view") {
        CHECK(std::is_same<decltype(*it), std::string_view>::value);
    }

    SECTION("Should contain std::string_view correctly") {
        std::vector<std::string_view> actual = splitter.toVector();
        std::vector<std::string_view> expected = { "Hello", "world", "test", "123" };

        CHECK(actual == expected);
    }
#endif
}

TEST_CASE("String splitter binary operations", "[String splitter][Binary ops]") {
    std::string toSplit = " Hello world test 123 ";
    auto splitter = lz::split(toSplit, ' ');
    auto it = splitter.begin();

    CHECK(it->empty());

    SECTION("Operator++") {
        CHECK(it->empty());
        ++it;
        CHECK(*it == "Hello");
        ++it;
        CHECK(*it == "world");
        ++it;
        CHECK(*it == "test");
        ++it;
        CHECK(*it == "123");
        ++it;
        CHECK(it->empty());
        ++it;
        CHECK(it == splitter.end());
    }

    SECTION("Operator--") {
        std::string test = " hello world test 123  ";
        auto spl = lz::split(test, ' ');
        auto end = spl.end();
        --end;
        CHECK(end->empty());
        --end;
        CHECK(end->empty());
        --end;
        CHECK(*end == "123");
        --end;
        CHECK(*end == "test");
        --end;
        CHECK(*end == "world");
        --end;
        CHECK(*end == "hello");
        --end;
        CHECK(end->empty());
        CHECK(end == spl.begin());
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != splitter.end());
        it = splitter.end();
        CHECK(it == splitter.end());
    }
}

TEST_CASE("String splitter to containers", "[String splitter][To container]") {
    std::string toSplit = "Hello world test 123";
    std::string delimiter = " ";
    auto splitter = lz::split<std::string>(toSplit, std::move(delimiter));

    SECTION("To array") {
        std::array<std::string, 4> actual = splitter.toArray<4>();
        std::array<std::string, 4> expected = { "Hello", "world", "test", "123" };

        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<std::string> actual = splitter.toVector();
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };

        CHECK(actual == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> actual = splitter.to<std::list>();
        std::list<std::string> expected = { "Hello", "world", "test", "123" };

        CHECK(actual == expected);
    }

    SECTION("To map") {
        std::map<std::string, std::string> actual = splitter.toMap([](const std::string& s) { return s; });
        std::map<std::string, std::string> expected = {
            std::make_pair(std::string("Hello"), std::string("Hello")),
            std::make_pair(std::string("world"), std::string("world")),
            std::make_pair(std::string("test"), std::string("test")),
            std::make_pair(std::string("123"), std::string("123")),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::string, std::string> actual = splitter.toUnorderedMap([](const std::string& s) { return s; });
        std::unordered_map<std::string, std::string> expected = {
            std::make_pair(std::string("Hello"), std::string("Hello")),
            std::make_pair(std::string("world"), std::string("world")),
            std::make_pair(std::string("test"), std::string("test")),
            std::make_pair(std::string("123"), std::string("123")),
        };

        CHECK(actual == expected);
    }
}
