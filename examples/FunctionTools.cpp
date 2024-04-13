#include <Lz/FunctionTools.hpp>

#include <fmt/format.h>
#include <array>
#include <string>
#include <vector>

int main() {
    std::vector<int> ints = {1, 2, 3, 4};

    double avg = lz::mean(ints); // avg == (1. + 2. + 3. + 4.) / 4.)
    double median = lz::median(ints); // median == (2 + 3) / 2.)

    std::vector<std::string> strings = {"hello", "world", "what's", "up"};
    auto unlines = lz::unlines(strings).toString(); // unlines == "hello\nworld\nwhat's\nup"

    std::string string = "aa\nbb\nbb";
    auto lines = lz::lines(string).toVector(); // lines == std::vector<std::string>{"aa", "bb", "bb"}

    std::vector<std::string> frontAndBack;
    auto hasOneElement = lz::hasOne(frontAndBack);     // hasOneElement == false
    auto hasManyElements = lz::hasMany(frontAndBack);  // hasManyElements == false
    auto value = lz::frontOr(frontAndBack, "default"); // value == "default"
    value = lz::backOr(frontAndBack, "default");       // value == "default"

    frontAndBack = { "hello", "world" };
    value = lz::frontOr(frontAndBack, "default"); // value == "hello"
    value = lz::backOr(frontAndBack, "default");  // value == "world"
    hasOneElement = lz::hasOne(frontAndBack);     // hasOneElement == false, only true if the size is 1
    hasManyElements = lz::hasMany(frontAndBack);  // hasManyElements == true, true if the size is greater than 1

    std::vector<std::string> s = {"hello", "world", "!"};

    std::string toFind = "hel";
    std::string def = "default";

    toFind = lz::findFirstOrDefault(s, toFind, def); // toFind == "default"

    toFind = "hello";
    toFind = lz::findFirstOrDefault(s, toFind, def); // toFind == "hello"

    toFind = lz::findFirstOrDefaultIf(
        s,
        [](const std::string& s) {
            return s.find('\'') != std::string::npos; // ' is present in the sequence
        },
        def); // toFind == "what's"

    toFind = lz::findFirstOrDefaultIf(
        s,
        [](const std::string& s) {
            return s.find('z') != std::string::npos; // z is not present in the sequence
        },
        "default"); // toFind == "default"

    std::vector<char> c = {'a', 'b', 'c', 'd'};
    size_t pos = lz::indexOf(c, 'b'); // pos == 1
    pos = lz::indexOf(c, 'e');        // pos == lz::npos

    strings = {"hello", "world"};
    pos = lz::indexOfIf(strings, [](const std::string& s) { return s.find('o') != std::string::npos; }); // pos == 0

    pos = lz::indexOfIf(strings, [](const std::string& s) { return s.find('q') != std::string::npos; }); // pos == lz::npos

    std::string str = "123,d35dd";
    auto f = lz::filterMap(str, [](const char c) { return static_cast<bool>(std::isdigit(c)); }, // if this is true
                           [](const char c) { return static_cast<int>(c - '0'); }); // return this
    // f will yield {1, 2, 3, 3, 5}

    auto arr = { 1, 2, 3, 4 };
	for (auto&& vals : lz::pairwise(arr)) {
		// printing values yields (using std::get):
		// 1 2
		// 2 3
		// 3 4
	}

    s = {"hello world", "what's up"};
    toFind = "hel";
    def = "default";

    toFind = lz::findLastOrDefault(s, toFind, def); // toFind == "default"

    toFind = "what's up";
    toFind = lz::findLastOrDefaultIf(
        s, [](const std::string& s) { return s.find('\'') != std::string::npos; }, def); // toFind == "what's up"

    toFind = lz::findLastOrDefaultIf(
        s, [](const std::string& s) { return lz::contains(s, 'q'); }, def); // toFind == "default"

    std::map<int, int> myMap = {{1, 2}, {2, 3}, {3, 4}};
    std::vector<int> values = lz::values(myMap).toVector(); // {2, 3, 4}
    std::vector<int> keys = lz::keys(myMap).toVector(); // {1, 2, 3}

    std::string trimMe = "\t\t\n   trim me   \t\t";
    fmt::print("{}\n", lz::trimString(trimMe).toString()); // 'trim me'

    std::array<int, 10> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lz::forEachWhile(numbers, [](int& i) {
        const auto performNextIteration = i < 5;
        if (i < 5) {
            i = 0;
        }
        return performNextIteration;
    });
    fmt::print("{}\n", numbers[3]); // prints 0
    fmt::print("{}\n", numbers[4]); // prints 5

    std::vector<std::string> zipWith1 = { "hello", "world" };
    std::vector<std::string> zipWith2 = { "what's", "up" };
    auto zipedWith = lz::zipWith([](const std::string& s1, const std::string& s2) { return s1 + " " + s2; }, zipWith1,
                                 zipWith2)
                         .toVector(); // zipedWith == {"hello what's", "world up"}

    std::vector<float> asFloats = { 1.0f, 2.0f, 3.0f, 4.0f };
    std::vector<int> asInts = lz::as<int>(asFloats).toVector(); // asInts == {1, 2, 3, 4}

    std::vector<int> toReverse = { 1, 2, 3, 4 };
    std::vector<int> reversed = lz::reverse(toReverse).toVector(); // reversed == {4, 3, 2, 1}

    std::vector<int> someValues = { 1, 2, 3, 4, 5 };
    std::vector<bool> selectors = { true, false, true, false, true };
    std::vector<int> selected = lz::select(someValues, selectors).toVector(); // selected == {1, 3, 5}

    std::vector<int> startsWith = { 1, 2 };
    auto doesStartWith = lz::startsWith(someValues, startsWith); // startsWith == true
    std::vector<int> endsWith = { 4, 5 };
    auto doesEndWith = lz::endsWith(someValues, endsWith); // endsWith == true
    startsWith = { 1, 3 };
    endsWith = { 3, 5 };
    doesStartWith = lz::startsWith(someValues, startsWith); // doesStartWith == false
    doesEndWith = lz::endsWith(someValues, endsWith);       // doesEndWith == false
}