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

#include "fmt/ostream.h"
#include "LzTools.hpp"


namespace lz { namespace internal {
    // ReSharper disable once CppUnnamedNamespaceInHeaderFile
    namespace {
        template<typename T>
        struct HasReserve {
        private:
            template<typename U, std::size_t(U::*)() const>
            struct SubstituteFailure {
            };

            template<typename U>
            static char test(SubstituteFailure<U, &U::reserve>*) {
                return 0;
            }

            template<typename U>
            static int test(...) {
                return 0;
            }
        	
        public:
            static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
        };
    }

    template<class LzIterator>
    class BasicIteratorView {
    public:
        using value_type = internal::ValueType<LzIterator>;

    private:
#if defined(LZ_GCC_VERSION) && (__GNUC__ == 4) && (__GNUC__MINOR__ < 9)
        template<class MapType, class KeySelectorFunc>
        MapType createMap(const KeySelectorFunc keyGen) const {
            MapType map;
#else // end GCC 4.[0, 9], i.e. 4.8, 4.7, 4.6... 4.0
        template<class MapType, class Allocator, class KeySelectorFunc>
        MapType createMap(const KeySelectorFunc keyGen, const Allocator& allocator) const {
            MapType map(allocator);
#endif
            std::transform(begin(), end(), std::inserter(map, map.end()), [keyGen](const value_type& value) {
                return std::make_pair(keyGen(value), value);
            });
            return map;
        }

        template<std::size_t N>
        void verifyRange() const {
            constexpr auto size = static_cast<internal::DiffType<LzIterator>>(N);

            if (std::distance(begin(), end()) > size) {
                throw std::invalid_argument(LZ_FILE_LINE ": the iterator size is too large and/or array size is too small");
            }
        }

        template<class Container>
        EnableIf<HasReserve<Container>::value, void> reserve(Container& container) const {
            container.reserve(std::distance(begin(), end()));
        }

        template<class Container>
        EnableIf<!HasReserve<Container>::value, void> reserve(Container&) const {}

#ifdef LZ_HAS_EXECUTION
        template<class Container, class... Args, class Execution>
        Container copyContainer(Execution execution, Args&& ... args) const {
            const LzIterator b = begin();
            const LzIterator e = end();
            Container cont(std::forward<Args>(args)...);
            reserve(cont);

            // Prevent static assertion
            if constexpr (internal::checkForwardAndPolicies<Execution, LzIterator>()) {
                static_cast<void>(execution);
                // If parallel execution, compilers throw an error if it's std::execution::seq. Use an output iterator to fill the contents.
                std::copy(b, e, std::inserter(cont, cont.begin()));
            }
            else {
                // use execution policies
                std::copy(execution, b, e, cont.begin());
            }

            return cont;
        }

        template<std::size_t N, class Execution>
        std::array<value_type, N> copyArray(Execution execution) const {
            verifyRange<N>();
            std::array<value_type, N> array{};

            if constexpr (internal::checkForwardAndPolicies<Execution, LzIterator>()) {
                static_cast<void>(execution);
                std::copy(begin(), end(), array.begin());
            }
            else {
                std::copy(std::forward<Execution>(execution), begin(), end(), array.begin());
            }
            return array;
        }
#else // ^^^ has execution vvv ! has execution

        template<class Container, class... Args>
        Container copyContainer(Args&& ... args) const {
            Container cont(std::forward<Args>(args)...);
            reserve(cont);
            std::copy(begin(), end(), std::inserter(cont, cont.begin()));
            return cont;
        }

        template<std::size_t N>
        std::array<value_type, N> copyArray() const {
            verifyRange<N>();
#if defined(LZ_GCC_VERSION) && (LZ_GCC_VERSION < 5)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
            std::array<value_type, N> array{};
#if defined(LZ_GCC_VERSION) && (LZ_GCC_VERSION < 5)
  #pragma GCC diagnostic pop
#endif
            std::copy(begin(), end(), array.begin());
            return array;
        }

#endif // end has execution

        template<class KeySelectorFunc>
        using KeyType = FunctionReturnType<KeySelectorFunc, value_type>;

        LzIterator _begin{};
        LzIterator _end{};

    public:
        virtual LzIterator begin() LZ_CONST_REF_QUALIFIER {
            return _begin;
        }

        virtual LzIterator end() LZ_CONST_REF_QUALIFIER {
            return _end;
        }

#ifdef LZ_HAS_REF_QUALIFIER
        virtual LzIterator begin() && {
            return std::move(_begin);
        }

        virtual LzIterator end() && {
            return std::move(_end);
        }
#endif // end lz has ref qualifier
        BasicIteratorView() = default;

        BasicIteratorView(LzIterator begin, LzIterator end):
            _begin(std::move(begin)),
            _end(std::move(end))
        {}

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
         * @param execution The execution policy. Must be one of `std::execution`'s tags.
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
        Container<value_type, Args...> to(const Execution execution = std::execution::seq, Args&& ... args) const {
            using Cont = Container<value_type, Args...>;
            return copyContainer<Cont>(execution, std::forward<Args>(args)...);
        }

        /**
        * @brief Creates a new `std::vector<value_type>` of the sequence.
        * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`. is used.
         * @param exec The execution policy. Must be one of `std::execution`'s tags.
        * @return A `std::vector<value_type>` with the sequence.
        */
        template<class Execution = std::execution::sequenced_policy>
        std::vector<value_type> toVector(const Execution exec = std::execution::seq) const {
            return to<std::vector>(exec);
        }

        /**
         * @brief Creates a new `std::vector<value_type, Allocator>`.
         * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
         * by this function.
         * @param exec The execution policy. Must be one of `std::execution`'s tags.
         * @param alloc The allocator.
         * @return A new `std::vector<value_type, Allocator>`.
         */
        template<class Allocator, class Execution = std::execution::sequenced_policy>
        std::vector<value_type, Allocator> toVector(const Execution exec = std::execution::seq,
                                                    const Allocator& alloc = Allocator()) const {
            return to<std::vector>(exec, alloc);
        }

        /**
         * @brief Creates a new `std::vector<value_type, N>`.
         * @tparam N The size of the array.
         * @param exec The execution policy. Must be one of `std::execution`'s tags.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<std::size_t N, class Execution = std::execution::sequenced_policy>
        std::array<value_type, N> toArray(const Execution exec = std::execution::seq) const {
            return copyArray<N>(exec);
        }

        /**
         * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
         * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
         * @param delimiter The delimiter between the previous value and the next.
         * @param exec The execution policy. Must be one of `std::execution`'s tags.
         * @return The converted iterator in string format.
         */
        template<class Execution = std::execution::sequenced_policy>
        std::string toString(const std::string& delimiter = "", const Execution exec = std::execution::seq) const {
            std::string string;
            auto formatFun = [delimiter, this](const value_type& v) {
                return fmt::format("{}{}", v, delimiter);
            };
            if constexpr (internal::checkForwardAndPolicies<Execution, LzIterator>()) {
                static_cast<void>(exec);
                // Prevent static assertion and/or weird errors when parallel policy is passed
                string = std::transform_reduce(begin(), end(), std::string(), std::plus<>(), formatFun);
            }
            else {
                string = std::transform_reduce(exec, begin(), end(), std::string(), std::plus<>(), formatFun);
            }

            const std::size_t delimiterLength = delimiter.length();
            if (!string.empty() && delimiterLength >= 1) {
                string.erase(string.size() - delimiterLength);
            }

            return string;
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
         * @tparam Args Additional arguments, automatically deduced.
         * @param args Additional arguments, for e.g. an allocator.
         * @return An arbitrary container specified by the entered template parameter.
         */
        template<template<class, class...> class Container, class... Args>
        Container<value_type, Args...> to(Args&& ... args) const {
            using Cont = Container<value_type, Args...>;
            return copyContainer<Cont>(std::forward<Args>(args)...);
        }

        /**
         * @brief Creates a new `std::vector<value_type>` of the sequence.
         * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`. is used.
         * @return A `std::vector<value_type>` with the sequence.
         */
        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        /**
         * @brief Creates a new `std::vector<value_type, Allocator>`.
         * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
         * by this function.
         * @param alloc The allocator.
         * @return A new `std::vector<value_type, Allocator>`.
         */
        template<class Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return to<std::vector>(alloc);
        }

        /**
         * @brief Creates a new `std::vector<value_type, N>`.
         * @tparam N The size of the array.
         * @return A new `std::array<value_type, N>`.
         * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
         */
        template<std::size_t N>
        std::array<value_type, N> toArray() const {
            return copyArray<N>();
        }

        /**
         * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
         * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
         * @param delimiter The delimiter between the previous value and the next.
         * @return The converted iterator in string format.
         */
        std::string toString(const std::string& delimiter = "") const {
            std::string string;

            for (const value_type& v : *this) {
                string += fmt::format("{}{}", v, delimiter);
            }

            const std::size_t delimiterLength = delimiter.length();
            if (!string.empty() && delimiterLength >= 1) {
                string.erase(string.size() - delimiterLength);
            }

            return string;
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
#if defined(LZ_GCC_VERSION) && LZ_GCC_VERSION < 5
        toMap(const KeySelectorFunc keyGen) const {
            using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
            return createMap<Map>(keyGen);
#else // ^^^gcc < 5 vvv gcc >= 5
        toMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const {
            using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
            return createMap<Map>(keyGen, allocator);
#endif // end lz gcc version < 5
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
#if defined(LZ_GCC_VERSION) && LZ_GCC_VERSION < 5
        toUnorderedMap(const KeySelectorFunc keyGen) const {
            using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality>;
            return createMap<UnorderedMap>(keyGen);
#else // ^^^gcc < 5 vvv gcc >= 5
        toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const {
            using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality>;
            return createMap<UnorderedMap>(keyGen, allocator);
#endif // end lz gcc version < 5
        }

        /**
         * Function to stream the iterator to an output stream e.g. `std::cout`.
         * @param o The stream object.
         * @param it The iterator to print.
         * @return The stream object by reference.
         */
        friend std::ostream& operator<<(std::ostream& o, const BasicIteratorView<LzIterator>& it) {
            return o << it.toString(" ");
        }
    };
}} // Namespace lz::internal
#endif // end LZ_BASIC_ITERATOR_VIEW_HPP
