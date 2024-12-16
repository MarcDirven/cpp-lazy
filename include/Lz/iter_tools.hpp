#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#define LZ_FUNCTION_TOOLS_HPP

#include "Concatenate.hpp"
#include "drop_while.hpp"
#include "filter.hpp"
#include "join.hpp"
#include "map.hpp"
#include "splitter.hpp"
#include "take.hpp"
#include "take_while.hpp"
#include "zip.hpp"

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
struct convert_fn {
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
struct get_fn {
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
 * Returns a split_iterable iterator, that splits the string on `'\n'`.
 * @tparam SubString The string type that the `split_iterable::value_type` must return. Must either be std::string or
 * std::string_view.
 * @tparam String The string type. `std::string` is assumed but can be specified.
 * @param string The string to split on.
 * @return Returns a split_iterable iterator, that splits the string on `'\n'`.
 */
template<class String>
LZ_NODISCARD string_splitter<String, typename String::value_type> lines(const String& string) {
    return lz::split(string, "\n");
}

/**
 * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
 * @param strings The container of `std::string` or `std::string_view`.
 * @return A Join iterator that joins the strings in the container on `'\n'`.
 */
template<class Strings>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<iter<Strings>, sentinel<Strings>> unlines(Strings&& strings) {
    return lz::join(strings, "\n");
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<std::reverse_iterator<iter<Iterable>>> reverse(Iterable&& iterable) {
    // ADL std::reverse
    std::reverse_iterator<Iterator> rev_begin(detail::begin(std::forward<Iterable>(iterable)));
    std::reverse_iterator<Iterator> rev_end(detail::end(std::forward<Iterable>(iterable)));
    return { std::move(rev_begin), std::move(rev_end) };
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERATOR Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<iter<Iterable>, sentinel<Iterable>, detail::convert_fn<T>> as(Iterable&& iterable) {
    return lz::map(std::forward<Iterable>(iterable), detail::convert_fn<T>());
}

template<class Fn, class... Iterables>
using ZipWith =
    Map<decltype(detail::begin(std::forward<Zip<iter<Iterables>...>>(std::declval<Zip<iter<Iterables>...>>()))),
        decltype(detail::end(std::forward<Zip<iter<Iterables>...>>(std::declval<Zip<iter<Iterables>...>>()))),
        decltype(detail::make_expand_fn(std::move(std::declval<Fn>()), detail::make_index_sequence<sizeof...(Iterables)>()))>;

/**
 * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
 * Signature must look like: `fn(value_type_tIterable1[, value_type_tIterable2[, value_type_tIterable3[, value_type_tIterable-n[,
 * ...]]]])`
 * @param fn The function to apply to each elements in all containers.
 * @param iterables The iterables.
 * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
 */
template<class Fn, class... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ZipWith<Fn, Iterables...> zipWith(Fn fn, Iterables&&... iterables) {
    auto zipper = lz::zip(std::forward<Iterables>(iterables)...);
    auto tupleExpanderFunc = detail::make_expand_fn(std::move(fn), detail::make_index_sequence<sizeof...(Iterables)>());
    return lz::map(std::move(zipper), std::move(tupleExpanderFunc));
}

/**
 * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
 * @param iterable A container/iterable.
 * @return A zip iterator that accesses two adjacent elements of one container.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<iter<Iterable>, iter<Iterable>> pairwise(Iterable&& iterable) {
    LZ_ASSERT(lz::has_many(iterable), "length of the sequence must be greater than or equal to 2");
    auto begin = std::begin(iterable);
    auto end = std::end(iterable);
    return { std::make_tuple(begin, std::next(begin)), std::make_tuple(end, end) };
}

/**
 * Returns an iterator object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
 * Useful for example getting the values from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<iter<Iterable>, sentinel<Iterable>, detail::get_fn<1>> values(Iterable&& iterable) {
    return lz::map(std::forward<Iterable>(iterable), detail::get_fn<1>());
}

/**
 * Returns an iterator object that returns all keys from an iterable of which its `value_type` supports `std::get<0>`.
 * Useful for example getting the keys from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the first value of `value_type` using `std::get<0>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<iter<Iterable>, sentinel<Iterable>, detail::get_fn<0>> keys(Iterable&& iterable) {
    return lz::map(std::forward<Iterable>(iterable), detail::get_fn<0>());
}

template<class Iterable, class UnaryFilterFunc, class UnaryMapFunc>
using FilterMap =
    Map<decltype(lz::filter(std::forward<Iterable>(std::declval<Iterable>()), std::declval<UnaryFilterFunc>()).begin()),
        decltype(lz::filter(std::forward<Iterable>(std::declval<Iterable>()), std::declval<UnaryFilterFunc>()).end()),
        UnaryMapFunc>;

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param iterable The iterable to filter/map.
 * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container
 * value is passed to the `mapFunc`.
 * @param mapFunc The function that returns the (new) type.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `mapFunc`.
 */
template<class Iterable, class UnaryFilterFunc, class UnaryMapFunc>
FilterMap<Iterable, UnaryFilterFunc, UnaryMapFunc>
filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
    return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(filterFunc)), std::move(mapFunc));
}

// TODO: make typedef that works here for selector using std::tuple
// template<class Iterable, class SelectorIterable>
// using Selector = FilterMap<Zip<iter<Iterable>, sentinel<Iterable>, iter<SelectorIterable>>, detail::get_fn<1>,
// detail::get_fn<0>>;

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param iterable The iterable to select values from.
 * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<class Iterable, class SelectorIterable>
auto select(Iterable&& iterable, SelectorIterable&& selectors)
    -> decltype(filterMap(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::get_fn<1>(),
                          detail::get_fn<0>())) {
    return filterMap(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::get_fn<1>(),
                     detail::get_fn<0>());
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
detail::basic_iterable<std::reverse_iterator<std::reverse_iterator<iter<Iterable>>>>
trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
    auto takenFirst = lz::drop_while(std::forward<Iterable>(iterable), std::move(first));
    auto takenLast = lz::drop_while(lz::reverse(std::move(takenFirst)), std::move(last));
    return lz::reverse(std::move(takenLast));
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param s The string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
template<class String>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(String&& s) {
    const auto isSpaceFn = [](const char c) {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    };
    return lz::trim(std::forward<String>(s), isSpaceFn, isSpaceFn);
}

} // End namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // End LZ_FUNCTION_TOOLS_HPP