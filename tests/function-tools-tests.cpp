#include "Lz/FunctionTools.hpp"
#include "Lz/Range.hpp"


#include "catch.hpp"
#include <cctype>

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

	SECTION("Sum to") {
        CHECK(lz::sumTo(4) == 10);
        CHECK(lz::sumTo(5, 10) == 45);
        CHECK(lz::sumTo(4, 10) == 49);
        
        CHECK(lz::sumTo(-4) == -10);
        CHECK(lz::sumTo(-4, 3) == -4);
        CHECK(lz::sumTo(-4, -10) == -49);
    	
        CHECK(lz::sumTo(-10, -20) == -165);
    }

    SECTION("Unlines") {
        std::vector<std::string> strings = {"hello", "world", "what's", "up"};
        auto unlines = lz::unlines(strings).toString();
        CHECK(unlines == "hello\nworld\nwhat's\nup");
    }

    SECTION("Lines") {
        std::string string = "aa\nbb\nbb";
        auto lines = lz::lines<std::string>(string).toVector();
        CHECK(lines == std::vector<std::string>{"aa", "bb", "bb"});
    }

#ifndef LZ_HAS_CXX_17
    SECTION("Transform accumulate") {
        std::vector<std::string> s = {"hello", "world", "!"};
        size_t totalSize = lz::transAccumulate(s, static_cast<std::size_t>(0), [](std::size_t i, const std::string& s) {
            return i + s.size();
        });
        CHECK(totalSize == 11);
    }
#endif // end lz has cxx 17

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
        std::size_t pos = lz::indexOf(c, 'b');
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

    SECTION("FilterMap") {
        std::string s = "123,d35dd";
        auto f = lz::filterMap(s, [](const char c) { return static_cast<bool>(std::isdigit(c)); },
                               [](const char c) { return static_cast<int>(c - '0'); });
        auto expected = { 1, 2, 3, 3, 5 };
        CHECK(std::equal(f.begin(), f.end(), expected.begin()));
    }

    SECTION("To string func") {
        std::vector<int> v = {1, 2, 3, 4, 5};
        auto dummy = lz::map(v, [](int i) { return i; });

        CHECK(dummy.toString() == "12345");
        CHECK(dummy.toString(" ") == "1 2 3 4 5");
        CHECK(dummy.toString(", ") == "1, 2, 3, 4, 5");
    }

    SECTION("String replace") {
        std::string myString = "picture.jpg";
        lz::strReplace(myString, ".jpg", ".jpeg");
        CHECK(myString == "picture.jpeg");
        CHECK(myString.length() == std::strlen("picture.jpeg"));

        myString = "picture.png.png";
        lz::strReplaceAll(myString, ".png", ".jpeg");
        CHECK(myString == "picture.jpeg.jpeg");
        CHECK(myString.length() == std::strlen("picture.jpeg.jpeg"));
    }

	SECTION("Reverse") {
        std::string s = "hello";
        CHECK(lz::reverse(s).toString() == "olleh");
    }

	SECTION("Transpose") {
        std::vector<std::vector<int>> vecs = {
        	{1, 2, 3, 4}, {5, 6, 7, 8}
        };

        CHECK(lz::transposeToVector(vecs) == std::vector<std::array<int, 2>>{ { 1, 5 }, { 2, 6 }, { 3, 7 }, { 4, 8 }});
    }
}