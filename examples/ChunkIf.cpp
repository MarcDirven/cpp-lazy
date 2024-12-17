#include "Lz/ChunkIf.hpp"


int main() {
	std::string s = "hello world; this is a message;";
	auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });

	for (auto&& chunk : chunked) {
		for (const char c : chunk) {
			fmt::print("{}", c);
		}
		fmt::print("\n");
	}
	// Output
	// hello world
	//  this is a message
}