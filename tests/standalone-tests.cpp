#include <Lz/Join.hpp>
#include <Lz/Map.hpp>
#include <Lz/Splitter.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Overall tests with LZ_STANDALONE defined") {
    std::array<unsigned, 4> array = { 1, 2, 3, 4 };
    auto combined = lz::join(array, ", ").toString();

    REQUIRE(combined == "1, 2, 3, 4");
    REQUIRE(lz::map(array, [](unsigned i) { return i + 1; }).toString(" ") == "2 3 4 5");

    std::array<bool, 4> bools = { true, false, true, false };
    auto boolMap = lz::join(bools, ", ").toString();

    REQUIRE(boolMap == "true, false, true, false");

    std::string toSplit = "hello, world";
    auto splitter = lz::split(toSplit, ", ");
    REQUIRE(lz::distance(splitter.begin(), splitter.end()) == 2);
    static_assert(std::is_same<decltype(*splitter.begin()), lz::View<decltype(toSplit.begin())>>::value, "Should be lz::View");

    std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
    auto doubles = lz::str_join(vec, ", ");
#ifdef __cpp_lib_to_chars
    CHECK(doubles == "1.1, 2.2, 3.3, 4.4");
#else
    CHECK(doubles == "1.100000, 2.200000, 3.300000, 4.400000");
#endif

    char charMap[] = { 'a', 'b', 'c', 'd' };
    auto charJoined = lz::join(charMap, ", ").toString();
    REQUIRE(charJoined == "a, b, c, d");
    REQUIRE(lz::map(charMap, [](char c) { return static_cast<char>(c + 1); }).toString(" ") == "b c d e");
}