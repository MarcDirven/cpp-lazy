#include <Lz/Generate.hpp>
#include <vector>
#include <iostream>

int main() {
    int myIncreasingCounter = 0;
    constexpr int amount = 4;
    auto generator = [&myIncreasingCounter]() {
        return myIncreasingCounter++;
    };
    const auto gen = lz::generate(generator, amount);
    std::cout << gen << '\n';
	// Output: 0 1 2 3
	for (int i : gen) {
		// Process i...
	}
}