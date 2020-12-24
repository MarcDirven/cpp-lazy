#include "Lz/Chunks.hpp"

int main() {
	std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
	auto chunks = lz::chunks(v, 3);

	for (auto&& chunk : chunks) {
		fmt::print("This chunk has length {}\n", std::distance(chunk.begin(), chunk.end()));
		for (const int i : chunk) {
			fmt::print("{}\n", i);
		}
	}
}