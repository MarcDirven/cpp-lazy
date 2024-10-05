#include <Lz/AnyView.hpp>
#include <Lz/Lz.hpp>
#include <iostream>

lz::AnyView<int, std::forward_iterator_tag, const int, const int> filterAndMap(const std::vector<int>& vec) {
    return lz::chain(vec).filter([](int i) { return i % 2 == 0; }).map([](int i) { return i * 2; });
}

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto view = filterAndMap(vec);
    for (int i : view) {
        std::cout << i << '\n';
    }
    return 0;
}