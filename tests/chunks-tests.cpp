#include "Lz/Chunks.hpp"
#include "Lz/FunctionTools.hpp"
#include "catch2/catch.hpp"
#include <list>


TEST_CASE("Chunks changing and creating elements", "[Chunks][Basic functionality]") {
	std::vector<int> v = {1, 2, 3, 4, 5, 6, 7};
	auto chunked = lz::chunks(v, 3);

	SECTION("Should be by ref") {
		auto it = chunked.begin();
		*it->begin() = 0;
		CHECK(v[0] == 0);
	}

	SECTION("Should be correct length") {
		CHECK(std::distance(chunked.begin(), chunked.end()) == 3);
	}
}

TEST_CASE("Chunks binary operations", "[Chunks][Binary ops]") {
	std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
	auto chunked = lz::chunks(v, 3);
	auto it = chunked.begin();

	SECTION("Operator++") {
		CHECK(*it->begin() == 1);
		++it;
		CHECK(*it->begin() == 4);
		++it;
		CHECK(*it->begin() == 7);
	}

	SECTION("Operator== & operator!=") {
		CHECK(it == chunked.begin());
		CHECK(it != chunked.end());
		it = chunked.end();
		CHECK(it != chunked.begin());
		CHECK(it == chunked.end());
	}

	SECTION("Lz distance") {
		auto c = lz::chunks(v, 3);
		CHECK(lz::distance(c.begin(), c.end()) == 3);
		c = lz::chunks(v, 4);
		CHECK(lz::distance(c.begin(), c.end()) == 2);
		c = lz::chunks(v, 8);
		CHECK(lz::distance(c.begin(), c.end()) == 1);
	}

	SECTION("Lz next") {
		auto c = lz::chunks(v, 3);
		CHECK(*lz::next(c.begin(), 0)->begin() == 1);
		CHECK(*lz::next(c.begin(), 1)->begin() == 4);
		CHECK(*lz::next(c.begin(), 2)->begin() == 7);
		CHECK(lz::next(c.begin(), 3) == c.end());

		c = lz::chunks(v, 4);
		CHECK(*lz::next(c.begin(), 0)->begin() == 1);
		CHECK(*lz::next(c.begin(), 1)->begin() == 5);
		CHECK(lz::next(c.begin(), 2) == c.end());

		c = lz::chunks(v, 8);
		CHECK(*lz::next(c.begin(), 0)->begin() == 1);
		CHECK(lz::next(c.begin(), 1) == c.end());
	}
}

TEST_CASE("Chunks to containers", "[Chunk][To container]") {
	std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
	auto chunked = lz::chunks(v, 3);

	SECTION("To array") {
		std::array<std::array<int, 3>, 3> arrays{};
		std::size_t i = 0;
		for (auto&& chunk : chunked) {
			std::copy(chunk.begin(), chunk.end(), arrays[i++].begin());
		}

		std::array<std::array<int, 3>, 3> expected = {
			std::array<int, 3>{1, 2, 3},
			std::array<int, 3>{4, 5, 6},
			std::array<int, 3>{7, 8}
		};

		CHECK(arrays == expected);
	}

	SECTION("To vector") {
		std::vector<std::vector<int>> vectors{};
		for (auto&& chunk : chunked) {
			vectors.emplace_back(chunk.toVector());
		}

		std::vector<std::vector<int>> expected = {
			std::vector<int>{1, 2, 3},
			std::vector<int>{4, 5, 6},
			std::vector<int>{7, 8}
		};

		CHECK(vectors == expected);
	}

	SECTION("To other container using to<>()") {
		std::list<std::list<int>> lists{};
		for (auto&& chunk : chunked) {
			lists.emplace_back(chunk.to<std::list>());
		}

		std::list<std::list<int>> expected = {
			std::list<int>{1, 2, 3},
			std::list<int>{4, 5, 6},
			std::list<int>{7, 8}
		};

		CHECK(lists == expected);
	}
}