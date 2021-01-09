#include <Lz/StringSplitter.hpp>
#include <iostream>


int main() {
    std::string toSplit = "Hello world ";
    std::string delim = " ";
    const auto splitter = lz::split(toSplit, std::move(delim));
    std::cout << splitter << '\n';
    // Output: Hello world
    // 
    // If C++ 17 or higher, use for (std::string_view s : lz::split) else use for (std::string& substring : lz::split)
    for (fmt::string_view substring : splitter) {
        std::cout << substring << '\n';
    }
    
}