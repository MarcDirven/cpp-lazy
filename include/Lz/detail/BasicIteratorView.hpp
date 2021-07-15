#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <algorithm>
#include <array>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(LZ_STANDALONE)
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>
#endif // LZ_HAS_FORMAT
#else
#include <fmt/ostream.h>
#endif // LZ_STANDALONE

#include "LzTools.hpp"

namespace lz {
namespace internal {
#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
template<class Iterator>
void
#if defined(LZ_HAS_STRING_VIEW)
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const std::string_view delimiter,
                        std::true_type /* isArithmetic */) {
#else
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const std::string& delimiter,
                        std::true_type /* isArithmetic */) {
#endif // defined(LZ_HAS_STRING_VIEW)
    std::for_each(begin, end, [&delimiter, &result](const ValueType<Iterator>& vt) {
        using std::to_string;
        using lz::internal::to_string;
        result += to_string(vt);
        result += delimiter;
    });
}

template<class Iterator>
void
#if defined(LZ_HAS_STRING_VIEW)
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const std::string_view delimiter,
                        std::false_type /* isArithmetic */) {
#else
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const std::string& delimiter,
                        std::false_type /* isArithmetic */) {
#endif // defined(LZ_HAS_STRING_VIEW)
    std::ostringstream oss;
    std::for_each(begin, end, [&oss, &delimiter](const ValueType<Iterator>& t) { oss << t << delimiter; });
    result = oss.str();
}
#endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)

template<class Iterator>
#ifdef LZ_HAS_STRING_VIEW
LZ_CONSTEXPR_CXX_20 void
toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const std::string_view delimiter) {
#else
void toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const std::string& delimiter) {
#endif
    using TValueType = ValueType<Iterator>;
    if (begin == end) {
        return;
    }
#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
    auto backInserter = std::back_inserter(result);
#endif // !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
#if !defined(LZ_STANDALONE)
    std::for_each(begin, end,
                  [&delimiter, backInserter](const TValueType& v) { fmt::format_to(backInserter, "{}{}", v, delimiter); });
#elif defined(LZ_HAS_FORMAT)
    std::for_each(begin, end,
                  [&delimiter, backInserter](const TValueType& v) { std::format_to(backInserter, "{}{}", v, delimiter); });
    // clang-format off
#else
    toStringImplSpecialized(result, begin, end, delimiter, std::is_arithmetic<TValueType>());
#endif // LZ_STANDALONE
    // clang-format on
    const auto resultEnd = result.end();
    result.erase(resultEnd - static_cast<std::ptrdiff_t>(delimiter.length()), resultEnd);
}

