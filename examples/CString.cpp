#include <Lz/c_string.hpp>
#include <iostream>


int main() {
    const char* my_str = "Hello, World!";

    for (const char c : lz::c_string(my_str)) {
        std::cout << c;
    }
    // Output: Hello, World!
    std::cout << '\n';
    // Or::
    for (const char c : lz::c_string(my_str, my_str + 5)) {
        std::cout << c;
    }
    // Creating the iterator this way, it becomes a random access iterator
    // Output: Hello
}