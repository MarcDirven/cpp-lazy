#pragma once


#include <vector>
#include <array>
#include <map>

#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
    template<class Iterator>
    class BasicIteratorView {
    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;

        virtual Iterator begin() const = 0;
        virtual Iterator end() const = 0;

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
        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to(Args&& ... args) const {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
         * @brief Creates a new `std::vector<value_type>` of the sequence.
         * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`. is used.
         * @return A `std::vector<value_type>` with the sequence.
         */
        std::vector<value_type> toVector() const {
            return std::vector<value_type>(begin(), end());
        }

        /**
         * @brief Creates a new `std::vector<value_type, Allocator>`.
         * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
         * by this function.
         * @tparam Allocator Is automatically deduced.
         * @param alloc The allocator.
         * @return A new `std::vector<value_type, Allocator>`.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a new `std::vector<value_type, N>`.
         * @tparam N The size of the array.
         * @return A new `std::array<value_type, N>`.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }

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
            class Key = std::decay_t<decltype(std::declval<KeySelectorFunc>()(std::declval<value_type>()))>,
            class Compare = std::less<Key>,
            class Allocator = std::allocator<std::pair<const Key, value_type>>>
        std::map<Key, value_type, Compare, Allocator> toMap(KeySelectorFunc keyGen,
                                                            const Allocator& allocator = Allocator()) {
            return detail::toMap<value_type, Key, Compare>(begin(), end(), keyGen, allocator);
        }
    };
}}