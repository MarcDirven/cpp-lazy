#include <Lz/Flatten.hpp>
#include <list>

int main() {
    std::vector<std::vector<std::vector<int>>> vectors = {
        { { 1,2, 3}, {}, {4} },
        {},
        {{ 5, 6 }, {7} , {}}
    };
    auto flattened = lz::flatten(vectors);
    fmt::print("{}\n", flattened);

    std::vector<std::list<int>> nested = {{1,2,3}, {}, {1}, {4,5,6}, {}};
    auto flattened2 = lz::flatten(nested);
    fmt::print("{}\n", flattened2);
}