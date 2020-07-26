#include <Lz/Map.hpp>
#include <vector>
#include <iostream>


struct SomeStruct {
    std::string s;
    int a{};
};


int main() {
    std::vector<SomeStruct> s = {
        SomeStruct{"Hello"},
        SomeStruct{"World"}
    };

    auto mapper = lz::map(s, [](const SomeStruct& s) { return s.s; });
    for (std::string s : mapper) {
        std::cout << s << '\n';
    }
    // Yields by value if lambda does not use: "[](std::string&) -> std::string& {}":
    // Hello
    // World
}