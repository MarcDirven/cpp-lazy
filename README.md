[![Build Status](https://travis-ci.com/MarcDirven/cpp-lazy.svg?branch=master)](https://travis-ci.com/MarcDirven/cpp-lazy) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# cpp-lazy
Cpp-lazy is a fast and easy lazy evaluation library for C++14/17/20. The two main reasons this is a fast library is because the library almost doesn't allocate anything. Another reason the iterators are fast is because the iterators are random access where possible. This makes operations such as `std::distance` an O(1) operation. It uses one dependency library `fmt`, which is automatically configured by CMake.

# Features
- C++14/17/20
- Compatible with old(er) compiler versions
- Tested with `-Wpedantic -Wextra -Wall -Wno-unused-function` and `/W4` for MSVC
- One dependency ([`fmt`](https://github.com/fmtlib/fmt)) which is automatically configured
- STL compatible

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
- **Generate** returns the value of a given function `amount` of times. This is essentially the same as `yield` in Python or `yield return` in C#.
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
- **Join** Can be used to join a container to a sequence of `std::string`. Uses `fmt` library to convert ints, floats etc to `std::string`. If the container type is `std::string`, then the elements are accessed by reference, otherwise they are accessed by value.
```cpp
std::vector<std::string> strings = {"hello", "world"};
auto join = lz::join(strings, ", ");
// if the container type is std::string, a std::string by reference is returned
for (std::string& s : strings) {
    std::cout << s;
}
// prints: hello, world

std::vector<int> ints = {1, 2, 3};
auto intJoin = lz::join(ints, ", ");
// if the container type is not std::string, a std::string by value is returned
for (std::string s : intJoin) {
    std::cout << s;
}
// prints 1, 2, 3
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
- **Unique** can be used to only get the unique values in a sequence.
```cpp
std::vector<int> vector = {5, 3, 2, 5, 6, 42, 2, 3, 56, 3, 1, 12, 3};
// Operator== and operator< are required
auto unique = lz::unique(vector);
 
for (int i : vector) {
    std::cout << i << '\n';
}
// prints
// 1
// 2
// 3
// 4
// 5
// 6
// 12
// 42
// 56
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

# Function tools
```cpp
std::vector<int> ints = {1, 2, 3, 4};

double avg = lz::mean(ints); // avg == (1. + 2. + 3. + 4.) / 4.)
double median = lz::median(ints); // median == (2 + 3) / 2.)

std::vector<std::string> strings = {"hello", "world", "what's", "up"};
auto unlines = lz::unlines(strings).toString(); // unlines == "hello\nworld\nwhat's\nup"

std::string string = "aa\nbb\nbb";
auto lines = lz::lines(string).toVector(); // lines == std::vector<std::string>{"aa", "bb", "bb"}

std::vector<std::string> s = {"hello", "world", "!"};
size_t totalSize = lz::transaccumulate(s, 0, [](const std::string& s) {
    return s.size();
}, std::plus<>()); // totalSize == 11
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

# What is lazy and why would I use it?
Lazy evaluation is an evaluation strategy which holds the evaluation of an expression until its value is needed. In this library, all the iterators are lazy evaluated. Suppose you want to have a sequence of `n` random numbers. You could write a for loop:

```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution dist(0, 32);

for (int i = 0; i < n; i++) {
 std::cout << dist(gen); // prints a random number n times, between [0, 32]
}
```

This is actually exactly the same as:
```cpp
for (int i : lz::random(0, 32, n)) {
 std::cout << i;  // prints a random number n times, between [0, 32]
}
```
Both methods do not allocate anything but the second example is a much more convenient way of writing the same thing. Now what if we wanted to do eager evaluation? Well then you could do this:

```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution dist(0, 32);
std::vector<int> randomNumbers;
std::generate(randomNumbers.begin(), randomNumbers.end(), []{ return dist(gen); });
```

Well, that certainly took alot amount of typing. Instead, try this for change:
```cpp
std::vector<int> randomNumbers = lz::random(0, 32 n).toVector();
```
But wait... I want to search if the sequence of random numbers contain 6! Well, in 'regular' C++ code that would be:
```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution dist(0, 32);

for (int i = 0; i < n; i++) {
 if (gen(dist)) == 6) {
  // do something
 }
}
```
In C++ using this library and because all iterators in this library are STL compatible, we could simply use `std::find`:
```cpp
auto random = lz::random(0, 32, n);
if (std::find(random.begin(), random.end(), 6) != random.end()) {
 // do something
}
```
So by using this lazy method, we 'pretend' it's a container, while it actually is not. Therefore it does not allocate any memory and has very little overhead.

## But I like writing loops myself
Well, I understand where you're comming from. You may think it's more readable. But the chances of getting bugs are bigger because you will have to write the whole loop yourself. On average [about 15 – 50 errors per 1000 lines of delivered code](https://labs.sogeti.com/how-many-defects-are-too-many/) contain bugs. While this library does all the looping for you and is thoroughly tested using `catch2`. The `lz::random` `for`-loop equivalent is quite trivial to write yourself, but you may want to look at `lz::concat`.

## What about `ranges::v3`?
This library is not a replacement for `ranges::v3` but rather a (smaller) alternative. However, chances are that the compile time of this library is faster. Some may argue about which library is more readable. `ranges::v3` does not support an easy printing (e.g. using `fmt`/`std` `print` and `format`, `toString()` and `operator<<` for output streams). However, both libraries will have its advantages and disadvantages.

# Installation
Clone the repository using `git clone --recurse-submodules https://github.com/MarcDirven/cpp-lazy` or `git submodule init && git submodule update` (after regular cloning) and add to `CMakeLists.txt` the following:
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
The time is equal to one iteration.

C++14

<div style="text-align:center"><img src="https://i.imgur.com/BbiaiFY.png" /></div>

C++17

<div style="text-align:center"><img src="https://i.imgur.com/BQEjTxI.png" /></div>

C++20

<div style="text-align:center"><img src="https://i.imgur.com/HzzrPgG.png" /></div>


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
