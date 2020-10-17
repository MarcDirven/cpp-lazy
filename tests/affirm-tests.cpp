#include <catch.hpp>

#include <Lz/Affirm.hpp>
#include <cstring>


TEST_CASE("Affirm basic tests", "[Affirm][Basic functionality]") {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    constexpr const char* message = "something went wrong";
    std::function<bool(int)> f = [](const int i) {
        return i != 3;
    };
    auto affirm = lz::affirm(vec, std::logic_error(message), f);
    auto iterator = affirm.begin();

    SECTION("Should not throw") {
        CHECK_NOTHROW(*iterator);
        CHECK(*iterator == 1);
        ++iterator;
        CHECK_NOTHROW(*iterator);
        CHECK(*iterator == 2);
    }

    SECTION("Should do for loop with throwing") {
        auto beg = affirm.begin();
        auto end = affirm.end();

        while (beg != end) {
            try {
                static_cast<void>(*beg);
            }
            catch (const std::logic_error& e) {
                CHECK(std::strcmp(e.what(), message) == 0);
            }
            ++beg;
        }
        CHECK(beg == end);
    }


    SECTION("Should throw") {
        ++iterator, ++iterator;
        CHECK_THROWS(*iterator);
    }

    SECTION("Should not throw if not dereferenced") {
        CHECK_NOTHROW(++iterator);
        CHECK_NOTHROW(++iterator);
        CHECK_NOTHROW(++iterator);
        CHECK_NOTHROW(++iterator);

        CHECK_NOTHROW(*iterator);
        CHECK(*iterator == 5);
    }
}