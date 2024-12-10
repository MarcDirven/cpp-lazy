#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#define LZ_FUNCTION_TOOLS_HPP

#include "Concatenate.hpp"
#include "Filter.hpp"
#include "Join.hpp"
#include "Map.hpp"
#include "Splitter.hpp"
#include "Take.hpp"
#include "TakeWhile.hpp"
#include "Zip.hpp"

#include <algorithm>
#include <cctype>
#include <numeric>

#ifdef LZ_HAS_CXX_17
#define LZ_INLINE_VAR inline
#else // ^^^ has cxx 17 vvv !has cxx 17
#define LZ_INLINE_VAR
#endif // LZ_HAS_CXX_17

namespace lz {
namespace detail {
template<class To>
struct ConvertFn {
    template<class From>
    LZ_CONSTEXPR_CXX_20 To operator()(From&& f) const {
        return static_cast<To>(f);
    }

    template<class From>
    LZ_CONSTEXPR_CXX_20 To operator()(From&& f) {
        return static_cast<To>(f);
    }
};

template<std::size_t I>
struct GetFn {
    template<class T>
    LZ_CONSTEXPR_CXX_20 auto operator()(T&& gettable) noexcept -> decltype(std::get<I>(std::forward<T>(gettable))) {
        return std::get<I>(std::forward<T>(gettable));
    }
};
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * This value is returned when indexOf(If) does not find the value specified.
 */
constexpr LZ_INLINE_VAR std::size_t npos = (std::numeric_limits<std::size_t>::max)();

/**
 * Returns a Splitter iterator, that splits the string on `'\n'`.
 * @tparam SubString The string type that the `Splitter::value_type` must return. Must either be std::string or
 * std::string_view.
 * @tparam String The string type. `std::string` is assumed but can be specified.
 * @param string The string to split on.
 * @return Returns a Splitter iterator, that splits the string on `'\n'`.
 */
template<class String>
LZ_NODISCARD StringSplitter<String, char> lines(const String& string) {
    return lz::split(string, "\n");
}

/**
 * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
 * @param strings The container of `std::string` or `std::string_view`.
 * @return A Join iterator that joins the strings in the container on `'\n'`.
 */
template<class Strings>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<IterT<Strings>> unlines(Strings&& strings) {
    return lz::join(strings, "\n");
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param begin The beginning of the sequence. Must have at least std::bidirectional_iterator_tag.
 * @param end The ending of the sequence. Must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<std::reverse_iterator<Iterator>>
reverse(Iterator begin, Iterator end) {
#ifndef LZ_HAS_CONCEPTS
    static_assert(detail::IsBidirectional<Iterator>::value, "the type of the iterator must be bidirectional or stronger");
#endif // LZ_HAS_CONCEPTS
    std::reverse_iterator<Iterator> reverseBegin(std::move(end));
    std::reverse_iterator<Iterator> reverseEnd(std::move(begin));
    return { std::move(reverseBegin), std::move(reverseEnd) };
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<std::reverse_iterator<IterT<Iterable>>> reverse(Iterable&& iterable) {
    // ADL std::reverse
    return lz::reverse(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(...begin(), ...end())` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERATOR Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, detail::ConvertFn<T>> as(Iterator begin, S end) {
    return lz::mapRange(std::move(begin), std::move(end), detail::ConvertFn<T>());
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @tparam Iterable A container/iterable.
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<IterT<Iterable>, detail::ConvertFn<T>> as(Iterable&& iterable) {
    return lz::as<T>(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
 * Signature must look like: `fn(ValueTypeIterable1, ValueTypeIterable2[, ValueTypeIterable3[, ValueTypeIterable-n[, ...]]])`
 * @param fn The function to apply to each elements in all containers.
 * @param begin The beginning of all the iterables
 * @param end The ending of all the iterables
 * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
 */
template<class Fn, LZ_CONCEPT_ITERATOR... Iterators, class Zipper = Zip<Iterators...>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zipWith(Fn fn, std::tuple<Iterators...> begin, std::tuple<Iterators...> end)
    -> Map<decltype(std::begin(std::declval<Zipper>())),
           decltype(detail::makeExpandFn(std::move(fn), detail::MakeIndexSequence<sizeof...(Iterators)>()))> {
    Zipper zipper = lz::zipRange(std::move(begin), std::move(end));
    auto tupleExpanderFunc = detail::makeExpandFn(std::move(fn), detail::MakeIndexSequence<sizeof...(Iterators)>());
    return lz::map(std::move(zipper), std::move(tupleExpanderFunc));
}

/**
 * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
 * Signature must look like: `fn(ValueTypeIterable1[, ValueTypeIterable2[, ValueTypeIterable3[, ValueTypeIterable-n[, ...]]]])`
 * @param fn The function to apply to each elements in all containers.
 * @param iterables The iterables.
 * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
 */
template<class Fn, class... Iterables, class Zipper = Zip<IterT<Iterables>...>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zipWith(Fn fn, Iterables&&... iterables)
    -> Map<decltype(std::begin(std::declval<Zipper>())),
           decltype(detail::makeExpandFn(std::move(fn), detail::MakeIndexSequence<sizeof...(Iterables)>()))> {
    return lz::zipWith(std::move(fn), std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
                       std::make_tuple(detail::end(std::forward<Iterables>(iterables))...));
}

/**
 * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
 * @param iterable A container/iterable.
 * @return A zip iterator that accesses two adjacent elements of one container.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<IterT<Iterable>, IterT<Iterable>> pairwise(Iterable&& iterable) {
    LZ_ASSERT(lz::hasMany(iterable), "length of the sequence must be greater than or equal to 2");
    auto begin = std::begin(iterable);
    auto end = std::end(iterable);
    return { std::make_tuple(begin, std::next(begin)), std::make_tuple(end, end) };
}

/**
 * Returns an iterator object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
 * Useful for example getting the values from a `std::(unordered_)map`
 * @param begin The beginning of the iterator.
 * @param end The ending of the iterator.
 * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERATOR Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, detail::GetFn<1>> values(Iterator begin, S end) {
    return lz::mapRange(std::move(begin), std::move(end), detail::GetFn<1>());
}

/**
 * Returns an iterator object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
 * Useful for example getting the values from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<IterT<Iterable>, detail::GetFn<1>> values(Iterable&& iterable) {
    return lz::values(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns an iterator object that returns all keys from an iterable of which its `value_type` supports `std::get<0>`.
 * Useful for example getting the keys from a `std::(unordered_)map`
 * @param begin The beginning of the iterator.
 * @param end The ending of the iterator.
 * @return An iterable object that returns the first value of `value_type` using `std::get<0>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, detail::GetFn<0>> keys(Iterator begin, Iterator end) {
    return lz::mapRange(std::move(begin), std::move(end), detail::GetFn<0>());
}

/**
 * Returns an iterator object that returns all keys from an iterable of which its `value_type` supports `std::get<0>`.
 * Useful for example getting the keys from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the first value of `value_type` using `std::get<0>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<IterT<Iterable>, detail::GetFn<0>> keys(Iterable&& iterable) {
    return lz::keys(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param iterable The iterable to filter/map.
 * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
 * passed to the `mapFunc`.
 * @param mapFunc The function that returns the (new) type.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `mapFunc`.
 */
template<class Iterable, class UnaryFilterFunc, class UnaryMapFunc>
Map<detail::FilterIterator<IterT<Iterable>, SentinelT<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
    return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(filterFunc)), std::move(mapFunc));
}

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param iterable The iterable to select values from.
 * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<class Iterable, class SelectorIterable>
auto select(Iterable&& iterable, SelectorIterable&& selectors)
    -> decltype(filterMap(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::GetFn<1>(),
                          detail::GetFn<0>())) {
    return filterMap(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::GetFn<1>(),
                     detail::GetFn<0>());
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long
 * as `last` returns true for the trimming of the ending.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @return An iterator view object.
 */
template<class Iterator, class UnaryPredicateFirst, class UnaryPredicateLast>
detail::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<Iterator>>>
trim(Iterator begin, Iterator end, UnaryPredicateFirst first, UnaryPredicateLast last) {
    auto takenFirst = lz::dropWhileRange(std::move(begin), std::move(end), std::move(first));
    auto takenLast = lz::dropWhile(lz::reverse(std::move(takenFirst)), std::move(last));
    return lz::reverse(std::move(takenLast));
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long
 * as `last` returns true for the trimming of the ending.
 * @param iterable The iterable to trim
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @return An iterator view object.
 */
template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
detail::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<IterT<Iterable>>>>
trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
    return lz::trim(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                    std::move(first), std::move(last));
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param begin The beginning of the string to trim.
 * @param end The ending of the string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
LZ_NODISCARD
LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(std::string::const_iterator begin, std::string::const_iterator end) {
    const auto isSpaceFn = [](const char c) {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    };
    return lz::trim(begin, end, isSpaceFn, isSpaceFn);
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param s The string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
LZ_NODISCARD
LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(const std::string& s) {
    return lz::trimString(s.begin(), s.end());
}

} // End namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // End LZ_FUNCTION_TOOLS_HPP