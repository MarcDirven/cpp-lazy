#include <Lz/Repeat.hpp>
#include <iostream>


int main() {
    const auto toRepeat = 155;
    const auto amount = 4;
    const auto repeater = lz::repeat(toRepeat, amount);
    std::cout << repeater << '\n';
	// Output: 155 155 155 155
    for (int i : lz::repeat(toRepeat, amount)) {
        // Process i..
    }
}