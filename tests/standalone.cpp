#define LZ_STANDALONE

#include <Lz/Join.hpp>
#include <Lz/Map.hpp>
#include <catch2/catch.hpp>


TEST_CASE("Overall tests with LZ_STANDALONE defined") {
	std::array<int, 4> array = {1, 2, 3, 4};
	std::string combined = lz::join(array, ", ").toString();

	REQUIRE(combined == "1, 2, 3, 4");
	REQUIRE(lz::map(array, [](int i) { return i + 1; }).toString(" ") == "2 3 4 5");

	std::array<bool, 4> bools = {true, false, true, false};
	auto boolMap = 	lz::map(bools, [](const bool b) -> std::string { return b ? "true" : "false"; }).toString(", ");
	REQUIRE(boolMap == "true, false, true, false");
}