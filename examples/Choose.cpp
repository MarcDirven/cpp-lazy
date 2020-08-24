#include <Lz/Choose.hpp>
#include <iostream>


int main() {
    std::string s = "12de352";
    auto chooser = lz::choose(s, [](const char c) {
        // If c is a digit (pair.first), return the int equivalent (pair.second)
        return std::make_pair(static_cast<bool>(std::isdigit(c)), static_cast<int>(c - '0'));
    });
    for (int i : chooser) {
        std::cout << i << '\n';
    }
    // chooser yields:
    // 1
    // 2
    // 3
    // 5
    // 2
}