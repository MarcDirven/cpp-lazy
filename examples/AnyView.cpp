#include <Lz/AnyView.hpp>
#include <Lz/Lz.hpp>
#include <iostream>

// First template parmeter is the value_type. The second the reference type.
// Because we do no return any references here, we need to specify a second template parameter: an int rvalue:
lz::any_iterable<int, int> filter_map_r_value(const std::vector<int>& vec) {
    return lz::chain(vec).filter([](int i) { return i % 2 == 0; }).map([](int i) { return i * 2; });
}

// First template parmeter is the value_type. The second the reference type.
// Because we do return a references here, we need to specify a second template parameter: an int& (lvalue):
lz::any_iterable<int> filter_map_reference(std::vector<int>& vec) {
    return lz::chain(vec).filter([](int i) { return i % 2 == 0; }).map([](int& i) -> int& { return i; });
}

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto view = filter_map_r_value(vec);
    for (int i : view) {
        std::cout << i << '\n';
    }
    // output:
    // 4 8

    auto second_view = filter_map_reference(vec);
    for (int& i : second_view) {
        std::cout << i << '\n';
    }
    // output:
    // 2 4
    return 0;
}