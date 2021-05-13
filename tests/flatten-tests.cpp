#include <catch2/catch.hpp>
#include <Lz/Flatten.hpp>
#include <list>
#include "Lz/FunctionTools.hpp"


TEST_CASE("Should flatten", "[Flatten][Basic functionality]") {
	SECTION("Flatten 1D") {
		std::vector<int> vec = {1,2,3,4};
		auto flattened = lz::flatten(vec);
		CHECK(flattened.toVector() == std::vector<int>{1,2,3,4});
		CHECK(lz::reverse(flattened).toVector() == std::vector<int>{4, 3, 2, 1});
	}

	SECTION("Flatten 2D") {
		std::vector<std::list<int>> nested = {{1,2,3}, {}, {1}, {4,5,6}, {}};
		auto flattened = lz::flatten(nested);
		CHECK(flattened.toVector() == std::vector<int>{1,2,3,1,4,5,6});
		CHECK(lz::reverse(flattened).toVector() == std::vector<int>{6, 5, 4, 1, 3, 2, 1});
	}

	SECTION("Flatten 3D") {
		std::vector<std::vector<std::vector<int>>> vectors = {
			{ {1, 2, 3}, {} },
			{ {4, 5}, {6}},
			{ {7}, {}}
		};

		auto flattened = lz::flatten(vectors);
		CHECK(flattened.toVector() == std::vector<int>{1,2,3,4,5,6,7});
		CHECK(lz::reverse(flattened).toVector() == std::vector<int>{7, 6, 5, 4, 3, 2, 1});
	}

	SECTION("Should be by ref") {
		std::vector<std::vector<std::vector<int>>> vectors = {
			{ {0}, {1, 2, 3}, {}, {4} },
			{{}},
			{{ 5, 6 }, {7} , {}},
			{{}},
			{{}}
		};
		auto flattened = lz::flatten(vectors);
		*flattened.begin() = -382753;
		CHECK(vectors[0][0][0] == -382753);
	}
}

TEST_CASE("Flatten binary operations", "[Flatten][Binary ops]") {
    std::vector<std::list<int>> nested = {{1,2,3}, {}, {1}, {4,5,6}, {}};
    auto flattened = lz::flatten(nested);

    SECTION("Operator++") {
        auto begin = flattened.begin();
        CHECK(*begin == 1);
        ++begin;
        CHECK(*begin == 2);
        ++begin, ++begin;
        CHECK(*begin == 1);
    }

    SECTION("Operator--") {
    	auto begin = flattened.begin();
    	++begin;
    	CHECK(*begin == 2);
    	--begin;
    	CHECK(*begin == 1);
    	CHECK(begin == flattened.begin());
    	++begin, ++begin, ++begin;
    	CHECK(*begin == 1);
    	--begin;
    	CHECK(*begin == 3);
    }

    SECTION("Operator== & operator!=") {
        auto begin = flattened.begin();
        CHECK(begin == flattened.begin());
        CHECK(begin != flattened.end());
        begin = flattened.end();
        CHECK(begin == flattened.end());
        CHECK(begin != flattened.begin());
    }

    SECTION("Lz distance") {
		std::vector<int> vec = {1,2,3,4};
		auto f1 = lz::flatten(vec);
		auto dist = lz::distance(f1.begin(), f1.end());
		CHECK(dist == 4);

		std::vector<std::list<int>> vec2 = {{1,2,3, 4}, {}, {}, {1}, {4,5,6}, {}};
		auto f2 = lz::flatten(vec2);
		dist = lz::distance(f2.begin(), f2.end());
		CHECK(dist == 8);

		std::vector<std::vector<std::vector<int>>> vec3 = {
			{ {1, 2, 3, 4}, {}, {} },
			{ {4, 5}, {6}},
			{ {7}, {}}
		};
		auto f3 = lz::flatten(vec3);
		dist = lz::distance(f3.begin(), f3.end());
		CHECK(dist == 8);
    }

    SECTION("Lz next") {
		std::vector<int> vec = {1,2,3,4};
		auto f1 = lz::flatten(vec);
		auto n = lz::next(f1.begin(), 2);
		CHECK(*n == 3);

		std::vector<std::vector<int>> vec2 = {{1,2,3, 4}, {}, {}, {500}, {4,5,6}, {}};
		auto f2 = lz::flatten(vec2);
		CHECK(*lz::next(f2.begin(), 2) == 3);
		CHECK(*lz::next(f2.begin(), 4) == 500);
		CHECK(*lz::next(f2.begin(), 7) == 6);
		CHECK(lz::next(f2.begin(), 8) == f2.end());


		std::vector<std::vector<std::vector<int>>> vec3 = {
			{ {1, 2, 3, 4}, {}, {} },
			{ {90, 5}, {6}},
			{ {7}, {}, {}}
		};
		auto f3 = lz::flatten(vec3);
		CHECK(*lz::next(f3.begin(), 3) == 4);
		CHECK(*lz::next(f3.begin(), 4) == 90);
		CHECK(*lz::next(f3.begin(), 5) == 5);
		CHECK(*lz::next(f3.begin(), 6) == 6);
		CHECK(*lz::next(f3.begin(), 7) == 7);
		CHECK(lz::next(f3.begin(), 8) == f3.end());
    }
}

TEST_CASE("Flatten to container", "[Flatten][To container]") {
    std::vector<std::vector<int>> vecs = {{1,2,3}, {4,5}, {}, {6,7}};
    auto flattened = lz::flatten(vecs);

    SECTION("To array") {
        CHECK(flattened.toArray<7>() == std::array<int, 7>{1,2,3,4,5,6,7});
    }

    SECTION("To vector") {
        CHECK(flattened.toVector() == std::vector<int>{1,2,3,4,5,6,7});
    }

    SECTION("To other container using to<>()") {
        CHECK(flattened.to<std::list>() == std::list<int>{1,2,3,4,5,6,7});
    }

    SECTION("To map") {
        std::map<int, int> actual = flattened.toMap([](const int i) {
            return i;
        });

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
            std::make_pair(4, 4),
            std::make_pair(5, 5),
            std::make_pair(6, 6),
            std::make_pair(7, 7),
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = flattened.toUnorderedMap([](const int i) {
            return i;
        });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
            std::make_pair(4, 4),
            std::make_pair(5, 5),
            std::make_pair(6, 6),
            std::make_pair(7, 7),
        };

        CHECK(expected == actual);
    }
}