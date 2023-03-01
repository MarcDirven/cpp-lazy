#include <Lz/GenerateWhile.hpp>

int main() {
    auto generator = lz::generateWhile(
        [](int& i) {
            auto copy = i++;
            // If `copy` == 4, stop generating values.
            // This function can must return a pair like object (e.g. std::pair) where pair::first must
            // be a type that is convertible to bool and where pair::second can be any type
            return std::make_pair(copy == 4, copy);
        },
        // 0 = the parameter for the lambda
        0);

    auto vec = generator.toVector();
    // vec = { 0, 1, 2, 3 }
}
