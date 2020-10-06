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

    const auto mapper = lz::map(s, [](const SomeStruct& s) { return s.s; });
    std::cout << mapper << '\n';
	// Output: Hello World

    // Yields by value if lambda does not use: "[](std::string&) -> std::string& {}":
	for (std::string i : mapper) {
		// process i...
	}
}