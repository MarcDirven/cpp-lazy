#include <Lz/regex_split.hpp>
#include <iostream>
#include <string>
#include <vector>


int main() {
    std::string input = "Hello,World!This,is,a,test";
    std::regex r(",");
    auto result = lz::regex_split(input, r);

    for (const lz::StringView word : result) {
#ifndef LZ_STANDALONE
        fmt::print("{}\n", word);
#else
        std::cout << word << std::endl;
#endif
    }
    // output: Hello World!This is a test
    return 0;
}