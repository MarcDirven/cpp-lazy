#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <vector>
#include <array>
#include <string>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <numeric>

#include "fmt/ostream.h"

#include "LzTools.hpp"


#ifdef LZ_HAS_EXECUTION
  #include <execution>
#endif

namespace lz { namespace detail {
    template<class Iterator>
    class BasicIteratorView {
        template<class MapType, class Allocator, class KeySelectorFunc>
        MapType createMap(const KeySelectorFunc keyGen, const Allocator& allocator) const& {
            MapType map(allocator);
            std::transform(begin(), end(), std::inserter(map, map.end()), [keyGen](const value_type& value) {
                return std::make_pair(keyGen(value), value);
            });
            return map;
        }

        template<class MapType, class Allocator, class KeySelectorFunc>
        MapType createMap(const KeySelectorFunc keyGen, const Allocator& allocator)&& {
            MapType map(allocator);
            std::transform(begin(), end(), std::inserter(map, map.end()),
                           [keyGen](typename std::iterator_traits<Iterator>::reference value) {
                               return std::make_pair(keyGen(value), std::move(value));
                           });
            return map;
        }

        template<size_t N>
        void verifyRange() const {
            constexpr auto size = static_cast<typename std::iterator_traits<Iterator>::difference_type>(N);

            if (std::distance(begin(), end()) > size) {
                throw std::invalid_argument(__LZ_FILE_LINE__ ": the iterator size is too large and/or array size is too small");
            }
        }

        template<class Container, class... Args, class Execution>
        Container moveContainer(const Execution execution, Args&& ... args) {
            const Iterator b = begin();
            const Iterator e = end();
            Container cont(std::forward<Args>(args)...);
            cont.resize(std::distance(b, e));

            // Prevent static assertion
            if constexpr (std::is_same_v<Execution, std::execution::sequenced_policy>) {
                std::move(b, e, cont.begin());
            }
            else {
                std::move(execution, b, e, cont.begin());
            }

            return cont;
        }

    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        template<class KeySelectorFunc>
        using KeyType = FunctionReturnType<KeySelectorFunc, value_type>;

    public:
        virtual Iterator begin() const = 0;

        virtual Iterator end() const = 0;

        virtual ~BasicIteratorView() = default;

#ifdef LZ_HAS_EXECUTION
        /**
         * @brief Returns an arbitrary container type, of which its constructor signature looks like:
         * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
         * the typedef `value_type`.
         * @details Use this function to convert the iterator to a container. Example:
         * ```cpp
         * auto list = lazyIterator.to<std::list>();
         * auto allocator = std::allocator<int>();
         * auto set = lazyIterator.to<std::set>(allocator);
         * ```
         * @tparam Container Is automatically deduced.
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
        Container<value_type, Args...> to(const Execution execution = std::execution::seq, Args&& ... args) const& {
            const Iterator b = begin();
            const Iterator e = end();
            Container<value_type, Args...> cont(std::forward<Args>(args)...);
            cont.resize(std::distance(b, e));

            // Prevent static assertion
            if constexpr (std::is_same_v<Execution, std::execution::sequenced_policy>) {
                std::copy(b, e, cont.begin());
            }
            else {
                std::copy(execution, b, e, cont.begin());
            }

            return cont;
        }

        /**
         * @brief R-value reference overload of `to`. See `to` const& overload for details. Example of moving:
         * @details Use this function to convert the iterator to a container. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::list<std::string> newVector = std::move(filter).to<std::list>();
         * // all the values in strings are empty here, where the lambda returned true
         * ```
         * @tparam Container Is automatically deduced.
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
        Container<value_type, Args...> to(const Execution execution = std::execution::seq, Args&& ... args)&& {
            return moveContainer<Container<value_type, Args...>>(execution, std::forward<Args>(args)...);
        }

        /**
        * @brief Creates a new `std::vector<value_type>` of the sequence.
        * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`. is used.
        * @return A `std::vector<value_type>` with the sequence.
        */
        template<class Execution = std::execution::sequenced_policy>
        std::vector<value_type> toVector(const Execution exec = std::execution::seq) const& {
            return to<std::vector>(exec);
        }

