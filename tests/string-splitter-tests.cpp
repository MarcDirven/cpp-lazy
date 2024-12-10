#include <Lz/CString.hpp>
#include <Lz/Splitter.hpp>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <list>

TEST_CASE("Splitter permutations") {
    SECTION("Starting with one delimiter ending with none") {
        const std::string toSplit = " Hello world test 123";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        CHECK(splitter == expected);
    }

    SECTION("Starting with two delimiters ending with none") {
        const std::string toSplit = "  Hello world test 123";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        CHECK(splitter == expected);
    }

    SECTION("Ending with one delimiter starting with none") {
        const std::string toSplit = "Hello world test 123 ";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        CHECK(splitter == expected);
    }

    SECTION("Ending with two delimiters starting with none") {
        const std::string toSplit = "Hello world test 123  ";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        CHECK(splitter == expected);
    }

    SECTION("Starting and ending with one delimiter") {
        const std::string toSplit = " Hello world test 123 ";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        CHECK(splitter == expected);
    }

    SECTION("Starting and ending with two delimiters") {
        const std::string toSplit = "  Hello world test 123  ";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        CHECK(splitter == expected);
    }

    SECTION("No delimiters at all") {
        const std::string toSplit = "Hello world test 123";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        CHECK(splitter == expected);
    }

    SECTION("Multiple delimiters in the middle") {
        const std::string toSplit = "Hello  world  test  123";
        auto splitter = lz::split(toSplit, " ").transformAs<std::vector<std::string>>([](const auto& v) { return v.toString(); });
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        CHECK(splitter == expected);
    }
}

TEST_CASE("Splitter changing and creating elements", "[Splitter][Basic functionality]") {
    const std::string toSplit = "Hello  world  test  123  ";
    const char* delimiter = "  ";
    auto splitter = lz::split(toSplit, delimiter);

    auto it = splitter.begin();

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(static_cast<std::size_t>(splitter.distance()) == expected.size());
        for (std::size_t i = 0; i < expected.size(); i++, ++it) {
            auto string = lz::toStringView(*it);
            CHECK(string == expected[i]);
        }
        CHECK(it == splitter.end());
    }

    SECTION("Should be std::string") {
        CHECK(std::is_same<decltype(*it), lz::View<decltype(toSplit.begin())>>::value);
    }
}

TEST_CASE("Splitter binary operations", "[Splitter][Binary ops]") {
    std::string toSplit = " Hello world test 123 ";
    auto splitter = lz::split(toSplit, " ");
    auto it = splitter.begin();

    CHECK(it->empty());

    SECTION("Operator++") {
        CHECK(it->empty());
        ++it;
        CHECK(lz::toStringView(*it) == "Hello");
        ++it;
        CHECK(lz::toStringView(*it) == "world");
        ++it;
        CHECK(lz::toStringView(*it) == "test");
        ++it;
        CHECK(lz::toStringView(*it) == "123");
        ++it;
        CHECK(it->empty());
        CHECK(it != splitter.end());
        ++it;
        CHECK(it == splitter.end());
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != splitter.end());
        while (it != splitter.end()) {
            ++it;
        }
        CHECK(it == splitter.end());
    }
}

TEST_CASE("Splitter to containers", "[Splitter][To container]") {
    std::string toSplit = "Hello world test 123 ";
    auto splitter = lz::split(toSplit, " ");
    using View = typename decltype(splitter.begin())::value_type;

    SECTION("To array") {
        auto actual = splitter.transformAs<std::array<std::string, 5>>([](const View& v) { return v.toString(); });
        std::array<std::string, actual.size()> expected = { "Hello", "world", "test", "123", "" };

        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<std::string> actual =
            splitter.transformAs<std::vector<std::string>>([](const View& v) { return v.toString(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };

        CHECK(actual == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> actual = splitter.transformAs<std::list<std::string>>([](const View& v) { return v.toString(); });
        std::list<std::string> expected = { "Hello", "world", "test", "123", "" };

        CHECK(actual == expected);
    }

    SECTION("To map") {
        std::map<std::string, std::string> actual =
            splitter.toMap([](const View& v) { return std::make_pair(v.toString(), v.toString()); });
        std::map<std::string, std::string> expected = {
            std::make_pair(std::string("Hello"), std::string("Hello")),
            std::make_pair(std::string("world"), std::string("world")),
            std::make_pair(std::string("test"), std::string("test")),
            std::make_pair(std::string("123"), std::string("123")),
            std::make_pair(std::string(""), std::string("")),
        };

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::string, std::string> actual =
            splitter.toUnorderedMap([](const View& v) { return std::make_pair(v.toString(), v.toString()); });
        std::unordered_map<std::string, std::string> expected = { std::make_pair(std::string("Hello"), std::string("Hello")),
                                                                  std::make_pair(std::string("world"), std::string("world")),
                                                                  std::make_pair(std::string("test"), std::string("test")),
                                                                  std::make_pair(std::string("123"), std::string("123")),
                                                                  std::make_pair(std::string(""), std::string("")) };

        CHECK(actual == expected);
    }
}
