#define CATCH_CONFIG_MAIN

#define LZ_STANDALONE

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
    REQUIRE(std::is_same<decltype(*splitter.begin()), lz::CString<char, false>>::value);
#endif

    std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
    auto doubles = lz::strJoin(vec, ", ");
#ifdef __cpp_lib_to_chars
    CHECK(doubles == "1.1, 2.2, 3.3, 4.4");
#else
    CHECK(doubles == "1.100000, 2.200000, 3.300000, 4.400000");
#endif
}