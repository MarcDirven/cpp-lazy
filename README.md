[![Build status](https://github.com/MarcDirven/cpp-lazy/workflows/Continuous%20Integration/badge.svg)](https://github.com/MarcDirven/cpp-lazy/actions) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

![](https://i.ibb.co/ccn2V8N/Screenshot-2021-05-05-Make-A-High-Quality-Logo-In-Just-5-Minutes-For-Under-30-v1-cropped.png)

Examples can be found [here](https://github.com/MarcDirven/cpp-lazy/wiki/Examples). Installation can be found [here](https://github.com/MarcDirven/cpp-lazy#installation).

# cpp-lazy
Cpp-lazy is a fast and easy lazy evaluation library for C++11/14/17/20. The two main reasons this is a fast library is because the library doesn't allocate anything. Another reason the iterators are fast is because the iterators are random access where possible. This makes operations such as `std::distance` an O(1) operation, either using "overloading" the `std::distance`/`std::next` functions using ADL lookup, or just a `std::random_access_iterator_tag`. Furthermore, the view object has many `std::execution::*` overloads. This library uses one optional (!) dependency library `fmt`, which is automatically configured by CMake. If you do use CMake or do not want to use `fmt`, use `#define LZ_STANDALONE` before including. Excample:

```cpp
#define LZ_STANDALONE
#include <Lz/Map.hpp>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  std::string result = lz::map(arr, [](int i) { return i + 1; }).toString(" "); // == "2 3 4 5"
}
```

# Features
- C++11/14/17/20; C++20 concept support; C++17 `execution` support (`std::execution::par`/`std::execution::seq` etc...)
- Easy print using `std::cout << [lz::IteratorView]` or `fmt::print("{}", [lz::IteratorView])`
- Compatible with old(er) compiler versions; at least `gcc` versions => `4.8` & `clang` => `5.0.0` (previous 
versions have not been checked, so I'd say at least a compiler with C++11 support).
- Tested with `-Wpedantic -Wextra -Wall -Wno-unused-function` and `/W3` for MSVC
- One dependency ([`fmt`](https://github.com/fmtlib/fmt)) which is automatically configured, or none if you use `#define LZ_STANDALONE` before including.
- STL compatible
- Little overhead
- Supported compiler versions:
  - &gt;= GCC 4.8
  - &gt;= clang 5
  - &gt;= Visual Studio 15 2017; MSVC 19.16.27043.0
  - Previous version have not been tested but C++11 is a minimum requirement
- Easy [installation](https://github.com/MarcDirven/cpp-lazy#installation)
- Clear [Examples](https://github.com/MarcDirven/cpp-lazy/wiki/Examples)
- Readable using chaining dot (`.map([]{}()).filter([]{}()).enumerate()./* ... */`) notation

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
std::generate(randomNumbers.begin(), randomNumbers.end(), [&dist, &gen]{ return dist(gen); });
```


That certainly took alot amount of typing. Instead, try this for change:
```cpp
std::vector<int> randomNumbers = lz::random(0, 32, n).toVector();
```
> I want to search if the sequence of random numbers contain 6. 

In 'regular' C++ code that would be:
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

// or

if (lz::contains(random, 6)) {
  // do something
}
```
So by using this lazy method, we 'pretend' it's a container, while it actually is not. Therefore it does not allocate 
any memory and has very little overhead.

## Writing loops yourself
I understand where you're coming from. You may think it's more readable. But the chances of getting bugs are 
bigger because you will have to write the whole loop yourself. On average 
[about 15 – 50 errors per 1000 lines of delivered code](https://labs.sogeti.com/how-many-defects-are-too-many/) contain 
bugs. While this library does all the looping for you and is thoroughly tested using `catch2`. The `lz::random` `for`-loop 
equivalent is quite trivial to write yourself, but you may want to look at `lz::concat`.

## What about `ranges::v3`?
This library is not a replacement for `ranges::v3` but rather a (smaller) alternative. However, chances are that the 
compile time of this library is faster. Some may argue about which library is more readable. However, both libraries will have its advantages and disadvantages. The ranges v3 library is also standardized but does not support C++11.

# Important note
To garantee the best performance, some iterators have custom `next`/`distance` implementations. If you use these functions, please be sure to do it as follows:
```cpp
auto view = lz::chunks(array, 3);
// Calculate distance:
using std::distance; using lz::distance;
auto distance = distance(view.begin(), view.end());

// Get nth element:
using std::next; using lz::next;
auto nth = next(view.begin(), 4);
```

If, for some reason, you do not wish do do this, then be sure to use `lz::next/lz::distance` for the following iterators:
- `CartesianProductIterator` created by `lz::cartesian::begin`
- `Range` created by `lz::range::begin`
- `TakeEveryIterator` created by `lz::takeEvery::begin`
- `ChunksIterator` created by `lz::chunks::begin`
- `FlattenIterator` created by `lz::flatten::begin`


# Installation

## Without CMake, without `fmt`
- Clone the repository
- Specify the include directory to `cpp-lazy/include`.
- Include files as follows:

```cpp
#define LZ_STANDALONE // Important! Define it before including <Lz/*> files.
#include <Lz/Map.hpp>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  std::string result = lz::map(arr, [](int i) { return i + 1; }).toString(" "); // == "1 2 3 4"
}
```

## Without CMake, with `fmt`
- Clone the repository
- Specify the include directory to `cpp-lazy/include` and `fmt/include`.
- Define `FMT_HEADER_ONLY` before including any `lz` files.
- Include files as follows:

```cpp
#define FMT_HEADER_ONLY
#include <Lz/Map.hpp>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  std::string result = lz::map(arr, [](int i) { return i + 1; }).toString(" "); // == "2 3 4 5"
}
```

# CMake
## Using `FetchContent`
Add to your CMakeLists.txt the following:
```cmake
include(FetchContent)
FetchContent_Declare(cpp-lazy
        GIT_REPOSITORY https://github.com/MarcDirven/cpp-lazy
        GIT_TAG ... # Commit hash
        UPDATE_DISCONNECTED YES)
FetchContent_MakeAvailable(cpp-lazy)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

## Using `git clone`
Clone the repository using `git clone https://github.com/MarcDirven/cpp-lazy/` and add to `CMakeLists.txt` the following:
```cmake
add_subdirectory(cpp-lazy)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

Or add `cpp-lazy/include` to the additional include directories in e.g. Visual Studio.

## Including
```cpp
#include <Lz.hpp> // or e.g. #include <Lz/Filter.hpp>

int main() {
  // use e.g. lz::filter
}
```

# Benchmarks cpp-lazy
The time is equal to one iteration. Compiled with: winlibs-x86_64-posix-seh-gcc-10.2.1-snapshot20200912-mingw-w64-7.0.0-r1

C++11
<div style="text-align:center"><img src="https://raw.githubusercontent.com/MarcDirven/cpp-lazy/master/bench/benchmarks-iterators-C%2B%2B11.png" /></div>

C++14

<div style="text-align:center"><img src="https://raw.githubusercontent.com/MarcDirven/cpp-lazy/master/bench/benchmarks-iterators-C%2B%2B14.png" /></div>

C++17

<div style="text-align:center"><img src="https://raw.githubusercontent.com/MarcDirven/cpp-lazy/master/bench/benchmarks-iterators-C%2B%2B17.png" /></div>

C++20

<div style="text-align:center"><img src="https://raw.githubusercontent.com/MarcDirven/cpp-lazy/master/bench/benchmarks-iterators-C%2B%2B20.png" /></div>

