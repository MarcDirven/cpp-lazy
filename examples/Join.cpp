#include <Lz/Join.hpp>
#include <iostream>

int main() {
    std::vector<std::string> strings = {"hello", "world"};
    auto join = lz::join(strings, ", ");
    // if the container type is std::string, a std::string by reference is returned
    for (std::string& s : strings) {
        std::cout << s;
    }
    // prints: hello, world

    std::vector<int> ints = {1, 2, 3};
    auto intJoin = lz::join(ints, ", ");
    // if the container type is not std::string, a std::string by value is returned
    for (std::string s : intJoin) {
        std::cout << s;
    }
    // prints 1, 2, 3
}
