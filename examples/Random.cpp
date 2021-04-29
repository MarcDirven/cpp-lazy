#include <Lz/Random.hpp>
#include <vector>
#include <iostream>


int main() {
    const float min = 0;
    const float max = 1;
    const size_t amount = 4;
    const auto rng = lz::random(min, max, amount);
    std::cout << rng << '\n';
    // Output: random number between [0, 1] random number between [0, 1] random number between [0, 1] random number between [0, 1]

	for (float i : rng) {
		// process i...
	}

	// Or create your own
	static std::random_device rd;
	std::mt19937_64 gen(rd());
	std::poisson_distribution<> d(500'000);
	auto r = lz::random(d, gen, 3);

	for (int i : r) {
		// process i...
	}
}