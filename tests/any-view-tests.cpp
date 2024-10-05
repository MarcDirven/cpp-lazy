#include "Lz/AnyView.hpp"

#include <catch2/catch.hpp>
#include <iostream>
#include <list>

std::vector<int> lst = { 1, 2, 3, 4, 5 };

lz::AnyView<int, std::random_access_iterator_tag> createBidirectionalView() {
    return lst;
}

TEST_CASE("Creating a basic any view from std::list ") {
    auto view = createBidirectionalView();
    auto begin = view.begin();
    auto anotherBegin = view.begin();
    auto end = view.end();
    CHECK(begin == anotherBegin);
    ++anotherBegin;
    CHECK(begin != anotherBegin);
    CHECK(begin < anotherBegin);
    CHECK(begin <= anotherBegin);
    CHECK(anotherBegin > begin);
    CHECK(anotherBegin >= begin);

    CHECK(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    ++end;

    CHECK(static_cast<std::size_t>(std::distance(begin, end)) == lst.size());
}