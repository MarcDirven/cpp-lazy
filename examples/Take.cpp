#include <Lz/Take.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> seq = {1, 2, 3, 4, 5, 6};
    const auto takeWhile = lz::takeWhile(seq, [](const int i) { return i != 4; });
    std::cout << takeWhile << '\n';
    // Output: 1 2 3

    for (int i : takeWhile) {
	    // process i...
    }
	
    const size_t amount = 2;
    const auto take = lz::view(seq.begin() + amount, seq.end());
    std::cout << take << '\n';
    // Output: 1 2

	for (int i : take) {
		// process i...
	}
	
    const auto slice = lz::view(seq.begin() + 1, seq.end() + 4);
    std::cout << slice << '\n';
    // Output: 2 3 4

	for (int i : slice) {
		// process i...
	}

    constexpr std::size_t size = 4;
    std::array<int, size> arr = { 1, 1, 2, 1 };
    const auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });
    std::cout << dropped << '\n';
    // output: 2 1

	for (int i : dropped) {
		// process i...
	}
}