#pragma once

#ifndef LZ_CONCEPTS_HPP
#define LZ_CONCEPTS_HPP

#include "Lz/detail/compiler_checks.hpp"

#ifdef LZ_HAS_CONCEPTS
#include <concepts>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class I>
concept basic_iterable = requires(I i) {
    { std::begin(i) } -> std::input_or_output_iterator;
    { std::end(i) } -> std::input_or_output_iterator;
};

template<class I>
concept bidirectional_iterable = requires(I i) {
    { std::begin(i) } -> std::bidirectional_iterator;
    { std::end(i) } -> std::bidirectional_iterator;
};

template<class I>
concept arithmetic = std::is_arithmetic_v<I>;

LZ_MODULE_EXPORT_SCOPE_END

} // End namespace lz

#define LZ_CONCEPT_ARITHMETIC lz::arithmetic
#define LZ_CONCEPT_INTEGRAL std::integral
#define LZ_CONCEPT_INVOCABLE std::invocable
#define LZ_CONCEPT_ITERABLE lz::basic_iterable
#define LZ_CONCEPT_ITERATOR std::input_or_output_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR std::bidirectional_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE lz::bidirectional_iterable

#else // ^^^ has concepts !has concepts vvv
#define LZ_CONCEPT_ARITHMETIC class
#define LZ_CONCEPT_INTEGRAL class
#define LZ_CONCEPT_ITERATOR class
#define LZ_CONCEPT_INVOCABLE class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR class
#define LZ_CONCEPT_ITERABLE class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class
#endif // LZ_HAS_CONCEPTSss

#endif // LZ_CONCEPTS_HPP