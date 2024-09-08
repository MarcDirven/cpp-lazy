#include <Lz/Exclude.hpp>
#include <fmt/format.h>

int main() {
	std::vector<int> vec = {1, 2, 3, 4, 5};
    fmt::print("{}\n", lz::exclude(vec, 2, 4)); // Exclude idx 2 to idx 4 [2, 4). Index [2, 3] is excluded
                                                // output: 1, 2, 5
}