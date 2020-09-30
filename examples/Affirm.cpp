#include <Lz/Affirm.hpp>
#include <iostream>


int main() {
    std::array<int, 4> array = {1, 2, 3, 4};

    auto affirm = lz::affirm(array, static_cast<const char*>("something went wrong"), [](const int i) {
        // If i == 4, the value is returned. If i == 4, an exception is thrown
        return i != 4;
    });

    // With for loop (not able to continue execution)
    try {
        for (int i : affirm) {
            std::cout << i << '\n';
        }
    } catch (const char* msg) {
        std::cout << msg << '\n';
    }
    // Output:
    // 1
    // 2
    // 3
    // something went wrong

    // Or with an exception
    auto affirm2 = lz::affirm(array, std::logic_error("logic error"), [](const int i) {
        // If i != 3, the value is returned. If i == 3, an exception is thrown
        return i != 3;
    });

    auto beg = affirm2.begin();
    auto end = affirm2.end();

    // With while loop, able to continue
    while (beg != end) {
        try {
            std::cout << *beg << '\n';
        }
        catch (const std::logic_error& err) {
            std::cout << err.what() << '\n';
        }
        ++beg;
    }
    // Output:
    // 1
    // 2
    // logic error
    // 4
}