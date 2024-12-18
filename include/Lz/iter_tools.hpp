#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#define LZ_FUNCTION_TOOLS_HPP

#include "Lz/concatenate.hpp"
#include "Lz/drop_while.hpp"
#include "Lz/filter.hpp"
#include "Lz/join.hpp"
#include "Lz/map.hpp"
#include "Lz/split.hpp"
#include "Lz/take.hpp"
#include "Lz/take_while.hpp"
#include "Lz/zip.hpp"

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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<iter_t<Strings>, sentinel_t<Strings>> unlines(Strings&& strings) {
    return lz::join(strings, "\n");
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<std::reverse_iterator<iter_t<Iterable>>> reverse(Iterable&& iterable) {
    using iterator = iter_t<Iterable>;
    std::reverse_iterator<iterator> rev_begin(detail::begin(std::forward<Iterable>(iterable)));
    std::reverse_iterator<iterator> rev_end(detail::end(std::forward<Iterable>(iterable)));
    return { std::move(rev_end), std::move(rev_begin) };
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERATOR Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::convert_fn<T>>
as(Iterable&& iterable) {
    return lz::map(std::forward<Iterable>(iterable), detail::convert_fn<T>());
}

template<class Fn, class... Iterables>
using zip_with_iterable = map_iterable<
    decltype(detail::begin(std::forward<zip_iterable<iter_t<Iterables>...>>(std::declval<zip_iterable<iter_t<Iterables>...>>()))),
    decltype(detail::end(std::forward<zip_iterable<iter_t<Iterables>...>>(std::declval<zip_iterable<iter_t<Iterables>...>>()))),
    decltype(detail::make_expand_fn(std::move(std::declval<Fn>()), detail::make_index_sequence<sizeof...(Iterables)>()))>;

/**
 * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
 * Signature must look like: `fn(value_type_tIterable1[, value_type_tIterable2[, value_type_tIterable3[, value_type_tIterable-n[,
 * ...]]]])`
 * @param fn The function to apply to each elements in all containers.
 * @param iterables The iterables.
 * @return A map_iterable<zip_iterable> object that applies fn over each expanded tuple elements from [begin, end)
 */
template<class Fn, class... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 zip_with_iterable<Fn, Iterables...> zip_with(Fn fn, Iterables&&... iterables) {
    auto zipper = lz::zip(std::forward<Iterables>(iterables)...);
    auto tuple_expander_func = detail::make_expand_fn(std::move(fn), detail::make_index_sequence<sizeof...(Iterables)>());
    return lz::map(std::move(zipper), std::move(tuple_expander_func));
}

/**
 * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
 * @param iterable A container/iterable.
 * @return A zip iterator that accesses two adjacent elements of one container.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 zip_iterable<iter_t<Iterable>, iter_t<Iterable>> pairwise(Iterable&& iterable) {
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::get_fn<1>>
values(Iterable&& iterable) {
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::get_fn<0>>
keys(Iterable&& iterable) {
    return lz::map(std::forward<Iterable>(iterable), detail::get_fn<0>());
}

template<class Iterable, class UnaryFiltePredicate, class UnaryMapOp>
using filter_map_iterable =
    map_iterable<decltype(lz::filter(std::forward<Iterable>(std::declval<Iterable>()), std::declval<UnaryFiltePredicate>()).begin()),
        decltype(lz::filter(std::forward<Iterable>(std::declval<Iterable>()), std::declval<UnaryFiltePredicate>()).end()),
        UnaryMapOp>;

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param iterable The iterable to filter/map.
 * @param filter_predicate The function that filters the elements. If this function returns `true`, its corresponding container
 * value is passed to the `map_op`.
 * @param map_op The function that returns the (new) type.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `map_op`.
 */
template<class Iterable, class UnaryFiltePredicate, class UnaryMapOp>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 filter_map_iterable<Iterable, UnaryMapOp, UnaryFiltePredicate>
filter_map(Iterable&& iterable, UnaryFiltePredicate filter_predicate, UnaryMapOp map_op) {
    return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(filter_predicate)), std::move(map_op));
}

// TODO: make typedef that works here for selector using std::tuple
// template<class Iterable, class SelectorIterable>
// using Selector = FilterMap<zip_iterable<iter_t<Iterable>, sentinel_t<Iterable>, iter_t<SelectorIterable>>, detail::get_fn<1>,
// detail::get_fn<0>>;

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param iterable The iterable to select values from.
 * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<class Iterable, class SelectorIterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto select(Iterable&& iterable, SelectorIterable&& selectors)
    -> decltype(filter_map(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::get_fn<1>(),
                           detail::get_fn<0>())) {
    return filter_map(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::get_fn<1>(),
                      detail::get_fn<0>());
}

// TODO trim_front and/or trim_back?

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long
 * as `last` returns true for the trimming of the ending.
 * @param iterable The iterable to trim
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @return An iterator view object.
 */
template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<std::reverse_iterator<std::reverse_iterator<iter_t<Iterable>>>>
trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
    auto taken_first = lz::drop_while(std::forward<Iterable>(iterable), std::move(first));
    auto taken_last = lz::drop_while(lz::reverse(std::move(taken_first)), std::move(last));
    return lz::reverse(std::move(taken_last));
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param s The string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
template<class String>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<std::reverse_iterator<std::reverse_iterator<iter_t<String>>>>
trim_string(String&& s) {
    const auto is_space_fn = [](const char c) {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    };
    return lz::trim(std::forward<String>(s), is_space_fn, is_space_fn);
}

} // End namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // End LZ_FUNCTION_TOOLS_HPP