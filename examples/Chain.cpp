#include <Lz/Lz.hpp>

int main() {
    int arr[]{ 3, 2, 4, 5 };
    int arr2[]{ 3, 2, 5, 5 };

    // clang-format off
    const auto sequence = lz::chain(arr)
        .map([](int i) { return i + 1; })
        .as<float>()
        .concat(lz::as<float>(arr2));
    // clang-format on

    fmt::print("{}\n", sequence);
    // [ 4, 3, 5, 6, 3, 2, 5, 5 ] type = float
    fmt::print("Type is float? {}\n", std::is_same<decltype(sequence.begin()[0]), float>::value); // prints true
}