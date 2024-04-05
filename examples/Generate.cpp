#include <Lz/Generate.hpp>
#include <vector>
#include <functional>
#include <iostream>

int main() {
	std::size_t counter = 0;
	static constexpr std::size_t amount = 4;
	auto gen = lz::generate(
		[](std::size_t& c) {
			auto tmp{ c++ };
			return tmp;
		},
		amount, counter);

	std::cout << gen << '\n';
	// Output: 0 1 2 3
	for (int i : gen) {
		// Process i...
	}
}
