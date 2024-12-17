#include <Lz/Map.hpp>
#include <vector>
#include <iostream>


struct some_struct {
    std::string s;
    int a{};
};


int main() {
    std::vector<some_struct> s = {
        some_struct{"Hello"},
        some_struct{"World"}
    };

    const auto mapper = lz::map(s, [](const some_struct& s) { return s.s; });
    std::cout << mapper << '\n';
	// Output: Hello World

    // Yields by value if lambda does not use: "[](std::string&) -> std::string& {}":
	for (std::string i : mapper) {
		// process i...
	}
}