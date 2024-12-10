#include "Lz/Lz.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>


template class lz::IterView<decltype(std::declval<std::vector<int>&>().begin())>;

TEST_CASE("Iterator chaining") {
}
