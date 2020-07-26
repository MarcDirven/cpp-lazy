#include <Lz/StringSplitter.hpp>
#include <vector>
#include <iostream>


int main() {
    std::string toSplit = "Hello world ";
    std::string delim = " ";

    // If C++ 17 or higher, use for (std::string_view s : lz::split) else use for (std::string& substring : lz::split)
    for (std::string& substring : lz::split(toSplit, std::move(delim))) {
        std::cout << substring << '\n';
    }
    // Yields (by value if C++17 or higher, by ref if C++14):
    // Hello
    // world
}