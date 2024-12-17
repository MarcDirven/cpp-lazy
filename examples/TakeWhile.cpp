#include <Lz/take_while.hpp>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    auto taken = lz::take_while(vec, [](int i) { return i < 4; });
    fmt::print("{}\n", taken); // output: 1, 2, 3

    auto dropped = lz::drop_while(vec, [](int i) { return i < 4; });
    fmt::print("{}\n", dropped); // output: 4, 5, 6
}