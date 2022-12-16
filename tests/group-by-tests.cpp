#include <Lz/GroupBy.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("GroupBy changing and creating elements", "[GroupBy][Basic functionality]") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };

    std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
    auto grouper = lz::groupBy(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SECTION("Should be correct chunks") {
        std::size_t stringLength = 3;

        for (auto&& g : grouper) {
            CHECK(g.first.length() == stringLength);
            for (auto& str : g.second) {
                CHECK(str.length() == stringLength);
            }
            ++stringLength;
        }
    }

    SECTION("Should be by ref") {
        auto begin = grouper.begin();
        *(begin->second.begin()) = "imm";
        CHECK(vec[0] == "imm");
    }
}

TEST_CASE("GroupBy binary operations", "[GroupBy][Binary ops]") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };

    std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
    auto grouper = lz::groupBy(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SECTION("Operator++") {
        auto it = grouper.begin();
        CHECK(it->first.length() == 3);
        CHECK(*it->second.begin() == "i'm");
        ++it;

        CHECK(it->first.length() == 4);
        CHECK(*it->second.begin() == "done");
    }

    SECTION("Operator== & operator!=") {
        auto it = grouper.begin();
        CHECK(it != grouper.end());
        it = grouper.end();
        CHECK(it == grouper.end());
    }
}