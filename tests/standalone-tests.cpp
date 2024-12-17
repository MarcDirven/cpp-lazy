#include <Lz/join.hpp>
#include <Lz/map.hpp>
#include <Lz/split.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Overall tests with LZ_STANDALONE defined") {
    std::array<unsigned, 4> array = { 1, 2, 3, 4 };
    auto combined = lz::join(array, ", ").to_string();

    REQUIRE(combined == "1, 2, 3, 4");
    REQUIRE(lz::map(array, [](unsigned i) { return i + 1; }).to_string(" ") == "2 3 4 5");

    std::array<bool, 4> bools = { true, false, true, false };
    auto bool_map = lz::join(bools, ", ").to_string();

    REQUIRE(bool_map == "true, false, true, false");

    std::string to_split = "hello, world";
    auto splitter = lz::split(to_split, ", ");
    REQUIRE(lz::distance(splitter.begin(), splitter.end()) == 2);
    static_assert(std::is_same<decltype(*splitter.begin()), lz::basic_iterable<decltype(to_split.begin())>>::value,
                  "Should be lz::View");

    std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
    auto doubles = lz::str_join(vec, ", ");
#ifdef __cpp_lib_to_chars
    CHECK(doubles == "1.1, 2.2, 3.3, 4.4");
#else
    CHECK(doubles == "1.100000, 2.200000, 3.300000, 4.400000");
#endif

    char char_map[] = { 'a', 'b', 'c', 'd' };
    auto char_joined = lz::join(char_map, ", ").to_string();
    REQUIRE(char_joined == "a, b, c, d");
    REQUIRE(lz::map(char_map, [](char c) { return static_cast<char>(c + 1); }).to_string(" ") == "b c d e");
}