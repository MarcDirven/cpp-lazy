#include <Lz/GenerateWhile.hpp>

int main() {
    auto generator = lz::generateWhile(
        [](int& i) {
            auto copy = i++;
            // If `copy` == 4, stop generating values.
            // This function can return any value that is compatible with std::get
            // Where std::get<0> must be a type that is convertible to bool
            // Where std::get<1> can be any type
            return std::make_pair(copy == 4, copy);
        },
        // 0 = the parameter for the lambda
        0);

    auto vec = generator.toVector();
    // vec = { 0, 1, 2, 3 }
}