#include "Lz/Lz.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>


template class lz::chain_iterable<decltype(std::declval<std::vector<int>&>().begin())>;

TEST_CASE("Iterator chaining") {
}
