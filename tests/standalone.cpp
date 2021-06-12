#define LZ_STANDALONE

#include <Lz/FunctionTools.hpp>
#include <Lz/Join.hpp>
#include <Lz/Map.hpp>
#include <Lz/StringSplitter.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Overall tests with LZ_STANDALONE defined") {
    std::array<int, 4> array = { 1, 2, 3, 4 };
    std::string combined = lz::join(array, ", ").toString();

    REQUIRE(combined == "1, 2, 3, 4");
    REQUIRE(lz::map(array, [](int i) { return i + 1; }).toString(" ") == "2 3 4 5");

    std::array<bool, 4> bools = { true, false, true, false };
    auto boolMap = lz::join(bools, ", ").toString();

    REQUIRE(boolMap == "true, false, true, false");

    std::string toSplit = "hello, world";
    auto splitter = lz::split(toSplit, ", ");
    REQUIRE(std::distance(splitter.begin(), splitter.end()) == 2);
#ifdef LZ_HAS_CXX_17
    REQUIRE(std::is_same_v<decltype(*splitter.begin()), std::string_view>);
#else
    REQUIRE(std::is_same<decltype(*splitter.begin()), std::string>::value);
#endif

    std::string lines = "hello\nworld\n";
    auto unlined = lz::lines(lines);
    REQUIRE(lz::lines(lines).toVector() == std::vector<std::string>{ "hello", "world" });

    std::vector<std::string> toLines = { "hello", "world" };
    auto lined = lz::unlines(toLines);
    REQUIRE(lz::unlines(toLines).toString() == std::string("hello\nworld"));
}