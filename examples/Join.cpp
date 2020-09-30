#include <Lz/Join.hpp>
#include <iostream>

int main() {
    const std::vector<std::string> strings = {"hello", "world"};
    const auto join = lz::join(strings, ", ");
    std::cout << join << '\n';
	// Output: Hello, world

     // if the container type is std::string, a std::string by reference is returned
	for (const std::string& s : join) {
		// Process s...
	}
    const std::vector<int> ints = {1, 2, 3};
    const auto intJoin = lz::join(ints, ", ");
    std::cout << intJoin << '\n';
    // Output: 1, 2, 3
    
    // if the container type is not std::string, a std::string by value is returned
	for (std::string i : intJoin) {
		// Process i...
	}
}
