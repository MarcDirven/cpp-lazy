#include <Lz/RegexSplit.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>


TEST_CASE("RegexSplit changing and creating elements", "[RegexSplit][Basic functionality]") {
    std::regex r1(R"(\s+)");

    SECTION("Empty string") {
        std::string s = "";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = {};
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Starting with delimiter") {
        std::string s = "    Hello, world! How are you?";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Ending with delimiter") {
        std::string s = "Hello, world! How are you?    ";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Starting and ending with delimiter") {
        std::string s = "    Hello, world! How are you?    ";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }
}

TEST_CASE("RegexSplit binary operations", "[RegexSplit][Binary ops]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regexSplit(s, r1);
    auto begin = splitter.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(lz::distance(begin, splitter.end()) == 4);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != splitter.end());
        while (begin != splitter.end()) {
            ++begin;
        }
        CHECK(begin == splitter.end());
    }
}

TEST_CASE("RegexSplit to containers", "[RegexSplit][To container]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regexSplit(s, r1);

    SECTION("To vector") {
        auto vec = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(vec.begin(), vec.end(), expected.begin()));
    }

    SECTION("To list") {
        auto list = splitter.transformAs<std::list<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::list<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(list.begin(), list.end(), expected.begin()));
    }
}
