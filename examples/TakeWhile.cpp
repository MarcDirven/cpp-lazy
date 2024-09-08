#include <Lz/TakeWhile.hpp>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    auto taken = lz::takeWhile(vec, [](int i) { return i < 4; });
    fmt::print("{}\n", taken); // output: 1, 2, 3

    auto dropped = lz::dropWhile(vec, [](int i) { return i < 4; });
    fmt::print("{}\n", dropped); // output: 4, 5, 6
}