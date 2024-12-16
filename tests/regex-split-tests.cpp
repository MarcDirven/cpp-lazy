#include <Lz/regex_split.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>


TEST_CASE("regex_split_iterable changing and creating elements", "[regex_split_iterable][Basic functionality]") {
    std::regex r1(R"(\s+)");

    SECTION("Empty string") {
        std::string s = "";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = {};
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Starting with delimiter") {
        std::string s = "    Hello, world! How are you?";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Ending with delimiter") {
        std::string s = "Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    SECTION("Starting and ending with delimiter") {
        std::string s = "    Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter.transformAs<std::vector<std::string>>(
            [](const lz::StringView sv) { return std::string(sv.data(), sv.size()); });
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }
}

TEST_CASE("regex_split_iterable binary operations", "[regex_split_iterable][Binary ops]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regex_split(s, r1);
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

TEST_CASE("regex_split_iterable to containers", "[regex_split_iterable][To container]") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regex_split(s, r1);

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
