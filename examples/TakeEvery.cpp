#include <Lz/TakeEvery.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> sequence = {1, 2, 3, 4, 5};
    const auto takeEvery = lz::takeEvery(sequence, 2);
    std::cout << takeEvery << '\n';
	// Output: 1 3 5
	
    for (int i : takeEvery) {
		// process i...
    }
}