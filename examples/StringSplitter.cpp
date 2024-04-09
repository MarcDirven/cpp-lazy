#include <Lz/StringSplitter.hpp>
#include <iostream>

// For fmt string_view
std::ostream& operator<<(std::ostream& s, fmt::string_view v) {
	for (const char c : v) {
		s << c;
	}
	return s;
}

int main() {
    std::string toSplit = "Hello world ";
    std::string delim = " ";
    // Alternative: lz::split(toSplit, ' ') (using a char is faster, so use it whenever possible)
    const auto splitter = lz::split(toSplit, std::move(delim));
    std::cout << splitter << '\n';
    // Output: Hello world
    // If C++ 17 or higher, use for (std::string_view s : lz::split) else use for (fmt::string_view substring : lz::split)
    // If LZ_STANDALONE is defined and C++ version is less than 17, then lz::StringView is returned
    for (fmt::string_view substring : splitter) {
        std::cout << substring << '\n';
    }
    
}
