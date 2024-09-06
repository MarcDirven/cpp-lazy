#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include "Lz/detail/CompilerChecks.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <array> // tuple_element

namespace lz {
namespace detail {

template<class>
struct AlwaysFalse : std::false_type {};

#ifdef LZ_HAS_CXX_11

#define MAKE_OPERATOR(OP, VALUE_TYPE) OP<VALUE_TYPE>

template<std::size_t...>
struct IndexSequence {};

template<std::size_t N, std::size_t... Rest>
struct IndexSequenceHelper : public IndexSequenceHelper<N - 1, N - 1, Rest...> {};

template<std::size_t... Next>
struct IndexSequenceHelper<0, Next...> {
    using Type = IndexSequence<Next...>;
};

template<std::size_t N>
using MakeIndexSequence = typename IndexSequenceHelper<N>::Type;

template<class T>
using Decay = typename std::decay<T>::type;

template<std::size_t I, class T>
using TupleElement = typename std::tuple_element<I, T>::type;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<VALUE_TYPE>
#else // ^^^ has cxx 11 vvv cxx > 11
template<std::size_t... N>
using IndexSequence = std::index_sequence<N...>;

template<std::size_t N>
using MakeIndexSequence = std::make_index_sequence<N>;

template<class T>
using Decay = std::decay_t<T>;

template<std::size_t I, class T>
using TupleElement = std::tuple_element_t<I, T>;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<>

#endif // LZ_HAS_CXX_11

template<class Iterable>
using IterTypeFromIterable = decltype(std::begin(std::forward<Iterable>(std::declval<Iterable>())));

template<class Iterator>
using ValueType = typename std::iterator_traits<Iterator>::value_type;

template<class Iterator>
using RefType = typename std::iterator_traits<Iterator>::reference;

template<class Iterator>
using PointerType = typename std::iterator_traits<Iterator>::pointer;

template<class Iterator>
using DiffType = typename std::iterator_traits<Iterator>::difference_type;

template<class Iterator>
using IterCat = typename std::iterator_traits<Iterator>::iterator_category;

template<class Function, class... Args>
using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class Iterable>
using ValueTypeIterable = typename std::iterator_traits<IterTypeFromIterable<Iterable>>::value_type;

template<class Iterable>
using DiffTypeIterable = typename std::iterator_traits<IterTypeFromIterable<Decay<Iterable>>>::difference_type;

template<bool B>
struct EnableIfImpl {};

template<>
struct EnableIfImpl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using EnableIf = typename EnableIfImpl<B>::template type<T>;

template<bool B>
struct ConditionalImpl;

template<>
struct ConditionalImpl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct ConditionalImpl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using Conditional = typename ConditionalImpl<B>::template type<IfTrue, IfFalse>;

template<class T, class U, class... Vs>
struct IsAllSame : std::integral_constant<bool, std::is_same<T, U>::value && IsAllSame<U, Vs...>::value> {};

template<class T, class U>
struct IsAllSame<T, U> : std::is_same<T, U> {};

template<class IterTag>
struct IsBidirectionalTag : std::is_convertible<IterTag, std::bidirectional_iterator_tag> {};

template<class Iterator>
struct IsBidirectional : IsBidirectionalTag<IterCat<Iterator>> {};

template<class Iterator>
struct IsForward : std::is_convertible<IterCat<Iterator>, std::forward_iterator_tag> {};

template<class IterTag>
struct IsRandomAccessTag : std::is_convertible<IterTag, std::random_access_iterator_tag> {};

template<class Iterator>
struct IsRandomAccess : IsRandomAccessTag<IterCat<Iterator>> {};

template<typename...>
using Voidify = void;

template<typename...>
using void_t = void;

template<typename, template<typename> class, typename = Voidify<>>
struct Detector : std::false_type {};

template<typename T, template<typename> class Op>
struct Detector<T, Op, Voidify<Op<T>>> : std::true_type {};

template<typename T>
using BeginFunc = decltype(std::begin(std::declval<T>()));

template<typename T>
using HasBeginFunc = Detector<T, BeginFunc>;

} // namespace detail
} // namespace lz

#endif // LZ_LZ_TOOLS_HPP