#include <iostream>
#include <lz/RegexSplit.hpp>
#include <string>
#include <vector>

int main() {
    std::string input = "Hello,World!This,is,a,test";
    std::regex r(",");
    auto result = lz::regexSplit(input, r);

    for (const lz::StringView word : result) {
        std::cout << word << std::endl;
    }
    // output: Hello World!This is a test
    return 0;
}