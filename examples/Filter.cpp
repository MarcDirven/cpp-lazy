#include <Lz/Filter.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> toFilter = {1, 2, 3, 4, 5, 6};
    const auto filter = lz::filter(toFilter, [](const int i) { return i % 2 == 0; });
    std::cout << filter << '\n';
	// Output: 2 4 6
	for (int i : filter) {
		// Process i...
	}
}