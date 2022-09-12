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
    // prints 1 2 3 4 5 6 7
    // i.e. also the following can be done
    for (int i : flattened) {
        // something with i
    }

    std::vector<std::list<int>> nested = {{1,2,3}, {}, {1}, {4,5,6}, {}};
    auto flattened2 = lz::flatten(nested);
    fmt::print("{}\n", flattened2);
    // prints 1 2 3 1 4 5 6
    // i.e. also the following can be done
    for (int i : flattened) {
        // something with i
    }
}