[![Build Status](https://travis-ci.com/MarcDirven/cpp-lazy.svg?branch=master)](https://travis-ci.com/MarcDirven/cpp-lazy) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# cpp-lazy
Cpp-lazy is a fast lazy evaluation library for C++14/17/20. The library is tested and compiled with the GCC flags `-Wpedantic -Wextra -Wall -Wno-unused-function`. It makes extended use of STL iterators and contains a set of iterators that allocate 0 bytes of memory on the heap (if >= C++17, else `lz::split` does allocate substrings), making it a very cheap and fast operation. Another reason the iterators are fast is because the iterators are random acces iterators where possible. This makes operations such as `std::distance` an O(1) operation. 

An example: if you want to iterate over multiple containers at the same time, you can use the `lz::zip` function to do this.
For C++17, [structured bindings](https://www.google.com/search?client=firefox-b-d&q=structured+bindings+c%2B%2B) can be used to acces the elements.
```cpp
std::vector<int> a;
std::vector<float> b;
const std::vector<double> c;

for (auto [i, f, d] : lz::zip(a, b, c)) {
  // i, f and d are accesed by reference. Assigning these values 
  // will alter the values in its corresponding container.
}
```
For C++14, this will work slightly different:
```cpp
std::vector<int> a;
std::vector<float> b;
const std::vector<double> c;

for (auto tup : lz::zip(a, b, c)) {
  std::get<0>(tup) // acces container element a (by reference)
  std::get<1>(tup) // acces container element b (by reference)
  std::get<2>(tup) // acces container element c (by reference)
}
```
All the iterator objects also contain a `toVector()` function to put the values of the containers (in this case) into a tuple of `int, float, double`. For example:
```cpp
std::vector<std::tuple<int, float, double>> v = lz::zip(a, b, c).toVector();
```


The library comes with other handy iterators aswell, for example the `lz::map` random acces iterator. This iterator selects a value from a given type, in this example we will use `Struct` as type, and select al `Struct::s` fields. Example:
```cpp
struct Struct {
  std::string data;
  int moreData;
};

std::vector<Struct> structs;
auto mapped = lz::map(structs, [](const Struct& s) { return s.data; });
for (const std::string& s : mapped) {
  // s contains Struct::s
}
```
If one would like to put the `mapped` into a vector, then this is accomplished by doing `mapped.toVector()` just like in the previous case, making the return type of the `toVector()` method a `std::vector<std::string>>`.


Another handy iterator is the `lz::split` forward iterator. This can be used to lazy-evaluate split a string on a given delimiter. For example in C++17 that will be:
```cpp
std::string toSplit = "Hello world";

for (std::string_view word : lz::split(toSplit, " ")) {
  std::cout << word << '\n';
}
// prints:
// Hello
// world
```
In C++14 however, that will be
```cpp
std::string toSplit = "Hello world";

for (std::string& word : lz::split(toSplit, " ")) {
  std::cout << word << '\n'; // one can use std::move(word) safely
}
// prints:
// Hello
// world
```
making this version a bit slower than the C++17 splitter, because this splitter needs to allocate an extra substring.


This library also contains an iterator that filters out elements contained by another container. For example:
```cpp
std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
std::array<int, 2> toExcept = {3, 5, 7, 10};

for (int i : lz::except(v, toExcept) {
  std::cout << i << '\n';
}
// prints:
// 1
// 2
// 4
// 6
// 8
// 9
```

Every iterator (except zip) has a `"[iterator-name]"range` and `[iterator-name]` equivalent. Example, the `enumerate` and the `enumeraterange` functions. The first funtion takes an iterable/container, the latter takes two iterators (begin and end).

# Current supported iterators
 Current supported iterators are:
- **Choose**, where you can iterate over a sequence and return a new type from the function entered. Example:
```cpp
std::string s = "1q9";
auto vector = lz::choose(s, [](const char s) {
    return std::make_pair(static_cast<bool>(std::isdigit(s)), static_cast<int>(s - '0'));
}).toVector();
// vector yields (int) {1, 9}
```
- **Concatenate**, this iterator can be used to merge two containers together.
- **Enumerate**, when iterating over this iterator, it returns a `std::pair` where the `.first` is the index counter and the `.second` the element of the container by reference.
- **Except** excepts/skips elements in container `iterable`, contained by `toExcept`, e.g. `lz::except({1, 2, 3}, {1, 2})` will result in `{ 3 }`.
- **Filter** filters out elements given by a function predicate
- **Generate** returns the value of a given function `amount` of times.
- **Map** selects certain values from a type given a function predicate
- **Random** returns a random number `amount` of times.
- **Range** creates a sequence of numbers e.g. `lz::range(30)` creates a range of ints from [0, 30).
- **Repeat** repeats an element `amount` of times.
- **StringSplitter** Splits a string on a given delimiter.
- **Take**/**slice**/**takerange**/**takewhile** Takes a certain range of elements/slices a range of elements/takes elements while a certain predicate function returns `true`.
- **TakeEvery** skips `offset` values in every iteration. E.g. `lz::takeevery({1, 2, 3, 4, 5}, 2)` will result in `{1, 3, 5}`.
- **Zip** can be used to iterate over multiple containers and stops at the shortest container length. The items contained by `std::tuple` (which the `operator*` returns), returns a `std::tuple` by value and its contained elements by reference (`std::tuple<TypeA&, TypeB&[...]>`).

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

<div style="text-align:center"><img src="https://i.imgur.com/O0Co84X.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/HUfmgvY.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/b6DVCuv.png" /></div>
