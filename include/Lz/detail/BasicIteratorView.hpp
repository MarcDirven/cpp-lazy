#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include "Lz/StringView.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/Concepts.hpp"
#include "Lz/detail/Procs.hpp"
#include "Lz/detail/Traits.hpp"

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
#endif

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty(const Iterator& begin, const Iterator& end) {
    return begin == end;
}

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty(const Iterable& iterable) {
    return lz::empty(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::RefType<Iterator> front(const Iterator& begin, const Iterator& end) {
    LZ_ASSERT(!lz::empty(begin, end), "sequence cannot be empty in order to get the first element");
    static_cast<void>(end);
    return *begin;
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param iterable The sequence to get the first element of.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::RefType<detail::IterTypeFromIterable<Iterable>> front(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the last element");
    return lz::front(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @param end The ending of the sequence.
 * @warning Static sserts if the iterator is not (at least) bidirectional
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::RefType<Iterator> back(Iterator begin, Iterator end) {
    LZ_ASSERT(!lz::empty(begin, end), "sequence cannot be empty in order to get the last element");
    static_assert(detail::IsBidirectional<Iterator>::value, "Iterator is not bidirectional. Cannot get last element");
    return *--end;
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @warning Static sserts if the iterator is not (at least) bidirectional
 * @param iterable The sequence to get the last element of.
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::RefType<detail::IterTypeFromIterable<Iterable>> back(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the last element");
    return lz::back(std::begin(iterable), std::end(iterable));
}

LZ_MODULE_EXPORT_SCOPE_END

namespace detail {
#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
template<class Iterator>
EnableIf<std::is_arithmetic<ValueType<Iterator>>::value>
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const StringView delimiter) {
    std::for_each(begin, end, [&delimiter, &result](const ValueType<Iterator>& vt) {
        char buff[SafeBufferSize<ValueType<Iterator>>::value]{};
        toStringFromBuff(vt, buff);
        result.append(buff);
        result.append(delimiter.begin(), delimiter.end());
    });
}

template<class Iterator>
EnableIf<!std::is_arithmetic<ValueType<Iterator>>::value>
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const StringView delimiter) {
    std::ostringstream oss;
    std::for_each(begin, end, [&oss, &delimiter](const ValueType<Iterator>& t) { oss << t << delimiter; });
    result = oss.str();
}
#endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)

template<class Iterator>
LZ_CONSTEXPR_CXX_20 void
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const StringView delimiter, const StringView fmt) {
#else
toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const StringView delimiter) {
#endif // LZ_HAS_FORMAT
    if (lz::empty(begin, end)) {
        return;
    }
#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
    auto backInserter = std::back_inserter(result);
    // Dynamic format merge with the `fmt` and the "{}" for the delimiter
    std::string format;
    format.reserve(3 + fmt.size());
    format.append(fmt.data(), fmt.size());
    format += "{}";
#endif // !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

#if defined(LZ_STANDALONE)
#if defined(LZ_HAS_FORMAT)
    std::for_each(begin, end, [&delimiter, backInserter, &format](const ValueType<Iterator>& v) {
        std::vformat_to(backInserter, format.c_str(), std::make_format_args(v, delimiter));
    });
    // clang-format off
#else
    toStringImplSpecialized(result, begin, end, delimiter);
#endif // defined(LZ_HAS_FORMAT)
#else
    std::for_each(begin, end, [&delimiter, backInserter, &format](const ValueType<Iterator>& v) {
#if defined(LZ_HAS_CXX_20) && FMT_VERSION >= 90000
        fmt::format_to(backInserter, fmt::runtime(format.c_str()), v, delimiter);
#else
        fmt::format_to(backInserter, format.c_str(), v, delimiter);
#endif // LZ_HAS_CXX_20 && FMT_VERSION >= 90000
    });
#endif // defined(LZ_STANDALONE)
    const auto resultEnd = result.end();
    result.erase(resultEnd - static_cast<std::ptrdiff_t>(delimiter.size()), resultEnd);
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20 detail::EnableIf<std::is_same<char, ValueType<Iterator>>::value, std::string>
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter, const StringView fmt) {
#else
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter) {
#endif // LZ_HAS_FORMAT
    if (delimiter.size() == 0) {
        return std::string(b, e);
    }
    std::string result;

    const auto len = static_cast<std::size_t>(sizeHint(b, e));
    if (len != 0) {
        result.reserve(len + delimiter.size() * len + 1);
    }

#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toStringImpl(result, b, e, delimiter, fmt);
#else
    toStringImpl(result, b, e, delimiter);
#endif
    return result;
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20 detail::EnableIf<!std::is_same<char, ValueType<Iterator>>::value, std::string>
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter, const StringView fmt) {
#else
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter) {
#endif // LZ_HAS_FORMAT
    std::string result;
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toStringImpl(result, b, e, delimiter, fmt);
#else
    toStringImpl(result, b, e, delimiter);
#endif
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

template<class It>
class BasicIteratorView {
protected:
    It _begin{};
    It _end{};

public:
    using value_type = ValueType<It>;
    using iterator = It;
    using reference = decltype(*_begin);
    using const_reference = typename std::add_const<reference>::type;
    using const_iterator = iterator;

private:
    template<class KeySelectorFunc>
    using KeyType = FunctionReturnType<KeySelectorFunc, RefType<It>>;

#ifndef __cpp_if_constexpr
    template<class Container>
    EnableIf<!HasReserve<Container>::value, void> tryReserve(Container&) const {
    }

    template<class Container>
    EnableIf<HasReserve<Container>::value, void> tryReserve(Container& container) const {
        container.reserve(static_cast<std::size_t>(sizeHint(_begin, _end)));
    }
#else
    template<class Container>
    LZ_CONSTEXPR_CXX_20 void tryReserve(Container& container) const {
        if constexpr (HasReserve<Container>::value) {
            container.reserve(static_cast<std::size_t>(sizeHint(_begin, _end)));
        }
    }
#endif // __cpp_if_constexpr
    template<class MapType, class KeySelectorFunc>
    LZ_CONSTEXPR_CXX_20 void createMap(MapType& map, KeySelectorFunc keyGen) const {
#ifdef LZ_HAS_CXX_14
        transformTo(std::inserter(map, map.end()), [k = std::move(keyGen)](auto&& value) { 
            return std::make_pair(k(value), value); });
#else
        transformTo(std::inserter(map, map.end()), [&keyGen](const value_type& value) { return std::make_pair(keyGen(value), value); });
#endif
    }

public:
#ifdef __cpp_explicit_this_parameter
    template<class Self>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto&& begin(this Self&& self) {
        return std::forward<Self>(self)._begin;
    }

    template<class Self>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto&& end(this Self&& self) {
        return std::forward<Self>(self)._end;
    }
#else
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() LZ_CONST_REF_QUALIFIER noexcept {
        return _begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It end() LZ_CONST_REF_QUALIFIER noexcept {
        return _end;
    }
#endif

#if defined(LZ_HAS_REF_QUALIFIER) && !defined(__cpp_explicit_this_parameter)
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() && noexcept {
        return std::move(_begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It end() && noexcept {
        return std::move(_end);
    }
#endif // LZ_HAS_REF_QUALIFIER

    constexpr BasicIteratorView() = default;

    constexpr BasicIteratorView(It&& begin, It&& end) noexcept : _begin(std::move(begin)), _end(std::move(end)) {
    }

    constexpr BasicIteratorView(const It& begin, const It& end) noexcept : _begin(begin), _end(end) {
    }

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief Returns an arbitrary container type, of which its constructor signature looks like:
     * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
     * the typedef `value_type`.
     * @attention This function only works properly if the template types matches the container. Example:
     * `to<std::vector, std::allocator<int>>() // fails`
     * `to<std::vector, std::allocator<int>>(std::execution::seq, std::allocator<int>{}); // ok`
     * `to<std::vector>(std::execution::seq, std::allocator<int>{}); // also ok, able to deduce type`
     * `to<std::vector, std::allocator<int>>(std::execution::seq, {}); // also ok`
     * `to<std::vector>(); // also ok`
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
        return to<Cont>(execution, std::forward<Args>(args)...);
    }

    /**
     * @brief This function can be used to create a new container from the current view. The template parameter `Container`
     * must be specified along with its value type: `view.to<std::vector<int>>()`. One could also use `view.to<std::vector>()`, 
     * however an other function will then be called; see the other `to` function overload for that documentation.
     * @example `lzView.to<std::vector<int>>(std::execution::seq, 100); // This will create a vec of size 100 with containing the
     * contents of lzView`
     * @tparam Container The container along with its value type.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @param args Additional container args. Must be compatible with the constructor of `Container`
     * @return The container.
     */
    template<class Container, class... Args, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container to(Execution execution = std::execution::seq, Args&&... args) const {
        Container container(std::forward<Args>(args)...);
        tryReserve(container);
        if constexpr (detail::IsSequencedPolicyV<Execution>) {
            copyTo(std::inserter(container, container.begin()), execution);
        }
        else {
            static_assert(HasResize<Container>::value, "Container needs to have a method resize() in order to use parallel"
                                                       " algorithms. Use std::execution::seq instead");
            static_assert(detail::IsRandomAccess<It>::value,
                          "The iterator needs to be random access. Please use copyTo if you want to parallize this function");
            container.resize(static_cast<std::size_t>(_end - _begin));
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
        if constexpr (detail::isCompatibleForExecution<Execution, OutputIterator>()) {
            std::copy(_begin, _end, outputIterator);
        }
        else {
            static_assert(IsForward<It>::value,
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
    transformTo(OutputIterator outputIterator, TransformFunc&& transformFunc, Execution execution = std::execution::seq) const {
        if constexpr (detail::IsSequencedPolicyV<Execution>) {
            std::transform(_begin, _end, outputIterator, std::forward<TransformFunc>(transformFunc));
        }
        else {
            static_assert(IsForward<It>::value, "Iterator type must be at least forward to use parallel execution");
            static_assert(IsForward<OutputIterator>::value,
                          "Output iterator type must be at least forward to use parallel execution");
            std::transform(execution, _begin, _end, outputIterator, std::forward<TransformFunc>(transformFunc));
        }
    }
    
    /**
     * @brief Transforms the current iterator to a container of type `Container`. The container is filled with the result of the function `f`.
     * i.e. `view.transformAs<std::vector>([](int i) { return i * 2; });` will return a vector with all elements doubled.
     * @tparam Container The container to transform into
     * @param f The transform function
     * @param exec The execution policy. Must be one of `std::execution`'s tags.
     * @param args args passed to the container
     * @return The container
     */
    template<template<class, class...> class Container, class TransformFunc, class Execution = std::execution::sequenced_policy, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto transformAs(TransformFunc&& f, Execution exec = std::execution::seq, 
                                                      Args&&... args) const -> Container<decltype(f(*begin())), Decay<Args>...> {
        using Cont = Container<decltype(f(*begin())), Decay<Args>...>;
        Cont cont(std::forward<Args>(args)...);
        transformTo(std::inserter(cont, cont.begin()), std::forward<TransformFunc>(f), exec);
        return cont;
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
        return to<Cont>(std::forward<Args>(args)...);
    }

    /**
     * @brief This function can be used to create a new container from the current view. The template parameter `Container`
     * must be specified along with its value type: `view.to<std::vector<int>>()`. One could also use `view.to<std::vector>()`.
     * See the other `to` function overload for documentation.
     * @tparam Container The container along with its value type.
     * @example `lzView.to<std::vector<int>>(100); // This will create a vec of size 100 with containing the contents of lzView`
     * @param args Additional container args. Must be compatible with the constructor of `Container`
     * @return The container.
     */
    template<class Container, class... Args>
    Container to(Args&&... args) const {
        Container cont(std::forward<Args>(args)...);
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
    void transformTo(OutputIterator outputIterator, TransformFunc&& transformFunc) const {
        std::transform(_begin, _end, outputIterator, std::forward<TransformFunc>(transformFunc));
    }

    
    /**
     * @brief Transforms the current iterator to a container of type `Container`. The container is filled with the result of the function `f`.
     * i.e. `view.transformAs<std::vector>([](int i) { return i * 2; });` will return a vector with all elements doubled.
     * @tparam Container The container to transform into
     * @param f The transform function
     * @param args args passed to the container
     * @return The container
     */
    template<template<class, class...> class Container, class TransformFunc, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto transformAs(TransformFunc&& f, Args&&... args) const -> Container<decltype(f(*begin())), Decay<Args>...> {
        using Cont = Container<decltype(f(*begin())), Decay<Args>...>;
        Cont cont(std::forward<Args>(args)...);
        transformTo(std::inserter(cont, cont.begin()), std::forward<TransformFunc>(f));
        return cont;
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
     * Creates a `std::unordered_map<<keyGen return type, value_type[, Hasher[, KeyEquality[, Allocator]]]>`. The keyGen function
     * generates the keys for the `std::unordered_map`. The value type is the current type this view contains. (`typename
     * decltype(view)::value_type`).
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
        UnorderedMap um(static_cast<std::size_t>(detail::sizeHint(_begin, _end)), h, cmp, alloc);
        createMap(um, keyGen);
        return um;
    }

    /**
     * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
     * lz::range(4).toString(" ") yields 0 1 2 3 and lz::range(4).toString(", ") yields 0, 1, 2, 3.
     * @param delimiter The delimiter between the previous value and the next.
     * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
     * @return The converted iterator in string format.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toString(const StringView delimiter = "", const StringView fmt = "{}") const {
        return detail::doMakeString(_begin, _end, delimiter, fmt);
#else
    toString(const StringView delimiter = "") const {
        return detail::doMakeString(_begin, _end, delimiter);
#endif
        // clang-format off
    }

    /**
     * Function to stream the iterator to an output stream e.g. `std::cout`.
     * @param o The stream object.
     * @param it The iterator to print.
     * @return The stream object by reference.
     */
    friend std::ostream& operator<<(std::ostream& o, const BasicIteratorView<It>& it) {
        const auto end = std::end(it);
#ifndef LZ_STANDALONE
        auto out = std::ostream_iterator<char>(o);
        for (auto i = std::begin(it); i != end; ++i) {
            if (std::next(i) == end) {
                fmt::format_to(out, "{}", *i);
                break;
            }
            fmt::format_to(out, "{}, ", *i);
        }
#elif defined(LZ_HAS_FORMAT) && defined(LZ_STANDALONE)
        auto out = std::ostream_iterator<char>(o);
        for (auto i = std::begin(it); i != end; ++i) {
            if (std::next(i) == end) {
                std::format_to(out, "{}", *i);
                break;
            }
            std::format_to(out, "{}, ", *i);
        }
#else
        for (auto i = std::begin(it); i != end; ++i) {
            if (std::next(i) == end) {
                o << *i;
                break;
            }
            o << *i << ", ";
        }
#endif
        return o;
    }

    /**
     * Returns the length of the view.
     * @note Please note that this traverses the whole sequence.
     * @return The length of the view.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::DiffType<It> distance() const {
        return std::distance(_begin, _end);
    }

    /**
     * Gets the nth position of the iterator from this sequence.
     * @param n The offset.
     * @return The element referred to by `begin() + n`
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It next(const detail::DiffType<It> n = 1) const {
        return std::next(_begin, n);
    }

    /**
     * Checks is the sequence is empty.
     * @return True if it is empty, false otherwise.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty() const {
        return lz::empty(_begin, _end);
    }

    //! See `lz::front(Iterable)` for details
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference front() const {
        return lz::front(*this);
    }

    /**
     * @brief See `lz::back` for details
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference back() const {
        return lz::back(*this);
    }
}; // namespace detail
// clang-format on
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a view to another view or container. Can be handy to slice, skip a few first or last elements using:
 * `auto range = lz::view(vec.begin() + 4, vec.end());`
 * @param begin The beginning of the 'view'.
 * @param end The ending of the 'view'.
 * @return A View object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<Iterator> view(Iterator begin, Iterator end) {
    return { begin, end };
}

#ifndef LZ_HAS_EXECUTION
/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB,
         class BinaryPredicate = MAKE_BIN_OP(std::equal_to, detail::ValueTypeIterable<IterableA>)>
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
    return std::equal(std::begin(a), std::end(a), std::begin(b), std::move(predicate));
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
    if constexpr (detail::isCompatibleForExecution<Execution, detail::IterTypeFromIterable<IterableA>>() &&
                  detail::isCompatibleForExecution<Execution, detail::IterTypeFromIterable<IterableB>>()) {
        static_cast<void>(execution);
        return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
    else {
        return std::equal(execution, std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
}
#endif // LZ_HAS_EXECUTION
} // Namespace lz

#if defined(LZ_HAS_FORMAT) && defined(LZ_STANDALONE)
template<class Iterable>
struct std::formatter<
    Iterable,
    lz::detail::EnableIf< // Enable if Iterable is base of BasicIteratorView
        std::is_base_of<lz::detail::BasicIteratorView<lz::detail::IterTypeFromIterable<Iterable>>, Iterable>::value, char>>
    : std::formatter<std::string> {
    using InnerIter = lz::detail::BasicIteratorView<lz::detail::IterTypeFromIterable<Iterable>>;

    template<class FormatCtx>
    auto format(const InnerIter& it, FormatCtx& ctx) const -> decltype(ctx.out()) {
        return std::formatter<std::string>::format(it.toString(), ctx);
    }
};
#elif !defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
template<class Iterable>
struct fmt::formatter<
    Iterable,
    lz::detail::EnableIf<
        std::is_base_of<lz::detail::BasicIteratorView<lz::detail::IterTypeFromIterable<Iterable>>, Iterable>::value, char>>
    : fmt::ostream_formatter {};
#endif

LZ_MODULE_EXPORT_SCOPE_END
// End of group
/**
 * @}
 */

#endif // LZ_BASIC_ITERATOR_VIEW_HPP