        /**
         * Rvalue reference overload. This will cause the original state of the iterator to be invalid. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::vector<std::string> newVector = std::move(filter).toVector();
         * // all the values in strings are empty here, where the lambda returned true. One could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @return A new vector, causing the original container to be left in an invalid state.
         */
        template<class Execution = std::execution::sequenced_policy>
        std::vector<value_type> toVector(const Execution exec = std::execution::seq)&& {
            return moveContainer<std::vector<value_type>>(exec);
        }

        /**
         * @brief Creates a new `std::vector<value_type, Allocator>`.
         * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
         * by this function.
         * @tparam Allocator Is automatically deduced.
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @param alloc The allocator.
         * @return A new `std::vector<value_type, Allocator>`.
         */
        template<class Allocator, class Execution = std::execution::sequenced_policy>
        std::vector<value_type, Allocator> toVector(const Execution exec = std::execution::seq,
                                                    const Allocator& alloc = Allocator()) const& {
            return to<std::vector, Allocator>(exec, alloc);
        }

        /**
         * Rvalue reference overload. This will cause the original state of the iterator to be invalid. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::vector<std::string> newVector = std::move(filter).toVector();
         * // all the values in strings where the lambda returns true, are empty here, one could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @tparam Allocator Is automatically deduced.
         * @param alloc The allocator
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @return A new vector, causing the original container to be left in an invalid state.
         */
        template<class Allocator, class Execution = std::execution::sequenced_policy>
        std::vector<value_type, Allocator> toVector(const Execution exec = std::execution::seq, const Allocator& alloc = Allocator())&& {
            return moveContainer<std::vector<value_type, Allocator>>(exec, alloc);
        }

        /**
         * @brief Creates a new `std::vector<value_type, N>`.
         * @tparam N The size of the array.
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<size_t N, class Execution = std::execution::sequenced_policy>
        std::array<value_type, N> toArray(const Execution exec = std::execution::seq) const& {
            verifyRange<N>();
            std::array<value_type, N> container{};

            if constexpr (std::is_same_v<Execution, std::execution::sequenced_policy>) {
                std::copy(begin(), end(), container.begin());
            }
            else {
                std::copy(exec, begin(), end(), container.begin());
            }

            return container;
        }

        /**
         * Rvalue reference overload. Causes the original iterator to be left in an invalid state. Example:
         * ```cpp
         * std::array<std::string, 2> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::array<std::string, 2> newArray = std::move(filter).toArray<1>();
         * // all the values in strings where the lambda returns true, are empty here, one could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @tparam N The size of the array.
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<size_t N, class Execution = std::execution::sequenced_policy>
        std::array<value_type, N> toArray(const Execution exec = std::execution::seq)&& {
            verifyRange<N>();
            std::array<value_type, N> container{};

            if constexpr (std::is_same_v<Execution, std::execution::sequenced_policy>) {
                std::move(begin(), end(), container.begin());
            }
            else {
                std::move(exec, begin(), end(), container.begin());
            }

            return container;
        }
#else
        /**
         * @brief Returns an arbitrary container type, of which its constructor signature looks like:
         * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
         * the typedef `value_type`.
         * @details Use this function to convert the iterator to a container. Example:
         * ```cpp
         * auto list = lazyIterator.to<std::list>();
         * auto allocator = std::allocator<int>();
         * auto set = lazyIterator.to<std::set>(allocator);
         * ```
         * @tparam Container Is automatically deduced.
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args>
        Container<value_type, Args...> to(std::execution::seq, Args&& ... args) const& {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
         * @brief R-value reference overload of `to`. See `to` const& overload for details. Example of moving:
         * @details Use this function to convert the iterator to a container. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::list<std::string> newVector = std::move(filter).to<std::list>();
         * // all the values in strings are empty here, where the lambda returned true
         * ```
         * @tparam Container Is automatically deduced.
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
        Container<value_type, Args...> to(const Execution execution = std::execution::seq, Args&& ... args)&& {
            return moveContentsOfContainer<Container<value_type, Args...>>(std::forward<Args>(args)...);
        }

        /**
         * @brief Creates a new `std::vector<value_type>` of the sequence.
         * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`. is used.
         * @return A `std::vector<value_type>` with the sequence.
         */
        std::vector<value_type> toVector() const& {
            return to<std::vector>();
        }

