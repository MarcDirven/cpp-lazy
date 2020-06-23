[![Build Status](https://travis-ci.com/MarcDirven/cpp-lazy.svg?branch=master)](https://travis-ci.com/MarcDirven/cpp-lazy) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# cpp-lazy
Cpp-lazy is a fast lazy evaluation library for C++14/17/20. It makes extended use of STL iterators. Current supported iterators are:
- Enumerate
- Filter
- Map
- Range
- StringSplitter
- Take/slice/take range/take while
- Zip

Every iterator (except zip) has a `"[iterator-name]range` and `[iterator-name]` equivalent. Example, the `enumerate` and the `enumeraterange` functions. The first funtion takes an iterable/container, the latter takes two iterators (begin and end).

# Benchmarks cpp-lazy
Time in nanoseconds

<div style="text-align:center"><img src="https://i.imgur.com/i8UNtz8.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/tuKCTzM.png" /></div>

<div style="text-align:center"><img src="https://i.imgur.com/NG1gV8h.png" /></div>
