[![Build Status](https://travis-ci.com/MarcDirven/cpp-lazy.svg?branch=master)](https://travis-ci.com/MarcDirven/cpp-lazy) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# cpp-lazy
Cpp-lazy is a fast and easy lazy evaluation library for C++14/17/20. It makes extended use of STL iterators and contains iterators that allocate 0 bytes of memory on the heap (if >= C++17, else `lz::split` does allocate substrings), making it a very cheap and fast operation. Another reason the iterators are fast is because the iterators are random acces where possible. This makes operations such as `std::distance` an O(1) operation.

# Current supported iterators & examples
 Current supported iterators are:
- **Choose**, where you can iterate over a sequence and return a new type (or the same type) from the function entered. Example:
```cpp
std::string s = "1q9";
auto vector = lz::choose(s, [](const char s) {
    return std::make_pair(static_cast<bool>(std::isdigit(s)), static_cast<int>(s - '0'));
}).toVector();
// vector yields (int) {1, 9}
```
- **Concatenate**, this iterator can be used to merge two or more containers together. The size of the arrays are 4 here, but they can be all have different sizes.
```cpp
std::array<int, 4> a{1, 2, 3, 4};
std::array<int, 4> b{5, 6, 7, 8};

for (int i : lz::concat(a, b)) {
    std::cout << i << '\n';
}

// Output:
// 1
// 2
// 3
// 4
// 5
// 6
// 7
// 8

std::cout << '\n';

std::array<int, 4> c{9, 10, 11, 12};
std::array<int, 4> d{13, 14, 15, 16};

for (int i : lz::concat(a, b, c, d)) {
    std::cout << i << '\n';
}

// Output:
// 1
// 2
// 3
// 4
// 5
// 6
// 7
// 8
// 9
// 10
// 11
// 12
// 13
// 14
// 15
// 16
```
- **Enumerate**, when iterating over this iterator, it returns a `std::pair` where the `.first` is the index counter and the `.second` the element of the container by reference.
```cpp
std::vector<int> toEnumerate = {1, 2, 3, 4, 5};

for (std::pair<int, int> pair : lz::enumerate(toEnumerate)) {
    std::cout << pair.first << ' ' << pair.second << '\n';
}
// yields:
// [(index element)by value] [(container element) by reference (if '&' is used)]
//          0                               1
//          1                               2
//          2                               3
//          3                               4
//          4                               5
```
- **Except** excepts/skips elements in container `iterable`, contained by `toExcept`, e.g. `lz::except({1, 2, 3}, {1, 2})` will result in `{ 3 }`.
```cpp
std::vector<int> values = {1, 2, 3, 4, 5};
std::vector<int> toExcept = {2, 3, 4};

for (int i : lz::except(values, toExcept)) {
    std::cout << i << '\n';
}
// yields (container element by reference if '&' is used):
// 1
// 5
```
- **Filter** filters out elements given by a function predicate
```cpp
std::vector<int> toFilter = {1, 2, 3, 4, 5, 6};
auto filter = lz::filter(toFilter, [](const int i) { return i % 2 == 0; });

for (int i : filter) {
    std::cout << i << '\n';
}
// yields (container element by reference if '&' is used):
// 2
// 4
// 6
```
- **Generate** returns the value of a given function `amount` of times.
```cpp
int myIncreasingCounter = 0;
constexpr int amount = 4;
auto generator = [&myIncreasingCounter]() {
    return myIncreasingCounter++;
};

for (int incrementer : lz::generate(generator, amount)) {
    std::cout << incrementer << '\n';
}
// yields (by value):
// 0
// 1
// 2
// 3
```
- **Map** selects certain values from a type given a function predicate
```cpp
struct SomeStruct {
    std::string s;
    int a{};
};
std::vector<SomeStruct> s = {
    SomeStruct{"Hello"},
    SomeStruct{"World"}
};

auto mapper = lz::map(s, [](const SomeStruct& s) { return s.s; });
for (std::string s : mapper) {
    std::cout << s << '\n';
}
// Yields by value if lambda does not use: "[](std::string&) -> std::string& {}":
// Hello
// World
```
- **Random** returns a random number `amount` of times.
```cpp
float min = 0;
float max = 1;
size_t amount = 4;
auto rng = lz::random(min, max, amount);

for (float f : rng) {
    std::cout << f << '\n';
}
// Yields (by value):
// random number between [0, 1]
// random number between [0, 1]
// random number between [0, 1]
// random number between [0, 1]
```
- **Range** creates a sequence of numbers e.g. `lz::range(30)` creates a range of ints from [0, 30).
```cpp
for (int i : lz::range(3)) {
    std::cout << i << '\n';
}
// Yields: (by value)
// 0
// 1
// 2
```
- **Repeat** repeats an element `amount` of times.
```cpp
auto toRepeat = 155;
auto amount = 4;

for (int i : lz::repeat(toRepeat, amount)) {
    std::cout << i << '\n';
}
// Yields (by reference if '&' is used):
// 155
// 155
// 155
// 155
```
- **StringSplitter** Splits a string on a given delimiter.
```cpp
std::string toSplit = "Hello world ";
std::string delim = " ";

// If C++ 17 or higher, use for (std::string_view s : lz::split) else use for (std::string& substring : lz::split)
for (std::string& substring : lz::split(toSplit, std::move(delim))) {
    std::cout << substring << '\n';
}
// Yields (by value if C++17 or higher, by ref if C++14):
// Hello
// world
```
- **Take**/**slice**/**takerange**/**takewhile** Takes a certain range of elements/slices a range of elements/takes elements while a certain predicate function returns `true`.
```cpp
std::vector<int> seq = {1, 2, 3, 4, 5, 6};
auto takeWhile = lz::takewhile(seq, [](const int i) { return i != 4; });
for (int i : takeWhile) {
    std::cout << i << '\n';
}
// Yields (by reference if '& is used):
// 1
// 2
// 3
std::cout << '\n';

size_t amount = 2;
auto take = lz::take(seq, amount);
for (int i : take) {
    std::cout << i << '\n';
}
// Yields (by reference if '& is used):
// 1
// 2
std::cout << '\n';

auto slice = lz::slice(seq, 1, 4);
for (int i : slice) {
    std::cout << i << '\n';
}
// Yields (by reference if '& is used):
// 2
// 3
// 4
```
- **TakeEvery** skips `offset` values in every iteration. E.g. `lz::takeevery({1, 2, 3, 4, 5}, 2)` will result in `{1, 3, 5}`.
```cpp
std::vector<int> sequence = {1, 2, 3, 4, 5};

for (int i : lz::takeevery(sequence, 2)) {
    std::cout << i << '\n';
}
// Yields (by reference if '& is used):
// 1
// 3
// 5
```
- **Zip** can be used to iterate over multiple containers and stops at the shortest container length. The items contained by `std::tuple` (which the `operator*` returns), returns a `std::tuple` by value and its contained elements by reference (`std::tuple<TypeA&, TypeB&[...]>`).
```cpp
std::vector<int> a = {1, 2, 3, 4};
std::vector<int> b = {1, 2, 3};

for (std::tuple<int&, int&> tup : lz::zip(a, b)) {
    std::cout << std::get<0>(tup) << ' ' << std::get<1>(tup) << '\n';
}
// or... if C++17 or higher:
for (auto [first, second] : lz::zip(a, b)) {
    std::cout << first << ' ' << second << '\n';
}
// Yields (by reference if '& is used):
// 1 1
// 2 2
// 3 3
```

# To containers, easy!
Every sequence created by the `lz` library, has the following functions: `toVector`, `to`, `toArray`, `toMap` and `toUnorderedMap`. Examples:
```cpp
char c = 'a';
auto generator = lz::generate([&c]() {
    return c++;
}, 4);

// To vector:
auto vec = generator.toVector();
for (char c : vec) {
    std::cout << c << '\n';
}
// Yields:
// a
// b
// c
// d
std::cout << '\n';
c = 'a';

// To set
auto set = generator.to<std::set>();
for (char c : set) {
    std::cout << c << '\n';
}
// Yields:
// a
// b
// c
// d
std::cout << '\n';
c = 'a';

// To list
auto list = generator.to<std::list>();
for (char c : set) {
    std::cout << c << '\n';
}
// Yields:
// a
// b
// c
// d
std::cout << '\n';
c = 'a';

// To map
std::map<char, char> map = generator.toMap([](const char c) { return static_cast<char>(c + 1); });
for (std::pair<char, char> pair : map) {
    std::cout << pair.first << ' ' << pair.second << '\n';
}
// Yields:
// b a
// c b
// d c
// e d
```


# Installation
Clone the repository and add to `CMakeLists.txt` the following:
```cmake
add_subdirectory(cpp-lazy)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy)
```
```cpp
#include <Lz.hpp> // or e.g. #include <Lz/Filter.hpp>

int main() {
  // use e.g. lz::filter
}
```
Or add `cpp-lazy/include` to the additional include directories in e.g. Visual Studio.

# Benchmarks cpp-lazy
These benchmarks include the creation of the view object + 1 iteration. All the container sizes were 32, except the iterable to except in `lz::except` function. The 'main' container had a size of 32, the container to except had a size of 16.

<div style="text-align:center"><img src="https://i.imgur.com/duTMqRz.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/cj3wcWq.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/QXTMkcf.png" /></div>


# Small side note...
If using a iterator view object that requests a function, and you would like to re-overwrite the same variable, a `std::function` is required, instead of an inline lambda. Example:
```cpp
// Wrong:
auto someView = lz::view(..., [](const int i) { return i; });
// Error, no operator= for lambda
// someView = lz::view(..., [](const int i) { return i; });

// Ok:
auto someView = lz::view(..., std::function<int(int)>([](const int i) { return i; }));
someView = lz::view(..., std::function<int(int)>([](const int i) { return i; }));

// Or ofcourse...
std::function<int(int)> func = [](const int i) { return i; };
auto anotherView = lz::view(..., func);
anotherView = lz::view(..., func);

```