        /**
         * Rvalue reference overload. This will cause the original state of the iterator to be invalid. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::vector<std::string> newVector = std::move(filter).toVector();
         * // all the values in strings are empty here, where the lambda returned true. One could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @return A new vector, causing the original container to be left in an invalid state.
         */
        std::vector<value_type> toVector()&& {
            return moveContentsOfContainer<std::vector<value_type>>();
        }

        /**
         * @brief Creates a new `std::vector<value_type, Allocator>`.
         * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
         * by this function.
         * @tparam Allocator Is automatically deduced.
         * @param alloc The allocator.
         * @return A new `std::vector<value_type, Allocator>`.
         */
        template<class Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const& {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * Rvalue reference overload. This will cause the original state of the iterator to be invalid. Example:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::vector<std::string> newVector = std::move(filter).toVector();
         * // all the values in strings where the lambda returns true, are empty here, one could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @tparam Allocator Is automatically deduced.
         * @param alloc The allocator
         * @return A new vector, causing the original container to be left in an invalid state.
         */
        template<class Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator())&& {
            return moveContentsOfContainer<std::vector<value_type, Allocator>>(alloc);
        }

        /**
         * @brief Creates a new `std::vector<value_type, N>`.
         * @tparam N The size of the array.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const& {
            verifyRange<N>();
            std::array<value_type, N> container{};
            std::copy(begin(), end(), container.begin());
            return container;
        }

        /**
         * Rvalue reference overload. Causes the original iterator to be left in an invalid state. Example:
         * ```cpp
         * std::array<std::string, 2> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::array<std::string, 2> newArray = std::move(filter).toArray<1>();
         * // all the values in strings where the lambda returns true, are empty here, one could also do:
         * strings = std::move(filter).toVector();
         * ```
         * @tparam N The size of the array.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<size_t N>
        std::array<value_type, N> toArray()&& {
            verifyRange<N>();
            std::array<value_type, N> container{};
            std::move(begin(), end(), container.begin());
            return container;
        }
#endif

        /**
         * @brief Creates a new `std::map<Key, value_type[, Compare[, Allocator]]>`.
         * @details Creates a new `std::map<Key, value_type[, Compare[, Allocator]]>`. Example:
         * ```cpp
         * std::vector<std::string> sequence = { "abc", "def", "ghi" };
         * auto someLazyViewIterator = lz::SomeLazyViewIterator(sequence); // value_type = std::string
         * std::map<char, std::string> map = someLazyViewIterator.toMap([](const std::string& s) {
         *      return s[0]; // Return the dict key, first char of the string
         * });
         * // map yields:
         * // 'a' : "abc"
         * // 'd' : "def"
         * // 'g' : "ghi"
         * ```
         * @tparam KeySelectorFunc Is automatically deduced.
         * @tparam Key Is automatically deduced.
         * @tparam Compare Can be used for the STL `std::map` ordering, default is `std::less<Key>`.
         * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
         * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
         * @param allocator Optional, can be used for using a custom allocator.
         * @return A `std::map<Key, value_type[, Compare[, Allocator]]>`
         */
        template<class KeySelectorFunc,
            class Compare = std::less<KeyType<KeySelectorFunc>>,
            class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
        std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>
        toMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const& {
            using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
            return createMap<Map>(keyGen, allocator);
        }

