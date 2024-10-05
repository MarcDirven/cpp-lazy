#include "Lz/AnyView.hpp"

#include <catch2/catch.hpp>
#include <iostream>
#include <list>

TEST_CASE("Creating a basic any view from std::list, random access iterator") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::AnyView<int, std::random_access_iterator_tag> view = vec;
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

    CHECK(static_cast<std::size_t>(std::distance(begin, end)) == vec.size());
    CHECK(static_cast<std::size_t>(end - begin) == vec.size());
}

TEST_CASE("Creating a basic any view from std::list, forward iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::AnyView<int, std::forward_iterator_tag> view = lst;
    auto begin = view.begin();
    auto anotherBegin = view.begin();
    auto end = view.end();
    CHECK(begin == anotherBegin);
    ++anotherBegin;
    CHECK(begin != anotherBegin);

    CHECK(*begin == 1);
    auto last = std::next(begin, 4);
    REQUIRE(*last == 5);
    ++last;

    CHECK(static_cast<std::size_t>(std::distance(begin, last)) == lst.size());
}

TEST_CASE("Creating a basic any view from std::list, bidirectional iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::AnyView<int, std::bidirectional_iterator_tag> view = lst;
    auto begin = view.begin();
    auto anotherBegin = view.begin();
    auto end = view.end();
    CHECK(begin == anotherBegin);
    ++anotherBegin;
    CHECK(begin != anotherBegin);

    CHECK(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    ++end;

    CHECK(static_cast<std::size_t>(std::distance(begin, end)) == lst.size());
}