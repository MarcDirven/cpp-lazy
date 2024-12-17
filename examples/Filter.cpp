#include <Lz/filter.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> to_filter = {1, 2, 3, 4, 5, 6};
    const auto filter = lz::filter(to_filter, [](const int i) { return i % 2 == 0; });
    std::cout << filter << '\n';
	// Output: 2 4 6
	for (int i : filter) {
		// Process i...
	}
}