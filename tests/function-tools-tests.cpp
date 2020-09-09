#include <Lz/FunctionTools.hpp>
#include <catch.hpp>
#include <utility>
#include <iostream>


TEST_CASE("Function tools") {
    std::vector<int> array = {1, 2, 3, 4};

    SECTION("Mean") {
        double avg = lz::mean(array);
        CHECK(avg == (1. + 2. + 3. + 4.) / 4.);
    }

    SECTION("Median") {
        double median = lz::median(array);
        CHECK(median == (2. + 3.) / 2.);
    }
}