#include <Lz/split.hpp>
#include <iostream>

// For fmt string_view
std::ostream& operator<<(std::ostream& s, fmt::string_view v) {
	for (const char c : v) {
		s << c;
	}
	return s;
}

int main() {
    std::string to_split = "Hello world ";
    std::string delim = " ";
    // Alternative: lz::split(to_split, ' ') (using a char is faster, so use it whenever possible)
    const auto splitter = lz::split(to_split, std::move(delim));
    std::cout << splitter << '\n';
    // Output: Hello world
    for (lz::StringView substring : splitter) {
        std::cout << substring << '\n';
    }
    
}
