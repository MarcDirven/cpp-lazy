#include <Lz/Repeat.hpp>
#include <iostream>


int main() {
    const auto to_repeat = 155;
    const auto amount = 4;
    const auto repeater = lz::repeat(to_repeat, amount);
    std::cout << repeater << '\n';
	// Output: 155 155 155 155
    for (int i : lz::repeat(to_repeat, amount)) {
        // Process i..
    }
}