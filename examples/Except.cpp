#include <Lz/Except.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::vector<int> toExcept = {2, 3, 4};

    std::sort(toExcept.begin(), toExcept.end()); // Always sort before creating except object
    const auto except = lz::except(values, toExcept);
    std::cout << except << '\n';
	// Output: 1 5

	for (int i : except) {
		// Process i...
	}
}