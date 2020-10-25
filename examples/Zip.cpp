#include <Lz/Zip.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<int> b = {1, 2, 3};
	
    for (std::tuple<int&, int&> tup : lz::zip(a, b)) {
        std::cout << std::get<0>(tup) << ' ' << std::get<1>(tup) << '\n';
    }
    // Yields (by reference if '& is used):
    // 1 1
    // 2 2
    // 3 3

	// Or, C++17:
#ifdef LZ_HAS_CXX_17
	for (auto [a, b] : lz::zip(a, b)) {
		// process a and b...
	}
#endif
}