#include <Lz/RegexSplit.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>


TEST_CASE("RegexSplit changing and creating elements", "[RegexSplit][Basic functionality]") {
    std::regex r1(R"(\s+)");

    SECTION("Empty string") {
        std::string s = "";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector>([](const auto& s) { return std::string(s.data(), s.size()); });
        std::vector<std::string> expected = {};
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("Starting with delimiter") {
        std::string s = "    Hello, world! How are you?";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector>([](const auto& s) { return std::string(s.data(), s.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("Ending with delimiter") {
        std::string s = "Hello, world! How are you?    ";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector>([](const auto& s) { return std::string(s.data(), s.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("Starting and ending with delimiter") {
        std::string s = "    Hello, world! How are you?    ";
        auto splitter = lz::regexSplit(s, r1);
        auto actual = splitter.transformAs<std::vector>([](const auto& s) { return std::string(s.data(), s.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }
}

TEST_CASE("RegexSplit binary operations", "[RegexSplit][Binary ops]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regexSplit(s, r1);
    auto begin = splitter.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(std::distance(begin, splitter.end()) == 4);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != splitter.end());
        begin = splitter.end();
        CHECK(begin == splitter.end());
    }
}

TEST_CASE("RegexSplit to containers", "[RegexSplit][To container]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regexSplit(s, r1);

    SECTION("To vector") {
        auto vec = splitter.transformAs<std::vector>([](const auto s) { return std::string(s.data(), s.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(vec.begin(), vec.end(), expected.begin(), expected.end()));
    }

    SECTION("To list") {
        auto list = splitter.transformAs<std::list>([](const auto s) { return std::string(s.data(), s.size()); });
        std::list<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(list.begin(), list.end(), expected.begin(), expected.end()));
    }
}
