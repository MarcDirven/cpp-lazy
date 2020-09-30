[![Build Status](https://travis-ci.com/MarcDirven/cpp-lazy.svg?branch=master)](https://travis-ci.com/MarcDirven/cpp-lazy) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# cpp-lazy
Cpp-lazy is a fast and easy lazy evaluation library for C++14/17/20. The two main reasons this is a fast library is 
because the library almost doesn't allocate anything. Another reason the iterators are fast is because the iterators are
random access where possible. This makes operations such as `std::distance` an O(1) operation. Furthermore, the view 
object has many `std::execution::*` overloads. This library uses one dependency library `fmt`, which is automatically configured by CMake.

# Features
- C++14/17/20; C++20 concept support; C++17 `execution` support (`std::execution::par`/`std::execution::seq` etc...)
- Easy print using `std::cout << [lz::IteratorView]` or `fmt::print("{}", [lz::IteratorView])`
- Compatible with old(er) compiler versions; at least `gcc` versions => `5.4.0` & `clang` => `7.0.0` (previous 
versions have not been checked)
- Tested with `-Wpedantic -Wextra -Wall -Wno-unused-function` and `/W4` for MSVC
- One dependency ([`fmt`](https://github.com/fmtlib/fmt)) which is automatically configured
- STL compatible
- Little overhead

# Current supported iterators & examples
The iterators are (with some exceptions) by reference. All iterators contain a `ostream<<` operator to print all the values of the iterator. This is also compatible with 
`fmt::print` and `fmt::format`. The iterator also contains a `toString` function. Current supported iterators are:
- filterMap (defined in `FunctionTools.hpp`)
```cpp
std::string s = "123,d35dd";
auto f = lz::filterMap(s, [](const char c) { return static_cast<bool>(std::isdigit(c)); }, // if this is true
                        [](const char c) { return static_cast<int>(c - '0'); }); // return this
std::cout << f << '\n';
// yields (int) 1 2 3 3 5

for (int i : f) {
    // process i...
}
```
- **Affirm** check the whole sequence for `predicate`. If predicate returns false, the exception given as parameter is thrown:
```cpp
std::array<int, 4> array = {1, 2, 3, 4};
auto affirm = lz::affirm(array, std::logic_error("logic error"), [](const int i) {
    // If i != 3, the value is returned. If i == 3, an exception is thrown
    return i != 3;
});

auto beg = affirm.begin();
auto end = affirm.end();

// With while loop, able to continue
while (beg != end) {
    try {
        std::cout << *beg << '\n';
    }
    catch (const std::logic_error& err) {
        std::cout << err.what() << '\n';
    }
    ++beg;
}
```
- **Concatenate**, this iterator can be used to merge two or more containers together. The size of the arrays are 4 
here, but they can be all have different sizes.
```cpp
std::array<int, 4> a{1, 2, 3, 4};
std::array<int, 4> b{5, 6, 7, 8};

const auto concat = lz::concat(a, b);
std::cout << concat << '\n';
// Output: 1 2 3 4 5 6 7 8
for (int i : concat) {
    // process i...
}

std::cout << '\n';

std::array<int, 4> c{9, 10, 11, 12};
std::array<int, 4> d{13, 14, 15, 16};

const auto concat2 = lz::concat(a, b, c, d);
std::cout << concat2 << '\n';
// Output: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
for (int i : concat2) {
    // process i...
}
```
- **Enumerate**, when iterating over this iterator, it returns a `std::pair` where the `.first` is the index counter 
and the `.second` the element of the container by reference.
```cpp
std::vector<int> values = {1, 2, 3, 4, 5};
std::vector<int> toExcept = {2, 3, 4};
const auto except = lz::except(values, toExcept);
std::cout << except << '\n';
// Output: 1 5

for (int i : except) {
    // Process i...
}
```
- **Except** excepts/skips elements in container `iterable`, contained by `toExcept`, e.g. 
`lz::except({1, 2, 3}, {1, 2})` will result in `{ 3 }`.
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
const auto filter = lz::filter(toFilter, [](const int i) { return i % 2 == 0; });
std::cout << filter << '\n';
// Output: 2 4 6
for (int i : filter) {
    // Process i...
}
```
- **Generate** returns the value of a given function `amount` of times. This is essentially the same as `yield` in 
Python or `yield return` in C#.
```cpp
int myIncreasingCounter = 0;
constexpr int amount = 4;
auto generator = [&myIncreasingCounter]() {
    return myIncreasingCounter++;
};
const auto gen = lz::generate(generator, amount);
std::cout << gen << '\n';
// Output: 0 1 2 3
for (int i : gen) {
    // Process i...
}
```
- **Join** Can be used to join a container to a sequence of `std::string`. Uses `fmt` library to convert ints, floats 
etc to `std::string`. If the container type is `std::string`, then the elements are accessed by reference, otherwise 
they are accessed by value.
```cpp
const std::vector<std::string> strings = {"hello", "world"};
const auto join = lz::join(strings, ", ");
std::cout << join << '\n';
// Output: Hello, world

 // if the container type is std::string, a std::string by reference is returned
for (std::string& s : join) {
    // Process s...
}
const std::vector<int> ints = {1, 2, 3};
const auto intJoin = lz::join(ints, ", ");
std::cout << intJoin << '\n';
// Output: 1, 2, 3

// if the container type is not std::string, a std::string by value is returned
for (std::string i : intJoin) {
    // Process i...
}
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

const auto mapper = lz::map(s, [](const SomeStruct& s) { return s.s; });
std::cout << mapper << '\n';
// Output: Hello World

// Yields by value if lambda does not use: "[](std::string&) -> std::string& {}":
for (std::string i : mapper) {
    // process i...
}
```
- **Random** returns a random number `amount` of times.
```cpp
const float min = 0;
const float max = 1;
const size_t amount = 4;
const auto rng = lz::random(min, max, amount);
std::cout << rng << '\n';
// Output: random number between [0, 1] random number between [0, 1] random number between [0, 1] random number between [0, 1]

for (float i : rng) {
    // process i...
}
```
- **Range** creates a sequence of numbers e.g. `lz::range(30)` creates a range of ints from [0, 30).
```cpp
const auto toRepeat = 155;
const auto amount = 4;
const auto repeater = lz::repeat(toRepeat, amount);
std::cout << repeater << '\n';
// Output: 155 155 155 155
for (int i : lz::repeat(toRepeat, amount)) {
    // Process i..
}
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
const auto splitter = lz::split(toSplit, std::move(delim));
std::cout << splitter << '\n';
// Output: Hello world
// 
// If C++ 17 or higher, use for (std::string_view s : lz::split) else use for (std::string& substring : lz::split)
for (std::string& substring : splitter) {
    std::cout << substring << '\n';
}
```
- **Take**/**slice**/**takeRange**/**takeWhile/dropWhile** Takes a certain range of elements/slices a range of 
elements/takes/drops elements while a certain predicate function returns `true`.
```cpp
std::vector<int> seq = {1, 2, 3, 4, 5, 6};
const auto takeWhile = lz::takeWhile(seq, [](const int i) { return i != 4; });
std::cout << takeWhile << '\n';
// Output: 1 2 3

for (int i : takeWhile) {
    // process i...
}

const size_t amount = 2;
const auto take = lz::take(seq, amount);
std::cout << take << '\n';
// Output: 1 2

for (int i : take) {
    // process i...
}

const auto slice = lz::slice(seq, 1, 4);
std::cout << slice << '\n';
// Output: 2 3 4

for (int i : slice) {
    // process i...
}

constexpr std::size_t size = 4;
std::array<int, size> arr = { 1, 1, 2, 1 };
const auto dropped = lz::dropWhile(arr, [](const int i) { return i == 1; });
std::cout << dropped << '\n';
// output: 2 1

for (int i : dropped) {
    // process i...
}
```
- **TakeEvery** skips `offset` values in every iteration. E.g. `lz::takeevery({1, 2, 3, 4, 5}, 2)` will result in 
`{1, 3, 5}`.
```cpp
std::vector<int> sequence = {1, 2, 3, 4, 5};
const auto takeEvery = lz::takeEvery(sequence, 2);
std::cout << takeEvery << '\n';
// Output: 1 3 5

for (int i : takeEvery) {
    // process i...
}
```
- **Unique** can be used to only get the unique values in a sequence.
```cpp
std::vector<int> vector = {5, 3, 2, 5, 6, 42, 2, 3, 56, 3, 1, 12, 3};
// Operator== and operator< are required
const auto unique = lz::unique(vector);
std::cout << unique << '\n';
// Output: 1 2 3 4 5 6 12 42 56
for (int i : unique) {
    // process i...
}
```
- **Zip** can be used to iterate over multiple containers and stops at the shortest container length. The items 
contained by `std::tuple` (which the `operator*` returns), returns a `std::tuple` by value and its contained elements by
reference (`std::tuple<TypeA&, TypeB&[...]>`).
```cpp
std::vector<int> a = {1, 2, 3, 4};
std::vector<int> b = {1, 2, 3};

for (std::tuple<int&, int&> tup : lz::zip(a, b)) {
    std::cout << std::get<0>(tup) << ' ' << std::get<1>(tup) << '\n';
}
// Yields (by reference if '& is used):
// 1 1
// 2 2
// 3 3

// Or, C++17:
#ifdef LZ_HAS_CXX17
for (auto [a, b] : lz::zip(a, b)) {
    // process a and b...
}
#endif
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
size_t totalSize = lz::transAccumulate(s, 0U, [](size_t i, const std::string& s) {
    return i + s.size();
}); // totalSize == 11

std::string toFind = "hel";
std::string def = "default";

toFind = lz::findOrDefault(s, toFind, def);
// toFind == "default"

toFind = "hello";
toFind = lz::findOrDefault(s, toFind, def);
// toFind == "hello"

toFind = lz::findOrDefaultIf(s, [](const std::string& s) {
    return s.find('\'') != std::string::npos; // ' is present in the sequence
}, def);
// toFind == "what's"

toFind = lz::findOrDefaultIf(s, [](const std::string& s) {
    return s.find('z') != std::string::npos; // z is not present in the sequence
}, "default");
// toFind == "default"

std::vector<char> c = {'a', 'b', 'c', 'd'};
size_t pos = lz::indexOf(c, 'b');
// pos == 1

pos = lz::indexOf(c, 'e');
// pos == lz::npos

strings = {"hello", "world"};
pos = lz::indexOfIf(strings, [](const std::string& s) {
    return s.find('o') != std::string::npos;
});
// pos == 0

pos = lz::indexOfIf(strings, [](const std::string& s) {
    return s.find('q') != std::string::npos;
});
// pos == lz::npos


std::string str = "123,d35dd";
auto f = lz::filterMap(str, 
                       [](const char c) { return static_cast<bool>(std::isdigit(c)); }, // if this is true
                       [](const char c) { return static_cast<int>(c - '0'); }); // return this
// f will yield {1, 2, 3, 3, 5}


std::string myString = "picture.jpg";
lz::strReplace(myString, ".jpg", ".png");
// myString == "picture.png"

myString = "picture.png.png";
lz::strReplaceAll(myString, ".png", ".jpg");
// myString == "picture.jpg.jpg"

auto arr = { 1, 2, 3, 4 };
for (auto&& vals : lz::pairwise(arr)) {
    // printing values yields (using std::get):
    // 1 2
    // 2 3
    // 3 4
}

myString = "picture.png.png";
myString = lz::reverse(myString).toString();
// myString == gnp.gnp.erutcip
```

# To containers, easy!
Every sequence created by the `lz` library, has the following functions: `toVector`, `to`, `toArray`, `toMap` and 
`toUnorderedMap`. Examples:
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
Lazy evaluation is an evaluation strategy which holds the evaluation of an expression until its value is needed. In this
library, all the iterators are lazy evaluated. Suppose you want to have a sequence of `n` random numbers. You could 
write a for loop:

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
Both methods do not allocate anything but the second example is a much more convenient way of writing the same thing.
Now what if we wanted to do eager evaluation? Well then you could do this:

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
So by using this lazy method, we 'pretend' it's a container, while it actually is not. Therefore it does not allocate 
any memory and has very little overhead.

## But I like writing loops myself
Well, I understand where you're coming from. You may think it's more readable. But the chances of getting bugs are 
bigger because you will have to write the whole loop yourself. On average 
[about 15 – 50 errors per 1000 lines of delivered code](https://labs.sogeti.com/how-many-defects-are-too-many/) contain 
bugs. While this library does all the looping for you and is thoroughly tested using `catch2`. The `lz::random` `for`-loop 
equivalent is quite trivial to write yourself, but you may want to look at `lz::concat`.

## What about `ranges::v3`?
This library is not a replacement for `ranges::v3` but rather a (smaller) alternative. However, chances are that the 
compile time of this library is faster. Some may argue about which library is more readable. `ranges::v3` does not
support an easy printing (e.g. using `fmt`/`std` `print` and `format`, `toString()` and `operator<<` for output streams). 
However, both libraries will have its advantages and disadvantages.

# Installation
Clone the repository using `git clone --recurse-submodules https://github.com/MarcDirven/cpp-lazy` or 
`git submodule init && git submodule update` (after regular cloning) and add to `CMakeLists.txt` the following:
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

# Side note
If you want to re-overwrite or default construct the iterator/view object, use a `std::function` instead of a lambda 
function. Example:
```cpp
// OK
std::function<std::string(TestStruct)> f = [](const TestStruct& t) {
    return t.testFieldStr;
};
auto map = lz::map(array, f);
auto it = map.begin();
it = map.end();

// Not OK
auto map = lz::map(array, [](const TestStruct& t) { return t.testFieldStr; });
auto it = map.begin();
// it = map.end(); error, attempting to reference deleted function operator= for lambda

```

# Benchmarks cpp-lazy
The time is equal to one iteration.

C++14

<div style="text-align:center"><img src="https://i.imgur.com/KYONRY6.png" /></div>

C++17

<div style="text-align:center"><img src="https://i.imgur.com/STOgeK5.png" /></div>

C++20

<div style="text-align:center"><img src="https://i.imgur.com/83qAmvL.png" /></div>

