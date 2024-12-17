#include <Lz/c_string.hpp>
#include <Lz/chunk_if.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/iterable.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>

// TODO: write chunk if with sentinels

TEST_CASE("chunk_if changing and creating elements", "[chunk_if][Basic functionality]") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");

    SECTION("Length should be correct") {
        CHECK(lz::distance(chunked.begin(), chunked.end()) == 4);
    }

    SECTION("Should be by reference") {
        auto it = chunked.begin();
        *it->begin() = 'w';
        CHECK(s[0] == 'w');
    }

    SECTION("Should chunk correct length") {
        auto it = chunked.begin();
        CHECK(it->to_string() == "hello world");
        ++it;
        CHECK(it->to_string() == " this is a message");
    }
}

TEST_CASE("chunk_if variations") {
    SECTION("Ending and starting with delimiter") {
        std::string s = ";hello world;; this is a message;;; testing;;";
        auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transform_as<std::vector<std::string>>([](const Range range) { return range.to_string(); });
        std::vector<std::string> expected = { "", "hello world", "", " this is a message", "", "", " testing", "", "" };
        CHECK(vec == expected);
    }

    SECTION("Ending with delimiters") {
        std::string s = "hello world; this is a message;;";
        auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transform_as<std::vector<std::string>>([](const Range range) { return range.to_string(); });
        std::vector<std::string> expected = { "hello world", " this is a message", "", "" };
        CHECK(vec == expected);
    }

    SECTION("Ending with two one delimiter") {
        std::string s = "hello world; this is a message;";
        auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transform_as<std::vector<std::string>>([](const Range range) { return range.to_string(); });
        std::vector<std::string> expected = { "hello world", " this is a message", "" };
        CHECK(vec == expected);
    }

    SECTION("No delimiters") {
        std::string s = "hello world; this is a message";
        auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transform_as<std::vector<std::string>>([](const Range range) { return range.to_string(); });
        std::vector<std::string> expected = { "hello world", " this is a message" };
        CHECK(vec == expected);
    }
}

TEST_CASE("chunk_if binary operations", "[chunk_if][Binary ops]") {
    std::string s = ";hello world;; this is a message;;; testing;;";
    auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");
    CHECK(chunked.begin()->to_string().empty());

    SECTION("Operator++") {
        CHECK(lz::distance(chunked.begin(), chunked.end()) == 9);
        auto begin = chunked.begin();
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin->to_string() == "hello world");
        ++begin;
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin->to_string() == " this is a message");
        ++begin;
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin->to_string() == " testing");
        ++begin;
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin->to_string().empty());
        ++begin;
        CHECK(begin == chunked.end());
    }

    SECTION("Operator== & operator!=") {
        auto begin = chunked.begin();
        CHECK(begin != chunked.end());
        while (begin != chunked.end()) {
            ++begin;
        }
        CHECK(begin == chunked.end());
    }

    SECTION("Operator--") {
    }
}

TEST_CASE("chunk_if to containers", "[chunk_if][To container]") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });
    using Iterator = decltype(*chunked.begin());

    SECTION("To array") {
        REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 4);
        std::array<std::string, 4> arr;
        lz::transform(chunked, arr.begin(), [](const Iterator& it) { return it.to_string(); });
        CHECK(arr ==
              decltype(arr){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }

    SECTION("To vector") {
        std::vector<std::string> vec;
        lz::transform(chunked, std::back_inserter(vec), [](const Iterator& it) { return it.to_string(); });
        CHECK(vec ==
              decltype(vec){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> list;
        lz::transform(chunked, std::inserter(list, list.begin()), [](const Iterator& it) { return it.to_string(); });
        CHECK(list ==
              decltype(list){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }
}