        /**
         * @brief Rvalue reference overload. See `toMap` const l-value reference overload for details. Example of moving:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::map<char, std::string> map = std::move(filter).toMap([](const std::string&s) {return s[0];});
         * // all the values in strings where the lambda returns true, are empty here
         * ```
         * @tparam KeySelectorFunc Is automatically deduced.
         * @tparam Key Is automatically deduced.
         * @tparam Compare Can be used for the STL `std::map` ordering, default is `std::less<Key>`.
         * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
         * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
         * @param allocator Optional, can be used for using a custom allocator.
         * @return A `std::map<Key, value_type[, Compare[, Allocator]]>`
         */
        template<class KeySelectorFunc,
            class Compare = std::less<KeyType<KeySelectorFunc>>,
            class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
        std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>
        toMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator())&& {
            using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
            return std::move(*this).template createMap<Map>(keyGen, allocator);
        }

        /**
         * @brief Creates a new `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`.
         * @details Creates a new `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`. Example:
         * ```cpp
         * std::vector<std::string> sequence = { "abc", "def", "ghi" };
         * auto someLazyViewIterator = lz::SomeLazyViewIterator(sequence); // value_type = std::string
         * std::unordered_map<char, std::string> uMap = someLazyViewIterator.toMap([](const std::string& s) {
         *      return s[0]; // Return the dict key, first char of the string
         * });
         * // uMap yields:
         * // 'a' : "abc"
         * // 'd' : "def"
         * // 'g' : "ghi"
         * ```
         * @tparam KeySelectorFunc Is automatically deduced.
         * @tparam Key Is automatically deduced.
         * @tparam Hasher The hash function, `std::hash<Key>` is used by default
         * @tparam KeyEquality Key equality checker. `std::equal_to<Key>` is used by default.
         * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
         * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
         * @param allocator Optional, can be used for using a custom allocator.
         * @return A `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`
         */
        template<class KeySelectorFunc,
            class Hasher = std::hash<KeyType<KeySelectorFunc>>,
            class KeyEquality = std::equal_to<KeyType<KeySelectorFunc>>,
            class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
        std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>
        toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const& {
            using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality>;
            return createMap<UnorderedMap>(keyGen, allocator);
        }

        /**
         * @brief Rvalue reference overload. See `toUnorderedMap` const l-value reference overload for details. Example of moving:
         * ```cpp
         * std::vector<std::string> strings = {"hello", "world" };
         * auto filter = lz::filter(v, [](const std::string& s) { return s == "hello"; });
         * std::unordered_map<char, std::string> map = std::move(filter).toUnorderedMap([](const std::string&s) {return s[0];});
         * // all the values in strings where the lambda returns true, are empty here
         * ```
         * @tparam KeySelectorFunc Is automatically deduced.
         * @tparam Key Is automatically deduced.
         * @tparam Compare Can be used for the STL `std::map` ordering, default is `std::less<Key>`.
         * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
         * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
         * @param allocator Optional, can be used for using a custom allocator.
         * @return A `std::map<Key, value_type[, Compare[, Allocator]]>`
         */
        template<class KeySelectorFunc,
            class Hasher = std::hash<KeyType<KeySelectorFunc>>,
            class KeyEquality = std::equal_to<KeyType<KeySelectorFunc>>,
            class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
        std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>
        toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator())&& {
            using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality>;
            return std::move(*this).template createMap<UnorderedMap>(keyGen, allocator);
        }

        /**
         * Function to stream the iterator to an output stream e.g. `std::cout`.
         * @param o The stream object.
         * @param it The iterator to print.
         * @return The stream object by reference.
         */
        friend std::ostream& operator<<(std::ostream& o, const BasicIteratorView<Iterator>& it) {
            return o << it.toString(" ");
        }

        /**
         * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
         * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
         * @param delimiter The delimiter between the previous value and the next.
         * @return The converted iterator in string format.
         */
        std::string toString(const char* delimiter = "") const {
            std::string string;
            for (const value_type& v : *this) {
                string += fmt::format("{}{}", v, delimiter);
            }

            const size_t delimiterLength = std::strlen(delimiter);
            if (!string.empty() && delimiterLength >= 1) {
                string.erase(string.size() - delimiterLength);
            }

            return string;
        }
    };
}}
#endif