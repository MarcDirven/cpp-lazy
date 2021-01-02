#include "Lz/ChunkIf.hpp"


int main() {
	std::string s = "hello world; this is a message;";
	auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });

	for (auto&& chunk : chunked) {
		for (const std::string& substring : chunk) {
			fmt::print("sub string = {}\n", chunked);
		}
	}
	// Output
	// sub string = 'hello world'
	// sub string = ' this is a message'
}