#include "Lz/CartesianProduct.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3};
    std::vector<char> chars = {'a', 'b', 'c'};

    auto cartesian = lz::cartesian(vec, chars);

    for (auto&& tup : cartesian) {
        auto&& first = std::get<0>(tup);
        auto&& snd = std::get<1>(tup);

        fmt::print("{} {}\n", first, snd);
    }
    // Output:
    // 1 a
    // 1 b
    // 1 c
    // 2 a
    // 2 b
    // 2 c
    // 3 a
    // 3 b
    // 3 c
}