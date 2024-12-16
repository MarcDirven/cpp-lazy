#include <Lz/CString.hpp>
#include <Lz/ChunkIf.hpp>
#include <Lz/IterTools.hpp>
#include <Lz/View.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>

// TODO: write chunk if with sentinels

TEST_CASE("ChunkIf changing and creating elements", "[ChunkIf][Basic functionality]") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
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
        CHECK(it->toString() == "hello world");
        ++it;
        CHECK(it->toString() == " this is a message");
    }
}

TEST_CASE("ChunkIf variations") {
    SECTION("Ending and starting with delimiter") {
        std::string s = ";hello world;; this is a message;;; testing;;";
        auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transformAs<std::vector<std::string>>([](const Range range) { return range.toString(); });
        std::vector<std::string> expected = { "", "hello world", "", " this is a message", "", "", " testing", "", "" };
        CHECK(vec == expected);
    }

    SECTION("Ending with delimiters") {
        std::string s = "hello world; this is a message;;";
        auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transformAs<std::vector<std::string>>([](const Range range) { return range.toString(); });
        std::vector<std::string> expected = { "hello world", " this is a message", "", "" };
        CHECK(vec == expected);
    }

    SECTION("Ending with two one delimiter") {
        std::string s = "hello world; this is a message;";
        auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transformAs<std::vector<std::string>>([](const Range range) { return range.toString(); });
        std::vector<std::string> expected = { "hello world", " this is a message", "" };
        CHECK(vec == expected);
    }

    SECTION("No delimiters") {
        std::string s = "hello world; this is a message";
        auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
        using Range = typename decltype(chunked.begin())::value_type;
        auto vec = chunked.transformAs<std::vector<std::string>>([](const Range range) { return range.toString(); });
        std::vector<std::string> expected = { "hello world", " this is a message" };
        CHECK(vec == expected);
    }
}

TEST_CASE("ChunkIf binary operations", "[ChunkIf][Binary ops]") {
    std::string s = ";hello world;; this is a message;;; testing;;";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");
    CHECK(chunked.begin()->toString().empty());

    SECTION("Operator++") {
        CHECK(lz::distance(chunked.begin(), chunked.end()) == 9);
        auto begin = chunked.begin();
        CHECK(begin->toString().empty());
        ++begin;
        CHECK(begin->toString() == "hello world");
        ++begin;
        CHECK(begin->toString().empty());
        ++begin;
        CHECK(begin->toString() == " this is a message");
        ++begin;
        CHECK(begin->toString().empty());
        ++begin;
        CHECK(begin->toString().empty());
        ++begin;
        CHECK(begin->toString() == " testing");
        ++begin;
        CHECK(begin->toString().empty());
        ++begin;
        CHECK(begin->toString().empty());
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

TEST_CASE("ChunkIf to containers", "[ChunkIf][To container]") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
    using Iterator = decltype(*chunked.begin());

    SECTION("To array") {
        REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 4);
        std::array<std::string, 4> arr;
        lz::transform(chunked, arr.begin(), [](const Iterator& it) { return it.toString(); });
        CHECK(arr ==
              decltype(arr){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }

    SECTION("To vector") {
        std::vector<std::string> vec;
        lz::transform(chunked, std::back_inserter(vec), [](const Iterator& it) { return it.toString(); });
        CHECK(vec ==
              decltype(vec){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> list;
        lz::transform(chunked, std::inserter(list, list.begin()), [](const Iterator& it) { return it.toString(); });
        CHECK(list ==
              decltype(list){ std::string{ "hello world" }, std::string{ " this is a message" }, std::string{}, std::string{} });
    }
}