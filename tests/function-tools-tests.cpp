#include <Lz/FunctionTools.hpp>
#include <Lz/Range.hpp>
#include <catch.hpp>


TEST_CASE("Function tools") {
    std::vector<int> ints = {1, 2, 3, 4};
    std::vector<double> doubles = {1.2, 2.5, 3.3, 4.5};

    SECTION("Mean") {
        double avg = lz::mean(ints);
        CHECK(avg == Approx((1. + 2. + 3. + 4.) / 4.));
    }

    SECTION("Median") {
        double median = lz::median(std::vector<int>(ints));
        CHECK(median == Approx((2 + 3) / 2.));

        median = lz::median(doubles);
        CHECK(median == Approx((2.5 + 3.3) / 2.));

        doubles.push_back(3.3);
        median = lz::median(doubles);
        CHECK(median == Approx(3.3));
    }

    SECTION("Unlines") {
        std::vector<std::string> strings = {"hello", "world", "what's", "up"};
        auto unlines = lz::unlines(strings).toString();
        CHECK(unlines == "hello\nworld\nwhat's\nup");
    }

    SECTION("Lines") {
        std::string string = "aa\nbb\nbb";
        auto lines = lz::lines(string).toVector();
        CHECK(lines == std::vector<std::string>{"aa", "bb", "bb"});
    }

    SECTION("Transform accumulate") {
        std::vector<std::string> s = {"hello", "world", "!"};
        size_t totalSize = lz::transaccumulate(s, 0U, [](size_t i, const std::string& s) {
           return i + s.size();
        });
        CHECK(totalSize == 11);
    }
}