#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include "Lz/detail/compiler_checks.hpp"

#include <array> // tuple_element
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace lz {
struct default_sentinel;

namespace detail {

template<class>
struct always_false : std::false_type {};

#ifdef LZ_HAS_CXX_11

#define MAKE_OPERATOR(OP, VALUE_TYPE) OP<VALUE_TYPE>

template<std::size_t...>
struct index_sequence {};

template<std::size_t N, std::size_t... Rest>
struct index_sequence_helper : public index_sequence_helper<N - 1, N - 1, Rest...> {};

template<std::size_t... Next>
struct index_sequence_helper<0, Next...> {
    using type = index_sequence<Next...>;
};

template<std::size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;

template<class T>
using decay = typename std::decay<T>::type;

template<std::size_t I, class T>
using tuple_element = typename std::tuple_element<I, T>::type;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<VALUE_TYPE>
#else // ^^^ has cxx 11 vvv cxx > 11
template<std::size_t... N>
using index_sequence = std::index_sequence<N...>;

template<std::size_t N>
using make_index_sequence = std::make_index_sequence<N>;

template<class T>
using decay = std::decay_t<T>;

template<std::size_t I, class T>
using tuple_element = std::tuple_element_t<I, T>;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<>

#endif // LZ_HAS_CXX_11

template<class Iterable>
constexpr auto begin(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin());

template<class Iterable>
constexpr auto end(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin());

template<class T, size_t N>
constexpr T* begin(T (&array)[N]) noexcept;

template<class T, size_t N>
constexpr T* end(T (&array)[N]) noexcept;

template<class T, size_t N>
constexpr const T* begin(const T (&array)[N]) noexcept;

template<class T, size_t N>
constexpr const T* end(const T (&array)[N]) noexcept;

} // namespace detail

template<class Iterable>
using iter = decltype(detail::begin(std::forward<Iterable>(std::declval<Iterable>())));

template<class S>
using sentinel = decltype(detail::end(std::forward<S>(std::declval<S>())));

template<class Iterator>
using value_type = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::value_type;

template<class Iterator>
using ref = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::reference;

template<class Iterator>
using pointer_t = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::pointer;

template<class Iterator>
using diff_type = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::difference_type;

template<class Iterator>
using iter_cat = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::iterator_category;

template<class Iterable>
using value_type_iterable = typename std::iterator_traits<iter<Iterable>>::value_type;

template<class Iterable>
using diff_iterable = typename std::iterator_traits<iter<Iterable>>::difference_type;

namespace detail {

template<class Function, class... Args>
using func_ret_type = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class... Ts>
using common_type = typename std::common_type<Ts...>::type;

template<bool B>
struct enable_if_impl {};

template<>
struct enable_if_impl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using enable_if = typename enable_if_impl<B>::template type<T>;

template<bool B>
struct conditional_impl;

template<>
struct conditional_impl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct conditional_impl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using conditional = typename conditional_impl<B>::template type<IfTrue, IfFalse>;

template<class T, class U, class... Vs>
struct is_all_same : std::bool_constant<std::is_same<T, U>::value && is_all_same<U, Vs...>::value> {};

template<class T, class U>
struct is_all_same<T, U> : std::is_same<T, U> {};

template<class IterTag>
struct is_bidi_tag : std::is_convertible<IterTag, std::bidirectional_iterator_tag> {};

template<class Iterator>
struct is_bidi : is_bidi_tag<iter_cat<Iterator>> {};

template<class Iterator>
struct is_fwd : std::is_convertible<iter_cat<Iterator>, std::forward_iterator_tag> {};

template<class IterTag>
struct is_ra : std::is_convertible<IterTag, std::random_access_iterator_tag> {};

template<class Iterator>
struct is_ra : is_ra<iter_cat<Iterator>> {};

template<class Iterable>
struct actual_sentinel : std::bool_constant<!std::is_same<iter<Iterable>, sentinel<Iterable>>::value> {};

} // namespace detail

/**
 * @brief Selects @p S if @p Tag is a `std::forward_iterator_tag`, otherwise selects @p Iterator.
 *
 * @tparam Tag The iterator tag to check.
 * @tparam Iterator The iterator type to select if @p Tag is not a `std::forward_iterator_tag`.
 * @tparam S The sentinel type to select if @p Tag is a `std::forward_iterator_tag`.
 */
template<class Tag, class Iterator, class S = default_sentinel>
using sentinel_selector = detail::conditional<std::is_same<Tag, std::forward_iterator_tag>::value, S, Iterator>;

/**
 * @brief Is @p TagFrom convertible to @p TagTo? If so, return @p TagFrom, otherwise return @p ToDecay.
 *
 * @tparam TagFrom The iterator tag to convert from.
 * @tparam TagTo The iterator tag to convert to.
 * @tparam ToDecay The type to decay to if @p TagFrom is not convertible to @p TagTo.
 */
template<class TagFrom, class TagTo, class ToDecay>
using iter_cat_decay = detail::conditional<std::is_convertible<TagFrom, TagTo>::value, TagFrom, ToDecay>;
} // namespace lz

#endif // LZ_LZ_TOOLS_HPP