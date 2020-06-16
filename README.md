# cpp-lazy
Cpp-lazy is a fast lazy evaluation library for C++14/17/20. It makes extended use of STL iterators. Current supported iterators are:
- Enumerate
- Filter
- Map
- Range
- StringSplitter
- Take/slice/take range/take while
- Zip

# Enumerate
The `Enumerate` iterator lets you iterate over a container while it also contains a current index. It has a reference to the current container element, and a value containing the index. Current supported enumerate iterators are:
- `lz::enumeraterange`
- `lz::enumerate`

## Lz::enumerate
To illustrate what it does, see the following example:
```cpp
std::array<int, 5> array{};
// 
for (std::pair<int, int> pair : lz::enumerate(array)) {
  std::cout << pair.first << ' ' << pair.second << '\n';
}
```
Output:
```
  0 0
  1 0
  2 0
  3 0
  4 0
```
The enumerate iterator `operator*` returns a `std::pair<int, T>` by value, containing an index by value and the element in the array by reference, so doing something like this:
```cpp
std::array<int, 5> array{};
// The second argument can be set to start counting from a certain index e.g. lz::enumerate(array, 5)
for (std::pair<int, int> pair : lz::enumerate(array)) {
  pair.second = pair.first;
}
```
Will alter `array`, and set each of its values to the index contained by `pair.first`. The output of the array after the for-loop will thus be: `{ 0, 1, 2, 3, 4 }`.

## Lz::enumeraterange
The function ```lz::enumeraterange``` will do exactly the same, instead, a `begin` and `end` can also be passed, i.e.:
```
std::array<int, 5> array{};
auto enumerator = lz::enumerate(array.begin(), array.end()); // Also, start index can be specified here, i.e.lz::enumerate(array.begin(), array.end(), 3)
```
