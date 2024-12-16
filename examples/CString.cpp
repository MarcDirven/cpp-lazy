#include <Lz/CString.hpp>
#include <iostream>


int main() {
    const char* myString = "Hello, World!";

    for (const char c : lz::c_string(myString)) {
        std::cout << c;
    }
    // Output: Hello, World!
    std::cout << '\n';
    // Or::
    for (const char c : lz::c_string(myString, myString + 5)) {
        std::cout << c;
    }
    // Creating the iterator this way, it becomes a random access iterator
    // Output: Hello
}