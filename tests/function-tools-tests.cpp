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
        double median = lz::median(doubles);
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
        size_t totalSize = lz::transAccumulate(s, static_cast<size_t>(0), [](size_t i, const std::string& s) {
            return i + s.size();
        });
        CHECK(totalSize == 11);
    }

    SECTION("Pairwise") {
        auto x = lz::pairwise(ints).toVector();
        CHECK(x == std::vector<std::tuple<int, int>>{std::make_tuple(1, 2), std::make_tuple(2, 3), std::make_tuple(3, 4)});
    }

    SECTION("As") {
        auto floats = lz::as<float>(ints).toVector();
        CHECK(std::is_same<typename decltype(floats)::value_type, float>::value);
        CHECK(floats == std::vector<float>{1., 2., 3., 4.});
    }


    SECTION("Find or default") {
        std::vector<std::string> s = {"hello world!", "what's up"};
        std::string toFind = "hel";
        std::string def = "default";

        toFind = lz::findOrDefault(s, toFind, def);
        CHECK(toFind == "default");

        def = "default";
        toFind = "hello world!";
        toFind = lz::findOrDefault(s, toFind, def);
        CHECK(toFind == "hello world!");

        def = ' ';
        toFind = lz::findOrDefaultIf(s, [](const std::string& s) {
            return s.find('!') != std::string::npos;
        }, def);
        CHECK(toFind == "hello world!");
        CHECK(!def.empty());

        toFind = lz::findOrDefaultIf(s, [](const std::string& s) {
            return s.find('z') != std::string::npos;
        }, "default");
        CHECK(toFind == "default");
    }

    SECTION("Position") {
        std::vector<char> c = {'a', 'b', 'c', 'd'};
        size_t pos = lz::indexOf(c, 'b');
        CHECK(pos == 1);

        pos = lz::indexOf(c, 'e');
        CHECK(pos == lz::npos);

        std::vector<std::string> strings = {"hello", "world"};
        pos = lz::indexOfIf(strings, [](const std::string& s) {
            return s.find('o') != std::string::npos;
        });
        CHECK(pos == 0);

        pos = lz::indexOfIf(strings, [](const std::string& s) {
            return s.find('q') != std::string::npos;
        });
        CHECK(pos == lz::npos);
    }

    SECTION("Filter map") {
        std::string s = "123swd355";
        std::function<bool(char)> f = [](char c) { return static_cast<bool>(std::isdigit(c)); };
        auto mf = lz::filterMap(s,f, [](const char c) { return static_cast<int>(c - '0'); });
        CHECK(mf.toVector() == std::vector<int>{1, 2, 3, 3, 5, 5});
    }
}