template<class Iterator>
#ifdef LZ_HAS_STRING_VIEW
LZ_CONSTEXPR_CXX_20 std::string
doMakeString(const Iterator& b, const Iterator& e, const std::string_view delimiter, std::true_type /* isChar */) {
#else
std::string doMakeString(const Iterator& b, const Iterator& e, const std::string& delimiter, std::true_type /* isChar */) {
#endif // LZ_HAS_STRING_VIEW
    if (delimiter.empty()) {
        return std::string(b, e);
    }
    std::string result;
    if LZ_CONSTEXPR_IF (IsForward<Iterator>::value) {
        using lz::distance;
        using std::distance;
        const auto len = static_cast<std::size_t>(distance(b, e));
        result.reserve(len + delimiter.size() * len + 1);
    }
    toStringImpl(result, b, e, delimiter);
    return result;
}

template<class Iterator>
std::string
#ifdef LZ_HAS_STRING_VIEW
    LZ_CONSTEXPR_CXX_20
    doMakeString(const Iterator& b, const Iterator& e, const std::string_view delimiter, std::false_type /* isChar */) {
#else
doMakeString(const Iterator& b, const Iterator& e, const std::string& delimiter, std::false_type /* isChar */) {
#endif // LZ_HAS_STRING_VIEW
    std::string result;
    toStringImpl(result, b, e, delimiter);
    return result;
}

template<class T, class = int>
struct HasResize : std::false_type {};

template<class T>
struct HasResize<T, decltype((void)std::declval<T&>().resize(1), 0)> : std::true_type {};

template<class T, class = int>
struct HasReserve : std::false_type {};

template<class T>
struct HasReserve<T, decltype((void)std::declval<T&>().reserve(1), 0)> : std::true_type {};

template<class LzIterator>
class BasicIteratorView {
protected:
    LzIterator _begin{};
    LzIterator _end{};

public:
    using value_type = internal::ValueType<LzIterator>;
    using iterator = LzIterator;
    using const_iterator = iterator;

private:
    template<class KeySelectorFunc>
    using KeyType = FunctionReturnType<KeySelectorFunc, internal::RefType<LzIterator>>;

#ifndef __cpp_if_constexpr
    template<class Container, class It = LzIterator,
             EnableIf<!HasReserve<Container>::value || !IsForward<It>::value, bool> = true>
    void tryReserve(Container&) const {
    }

    template<class Container, class It = LzIterator, EnableIf<HasReserve<Container>::value && IsForward<It>::value, bool> = true>
    void tryReserve(Container& container) const {
        using lz::distance;
        using std::distance;
        container.reserve(static_cast<std::size_t>(distance(_begin, _end)));
    }
#else
    template<class Container>
    LZ_CONSTEXPR_CXX_20 void tryReserve(Container& container) const {
        if constexpr (HasReserve<Container>::value && IsForward<LzIterator>::value) {
            using lz::distance;
            using std::distance;
            container.reserve(static_cast<std::size_t>(distance(_begin, _end)));
        }
    }
#endif // __cpp_if_constexpr
    template<class MapType, class KeySelectorFunc>
    LZ_CONSTEXPR_CXX_20 void createMap(MapType& map, const KeySelectorFunc keyGen) const {
        transformTo(std::inserter(map, map.end()), [keyGen](internal::RefType<LzIterator> value) {
            return std::make_pair(keyGen(value), value);
        });
    }

public:
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator begin() LZ_CONST_REF_QUALIFIER noexcept {
        return _begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator end() LZ_CONST_REF_QUALIFIER noexcept {
        return _end;
    }

#ifdef LZ_HAS_REF_QUALIFIER
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator begin() && noexcept {
        return std::move(_begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator end() && noexcept {
        return std::move(_end);
    }
#endif // LZ_HAS_REF_QUALIFIER

    constexpr BasicIteratorView() = default;

    constexpr BasicIteratorView(LzIterator&& begin, LzIterator&& end) : _begin(std::move(begin)), _end(std::move(end)) {
    }

    constexpr BasicIteratorView(const LzIterator& begin, const LzIterator& end) : _begin(begin), _end(end) {
    }

    virtual ~BasicIteratorView() = default;

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief Returns an arbitrary container type, of which its constructor signature looks like:
     * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
     * the typedef `value_type`.
     * @attention This function only works properly if the template types matches the container. Example:
     * `to<std::vector, std::allocator<int>>() // fails `
     * `to<std::vector, std::allocator<int>>(std::execution::seq, std::allocator<int>{}); // ok`
     * `to<std::vector>(std::execution::seq, std::allocator<int>{}); // also ok, able to deduce type`
     * `to<std::vector, std::allocator<int>>(std::execution::seq, {}); // also ok`
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
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container<value_type, Decay<Args>...>
    to(Execution execution = std::execution::seq, Args&&... args) const {
        using Cont = Container<value_type, Decay<Args>...>;
        Cont container(std::forward<Args>(args)...);
        tryReserve(container);
        if constexpr (internal::IsSequencedPolicyV<Execution>) {
            tryReserve(container);
            copyTo(std::inserter(container, container.begin()), execution);
        }
        else {
            static_assert(HasResize<Cont>::value, "Container needs to have a method resize() in order to use parallel algorithms."
                                                  " Use std::execution::seq instead");
            using lz::distance;
            using std::distance;
            container.resize(distance(_begin, _end));
            copyTo(container.begin(), execution);
        }
        return container;
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`).
     * @param outputIterator The output to fill into. Essentially the same as:
     * `std::copy(lzView.begin(), lzView.end(), myContainer.begin());`
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     */
    template<class OutputIterator, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 void copyTo(OutputIterator outputIterator, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, OutputIterator>()) {
            std::copy(_begin, _end, outputIterator);
        }
        else {
            static_assert(IsForward<LzIterator>::value,
                          "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
            std::copy(execution, _begin, _end, outputIterator);
        }
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`), using `transformFunc`.
     * @param outputIterator The output to fill into.
     * @param transformFunc The transform function. Must be a callable object that has a parameter of the current value type.
     * Essentially the same as: `std::transform(lzView.begin(), lzView.end(), myContainer.begin(), [](T value) { ... });`
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     */
    template<class OutputIterator, class TransformFunc, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 void
    transformTo(OutputIterator outputIterator, TransformFunc transformFunc, Execution execution = std::execution::seq) const {
        if constexpr (internal::IsSequencedPolicyV<Execution>) {
            std::transform(_begin, _end, outputIterator, transformFunc);
        }
        else {
            static_assert(IsForward<LzIterator>::value, "Iterator type must be at least forward to use parallel execution");
            static_assert(IsForward<OutputIterator>::value,
                          "Output iterator type must be at least forward to use parallel execution");
            std::transform(execution, _begin, _end, outputIterator, transformFunc);
        }
    }

    /**
     * @brief Creates a new `std::vector<value_type>` of the sequence.
     * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @return A `std::vector<value_type>` with the sequence.
     */
    template<class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type> toVector(Execution execution = std::execution::seq) const {
        return to<std::vector>(execution);
    }

    /**
     * @brief Creates a new `std::vector<value_type, Allocator>`.
     * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
     * by this function.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @param alloc The allocator.
     * @return A new `std::vector<value_type, Allocator>`.
     */
    template<class Allocator, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type, Allocator>
    toVector(const Allocator& alloc, Execution execution) const {
        return to<std::vector>(execution, alloc);
    }

    /**
     * @brief Creates a new `std::vector<value_type, N>`.
     * @tparam N The size of the array.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @return A new `std::array<value_type, N>`.
     * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
     */
    template<std::size_t N, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::array<value_type, N> toArray(Execution execution = std::execution::seq) const {
        std::array<value_type, N> container{};
        copyTo(container.begin(), execution);
        return container;
    }
#else
    /**
     * @brief Returns an arbitrary container type, of which its constructor signature looks like:
     * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the sequence is equal to
     * the typedef `value_type`.
     * @attention This function only works properly if the template types matches the container. Example:
     * `to<std::vector, std::allocator<int>>() // fails, no args given`
     * `to<std::vector, std::allocator<int>>(std::allocator<int>{}); // ok`
     * `to<std::vector>(std::allocator<int>{}); // also ok, able to deduce type`
     * `to<std::vector, std::allocator<int>>({}); // also ok`
     * @details Use this function to convert the iterator to a container. Example:
     * ```cpp
     * auto list = lazyIterator.to<std::list>();
     * auto allocator = std::allocator<int>();
     * auto set = lazyIterator.to<std::set>(allocator);
     * ```
     * @tparam Args Additional arguments, automatically deduced
     * @param args Additional arguments, for e.g. an allocator.
     * @return An arbitrary container specified by the entered template parameter.
     */
    template<template<class, class...> class Container, class... Args>
    Container<value_type, Decay<Args>...> to(Args&&... args) const {
        using Cont = Container<value_type, Decay<Args>...>;
        Cont cont(std::forward<Args>(args)...);
        tryReserve(cont);
        copyTo(std::inserter(cont, cont.begin()));
        return cont;
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`)
     * @param outputIterator The output to fill into. Essentially the same as:
     * `std::copy(lzView.begin(), lzView.end(), myContainer.begin());`
     */
    template<class OutputIterator>
    void copyTo(OutputIterator outputIterator) const {
        std::copy(_begin, _end, outputIterator);
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`), using `transformFunc`.
     * @param outputIterator The output to fill into.
     * @param transformFunc The transform function. Must be a callable object that has a parameter of the current value type.
     * Essentially the same as: `std::transform(lzView.begin(), lzView.end(), myContainer.begin(), [](T value) { ... });`
     */
    template<class OutputIterator, class TransformFunc>
    void transformTo(OutputIterator outputIterator, TransformFunc transformFunc) const {
        std::transform(_begin, _end, outputIterator, transformFunc);
    }

    /**
     * @brief Creates a new `std::vector<value_type>` of the sequence.
     * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
     * @return A `std::vector<value_type>` with the sequence.
     */
    std::vector<value_type> toVector() const {
        return to<std::vector>();
    }

    /**
     * @brief Creates a new `std::vector<value_type, Allocator>`.
     * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
     * by this function.
     * @param alloc The allocator
     * @return A new `std::vector<value_type, Allocator>`.
     */
    template<class Allocator>
    std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
        return to<std::vector, Allocator>(alloc);
    }

    /**
     * @brief Creates a new `std::vector<value_type, N>`.
     * @tparam N The size of the array
     * @return A new `std::array<value_type, N>`.
     * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
     */
    template<std::size_t N>
    std::array<value_type, N> toArray() const {
        LZ_ASSERT(std::distance(_begin, _end) <= static_cast<internal::DiffType<LzIterator>>(N),
                  "the iterator size is too large and/or array size is too small");
        std::array<value_type, N> cont{};
        copyTo(cont.begin());
        return cont;
    }
#endif // LZ_HAS_EXECUTION

    /**
     * Creates a `std::map<<keyGen return type, value_type[, Compare[, Allocator]]>`. The keyGen function generates the keys
     * for the `std::map`. The value type is the current type this view contains. (`typename decltype(view)::value_type`).
     * @param keyGen Function generates the keys for the `std::map`. Must contains 1 arg that is equal to `typename
     * decltype(view)::value_type`
     * @param allocator Optional, a custom allocator. `std::allocator<decltype(func(*begin()))>` is default.
     * @param cmp Optional, a custom key comparer. `std::less<decltype(func(*begin()))>` is default.
     * @return A `std::map` with as key type the return type of `keyGen`, and as value the current values contained by this view.
     */
    template<class KeySelectorFunc, class Compare = std::less<KeyType<KeySelectorFunc>>,
             class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>
    toMap(const KeySelectorFunc keyGen, const Allocator& allocator = {}, const Compare& cmp = {}) const {
        using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
        Map m(cmp, allocator);
        createMap(m, keyGen);
        return m;
    }

    /**
     * Creates a `std::unordered_map<<keyGen return type, value_type[, Hasher[, KeyEquality[, Allocator]]]>`. The keyGen function generates the keys
     * for the `std::unordered_map`. The value type is the current type this view contains. (`typename decltype(view)::value_type`).
     * @param keyGen Function generates the keys for the `std::unordered_map`. Must contains 1 arg that is equal to `typename
     * decltype(view)::value_type`
     * @param allocator Optional, a custom allocator. `std::allocator<decltype(func(*begin()))>` is default.
     * @param cmp Optional, a custom key comparer. `std::less<decltype(func(*begin()))>` is default.
     * @param cmp Optional, the key comparer. `std::equal_to<decltype(func(*begin()))>` is default.
     * @param h Hash function. `std::hash<decltype(func(*begin()))>` is default.
     * @return A `std::map` with as key type the return type of `keyGen`, and as value the current values contained by this view.
     */
    template<class KeySelectorFunc, class Hasher = std::hash<KeyType<KeySelectorFunc>>,
             class KeyEquality = std::equal_to<KeyType<KeySelectorFunc>>,
             class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>
    toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& alloc = {}, const KeyEquality& cmp = {},
                   const Hasher& h = {}) const {
        using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>;
        using lz::distance;
        using std::distance;
        UnorderedMap um(static_cast<std::size_t>(distance(_begin, _end)), h, cmp, alloc);
        createMap(um, keyGen);
        return um;
    }

    /**
     * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
     * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
     * @param delimiter The delimiter between the previous value and the next.
     * @return The converted iterator in string format.
     */
#ifdef LZ_HAS_STRING_VIEW
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string toString(const std::string_view delimiter = "") const {
#else
    LZ_NODISCARD std::string toString(const std::string& delimiter = "") const {
#endif
        // clang-format off
        return internal::doMakeString(_begin, _end, delimiter, std::is_same<char, value_type>());
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
}; // namespace internal
// clang-format on
} // namespace lz

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

#ifndef LZ_HAS_EXECUTION
/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
#ifdef LZ_HAS_CXX_11
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<internal::ValueTypeIterable<IterableA>>>
#else
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<>>
#endif // LZ_HAS_CXX_11
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
    return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
}
#else // ^^^ !LZ_HAS_EXECUTION vvv LZ_HAS_EXECUTION
/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<>,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
    if constexpr (internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableA>>() &&
                  internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableB>>()) {
        static_cast<void>(execution);
        return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
    else {
        return std::equal(execution, std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
}
#endif
} // Namespace lz

// End of group
/**
 * @}
 */

#endif // LZ_BASIC_ITERATOR_VIEW_HPP