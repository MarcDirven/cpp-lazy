#include <Lz/AnyView.hpp>
#include <Lz/Lz.hpp>
#include <iostream>

// First template parmeter is the value_type. The second the reference type.
// Because we do no return any references here, we need to specify a second template parameter: an int rvalue:
lz::AnyView<int, int> filterAndMapRValue(const std::vector<int>& vec) {
    return lz::chain(vec).filter([](int i) { return i % 2 == 0; }).map([](int i) { return i * 2; });
}

// First template parmeter is the value_type. The second the reference type.
// Because we do return a references here, we need to specify a second template parameter: an int& (lvalue):
lz::AnyView<int> filterAndMapReference(std::vector<int>& vec) {
    return lz::chain(vec).filter([](int i) { return i % 2 == 0; }).map([](int& i) -> int& { return i; });
}

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto view = filterAndMapRValue(vec);
    for (int i : view) {
        std::cout << i << '\n';
    }
    // output:
    // 4 8

    auto secondView = filterAndMapReference(vec);
    for (int& i : secondView) {
        std::cout << i << '\n';
    }
    // output:
    // 2 4
    return 0;
}