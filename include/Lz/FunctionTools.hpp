#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#    define LZ_FUNCTION_TOOLS_HPP

#    include "Concatenate.hpp"
#    include "Filter.hpp"
#    include "Join.hpp"
#    include "Map.hpp"
#    include "StringSplitter.hpp"
#    include "Take.hpp"
#    include "Zip.hpp"

#    include <algorithm>
#    include <cctype>
#    include <iterator>
#    include <numeric>

#    ifdef LZ_HAS_CXX_17
#        define LZ_INLINE_VAR inline
#    else // ^^^ has cxx 17 vvv !has cxx 17
#        define LZ_INLINE_VAR
#    endif // lz has cxx17

namespace lz {
namespace internal {
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

template<class Fn, std::size_t... I>
struct TupleExpand {
    FunctionContainer<Fn> fn{};

    constexpr TupleExpand() = default;

    explicit constexpr TupleExpand(Fn fn) : fn(std::move(fn)) {
    }

    template<class Tuple>
    constexpr auto operator()(Tuple&& tuple) -> decltype(fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }
};

template<class Fn, std::size_t... I>
constexpr TupleExpand<Fn, I...> makeExpandFn(Fn fn, IndexSequence<I...>) {
    return TupleExpand<Fn, I...>(std::move(fn));
}

#    ifdef __cpp_if_constexpr
template<class Iterator>
RefType<Iterator> lastImpl(Iterator begin, Iterator end) {
    if constexpr (std::is_convertible_v<IterCat<Iterator>, std::bidirectional_iterator_tag>) {
        static_cast<void>(begin);
        return *--end;
    }
    else {
        using lz::distance;
        using lz::next;
        using std::distance;
        using std::next;
        return *next(begin, distance(begin, end) - 1);
    }
}
#    else
template<class Iterator>
LZ_CONSTEXPR_CXX_20
internal::EnableIf<std::is_convertible<IterCat<Iterator>, std::bidirectional_iterator_tag>::value, RefType<Iterator>>
lastImpl(Iterator, Iterator end) {
    return *--end;
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20
internal::EnableIf<!std::is_convertible<IterCat<Iterator>, std::bidirectional_iterator_tag>::value, RefType<Iterator>>
lastImpl(Iterator begin, Iterator end) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    return *next(begin, distance(begin, end) - 1);
}
#    endif // __cpp_if_constexpr
} // namespace internal

/**
 * This value is returned when indexOf(If) does not find the value specified.
 */
constexpr LZ_INLINE_VAR std::size_t npos = (std::numeric_limits<std::size_t>::max)();

/**
 * Returns a StringSplitter iterator, that splits the string on `'\n'`.
 * @tparam SubString The string type that the `StringSplitter::value_type` must return. Must either be std::string or
 * std::string_view.
 * @tparam String The string type. `std::string` is assumed but can be specified.
 * @param string The string to split on.
 * @return Returns a StringSplitter iterator, that splits the string on `'\n'`.
 */
template<class SubString = std::string, class String = std::string>
LZ_NODISCARD StringSplitter<SubString, String, char> lines(const String& string) {
    return split<SubString>(string, '\n');
}

/**
 * Sums all the values from [from, upToAndIncluding]
 * @tparam T An integral value.
 * @param from The start to sum from
 * @param upToAndIncluding The end of the sum
 * @return The result of the sum from [from, upToAndIncluding]
 */
template<class T>
LZ_NODISCARD constexpr T sumTo(const T from, const T upToAndIncluding) {
    static_assert(std::is_integral<T>::value, "T must be integral type");
    const T fromAbs = from < 0 ? -from : from;
    const T toAbs = upToAndIncluding < 0 ? -upToAndIncluding : upToAndIncluding;
    const T error = ((fromAbs - 1) * fromAbs) / 2;
    const T sum = (toAbs * (toAbs + 1)) / 2;
    if (upToAndIncluding < 0) {
        return error - sum;
    }
    if (from < 0) {
        return -(error + fromAbs - sum);
    }
    LZ_ASSERT(from < upToAndIncluding, "`from` cannot be smaller than `upToAndIncluding` if both are positive");
    return sum - error;
}

/**
 * Sums all the values from [0, upToAndIncluding]
 * @tparam T An integral value.
 * @param upToAndIncluding The end of the sum
 * @return The result of the sum from [0, upToAndIncluding]
 */
template<class T>
LZ_NODISCARD constexpr T sumTo(const T upToAndIncluding) {
    return sumTo(0, upToAndIncluding);
}

/**
 * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
 * @param strings The container of `std::string` or `std::string_view`.
 * @return A Join iterator that joins the strings in the container on `'\n'`.
 */
template<class Strings>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<internal::IterTypeFromIterable<Strings>> unlines(Strings&& strings) {
    return join(strings, "\n");
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param begin The beginning of the sequence. Must have at least std::bidirectional_iterator_tag.
 * @param end The ending of the sequence. Must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<std::reverse_iterator<Iterator>>
reverse(Iterator begin, Iterator end) {
#    ifndef LZ_HAS_CONCEPTS
    static_assert(internal::IsBidirectional<Iterator>::value, "the type of the iterator must be bidirectional or stronger");
#    endif // LZ_HAS_CONCEPTS
#    ifdef LZ_HAS_CXX_11
    return internal::BasicIteratorView<std::reverse_iterator<Iterator>>(std::reverse_iterator<Iterator>(std::move(end)),
                                                                        std::reverse_iterator<Iterator>(std::move(begin)));
#    else
    return internal::BasicIteratorView<std::reverse_iterator<Iterator>>(std::make_reverse_iterator(std::move(end)),
                                                                        std::make_reverse_iterator(std::move(begin)));
#    endif // LZ_HAS_CXX_11
}

/**
 * Returns a view object of which its iterators are reversed.
 * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
 * @return A Take view object contains the reverse order of [begin end)
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<std::reverse_iterator<internal::IterTypeFromIterable<Iterable>>>
reverse(Iterable&& iterable) {
    // ADL std::reverse
    return lz::reverse(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(...begin(), ...end())` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, internal::ConvertFn<T>> as(Iterator begin, Iterator end) {
    return mapRange(std::move(begin), std::move(end), internal::ConvertFn<T>());
}

/**
 * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
 * floating points from the given values in the container.
 * @tparam T The type to construct from the elements in the given container
 * @tparam Iterable A container/iterable.
 * @return A map iterator that constructs To from each of the elements in the given container.
 */
template<class T, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<internal::IterTypeFromIterable<Iterable>, internal::ConvertFn<T>> as(Iterable&& iterable) {
    return as<T>(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
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
           decltype(internal::makeExpandFn(std::move(fn), internal::MakeIndexSequence<sizeof...(Iterators)>()))> {
    Zipper zipper = zipRange(std::move(begin), std::move(end));
    auto tupleExpanderFunc = internal::makeExpandFn(std::move(fn), internal::MakeIndexSequence<sizeof...(Iterators)>());
    return map(std::move(zipper), std::move(tupleExpanderFunc));
}

/**
 * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
 * Signature must look like: `fn(ValueTypeIterable1[, ValueTypeIterable2[, ValueTypeIterable3[, ValueTypeIterable-n[, ...]]]])`
 * @param fn The function to apply to each elements in all containers.
 * @param iterables The iterables.
 * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
 */
template<class Fn, class... Iterables, class Zipper = Zip<internal::IterTypeFromIterable<Iterables>...>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zipWith(Fn fn, Iterables&&... iterables)
    -> Map<decltype(std::begin(std::declval<Zipper>())),
           decltype(internal::makeExpandFn(fn, internal::MakeIndexSequence<sizeof...(Iterables)>()))> {
    return zipWith(std::move(fn), std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                   std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
}

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool isEmpty(const Iterator begin, const Iterator end) {
    return begin == end;
}

/**
 * Checks whether `iterable` is empty.
 * @param iterable The sequence to check whether it is empty yes/no.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool isEmpty(const Iterable& iterable) {
    return isEmpty(std::begin(iterable), std::end(iterable));
}

/**
 * Returns whether the sequence holds exactly one element.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it has one element exactly, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasOne(Iterator begin, const Iterator end) {
    return !isEmpty(begin, end) && ++begin == end;
}

/**
 * Returns whether the sequence holds exactly one element.
 * @param iterable The sequence to check whether is has exactly one element.
 * @return True if it has one element exactly, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasOne(const Iterable& iterable) {
    return hasOne(std::begin(iterable), std::end(iterable));
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasMany(Iterator begin, Iterator end) {
    return !isEmpty(begin, end) && !hasOne(begin, end);
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param iterable The sequence to check whether it has many (>= 2) elements.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasMany(const Iterable& iterable) {
    return !isEmpty(iterable) && !hasOne(iterable);
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<Iterator> first(Iterator begin, Iterator end) {
    LZ_ASSERT(!isEmpty(begin, end), "sequence cannot be empty in order to get the first element");
    static_cast<void>(end);
    return *begin;
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param iterable The sequence to get the first element of.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<internal::IterTypeFromIterable<Iterable>> first(Iterable&& iterable) {
    return first(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<Iterator> last(Iterator begin, Iterator end) {
    LZ_ASSERT(!isEmpty(begin, end), "sequence cannot be empty in order to get the last element");
    return internal::lastImpl(std::move(begin), std::move(end));
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @param iterable The sequence to get the last element of.
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<internal::IterTypeFromIterable<Iterable>> last(Iterable&& iterable) {
    return last(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the first element, or `defaultValue` if [begin, end) is empty
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence.
 * @param defaultValue The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator> firstOr(Iterator begin, Iterator end, const T& defaultValue) {
    return isEmpty(begin, end) ? static_cast<internal::ValueType<Iterator>>(defaultValue) : first(begin, end);
}

/**
 * Gets the first element of the sequence, or `defaultValue` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param defaultValue The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable> firstOr(const Iterable& iterable, const T& defaultValue) {
    return firstOr(std::begin(iterable), std::end(iterable), defaultValue);
}

/**
 * This function returns the last element. If the sequence is empty, it returns `value`.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence.
 * @param value The value to return if `iterable` is empty.
 * @return Either the last element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator> lastOr(Iterator begin, Iterator end, const T& value) {
    return isEmpty(begin, end) ? static_cast<internal::ValueType<Iterator>>(value) : last(begin, end);
}

/**
 * This function returns the last element. If the sequence is empty, it returns `value`.
 * @param iterable The iterable to get the last value of, or `value` in case it is empty.
 * @param value The value to return if `iterable` is empty.
 * @return Either the last element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable> lastOr(const Iterable& iterable, const T& value) {
    return lastOr(std::begin(iterable), std::end(iterable), value);
}

/**
 * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @return A zip iterator that accesses two adjacent elements of one container.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<Iterator, Iterator> pairwise(Iterator begin, Iterator end) {
    LZ_ASSERT(hasMany(begin, end), "length of the sequence must be greater than or equal to 2");
    auto start = begin++;
    return zipRange(std::make_tuple(std::move(start), std::move(begin)), std::make_tuple(end, end));
}

/**
 * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
 * @param iterable A container/iterable.
 * @return A zip iterator that accesses two adjacent elements of one container.
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERATOR Iterator = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<Iterator, Iterator> pairwise(Iterable&& iterable) {
    return pairwise(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns an iterator object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
 * Useful for example getting the values from a `std::(unordered_)map`
 * @param begin The beginning of the iterator.
 * @param end The ending of the iterator.
 * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, internal::GetFn<1>> valuesRange(Iterator begin, Iterator end) {
    return mapRange(std::move(begin), std::move(end), internal::GetFn<1>());
}

/**
 * Returns an iterator object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
 * Useful for example getting the values from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<internal::IterTypeFromIterable<Iterable>, internal::GetFn<1>> values(Iterable&& iterable) {
    return valuesRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, internal::GetFn<0>> keysRange(Iterator begin, Iterator end) {
    return mapRange(std::move(begin), std::move(end), internal::GetFn<0>());
}

/**
 * Returns an iterator object that returns all keys from an iterable of which its `value_type` supports `std::get<0>`.
 * Useful for example getting the keys from a `std::(unordered_)map`
 * @param iterable The iterable to extract the elements from.
 * @return An iterable object that returns the first value of `value_type` using `std::get<0>`. (works for any tuple-like
 * interface)`
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<internal::IterTypeFromIterable<Iterable>, internal::GetFn<0>> keys(Iterable&& iterable) {
    return keysRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

#    ifdef LZ_HAS_EXECUTION
/**
 * Gets the mean of a sequence.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param execution The execution policy.
 * @return The mean of the sequence.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class BinaryOp = std::plus<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double
mean(Iterator begin, Iterator end, BinaryOp binaryOp = {}, Execution execution = std::execution::seq) {
    using ValueType = internal::ValueType<Iterator>;
    using lz::distance;
    using std::distance;
    const internal::DiffType<Iterator> dist = distance(begin, end);
    ValueType sum;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        sum = std::reduce(begin, end, ValueType{ 0 }, std::move(binaryOp));
    }
    else {
        sum = std::reduce(execution, begin, end, ValueType{ 0 }, std::move(binaryOp));
    }
    return static_cast<double>(sum) / dist;
}

/**
 * Gets the mean of a sequence.
 * @param container The container to calculate the mean of.
 * @param execution The execution policy
 * @return The mean of the container.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryOp = std::plus<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double
mean(const Iterable& container, BinaryOp binOp = {}, Execution execution = std::execution::seq) {
    return mean(std::begin(container), std::end(container), std::move(binOp), execution);
}

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
 * passed to the `mapFunc`.
 * @param mapFunc The function that returns the (new) type.
 * @param execPolicy TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `mapFunc`.
 */
template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc,
         LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>
filterMap(Iterator begin, Iterator end, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc,
          Execution execPolicy = std::execution::seq) {
    auto filterView = filterRange(std::move(begin), std::move(end), std::move(filterFunc), execPolicy);
    return map(std::move(filterView), std::move(mapFunc));
}

// clang-format off

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param iterable The iterable to filter/map.
 * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
 * passed to the `mapFunc`.
 * @param mapFunc The function that returns the (new) type.
 * @param execution The execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `mapFunc`.
 */
template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc,
         LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
Map<internal::FilterIterator<internal::IterTypeFromIterable<Iterable>, UnaryFilterFunc, Execution>, UnaryMapFunc>
filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc, Execution execution = std::execution::seq) {
    return filterMap(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                     std::move(filterFunc), std::move(mapFunc), execution);
}

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param begin The iterator beginning to select values from.
 * @param end The iterator ending to select values from.
 * @param beginSelector The selector beginning iterator that specifies to select an item in [begin, end) yes or no.
 * @param endSelector The selector ending iterator that specifies to select an item in [begin, end) yes or no.
 * @param execution The `std::execution` policy.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR SelectorIterator, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>, internal::GetFn<1>, Execution>,
    internal::GetFn<0>>
select(Iterator begin, Iterator end, SelectorIterator beginSelector, SelectorIterator endSelector,
           Execution execution = std::execution::seq) {
    auto zipper = zipRange(std::make_tuple(std::move(begin), std::move(beginSelector)),
                               std::make_tuple(std::move(end), std::move(endSelector)));
    return filterMap(std::move(zipper), internal::GetFn<1>(), internal::GetFn<0>(), execution);
}

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param iterable The iterable to select values from.
 * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
 * @param execution The `std::execution` policy.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE SelectorIterable, class Execution = std::execution::sequenced_policy,
         class Iterator = internal::IterTypeFromIterable<Iterable>,
         class SelectorIterator = internal::IterTypeFromIterable<SelectorIterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>, internal::GetFn<1>, Execution>,
    internal::GetFn<0>>
select(Iterable&& iterable, SelectorIterable&& selectors, Execution execution = std::execution::seq) {
    return select(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                  internal::begin(std::forward<SelectorIterable>(selectors)),
                  internal::end(std::forward<SelectorIterable>(selectors)), execution);
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long as
 * `last` returns true for the trimming of the ending.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @param execution The execution policy. Must be of type std::execution::*
 * @return An iterator view object.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator, class UnaryPredicateFirst, class UnaryPredicateLast,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<Iterator>>>
trim(Iterator begin, Iterator end, UnaryPredicateFirst first, UnaryPredicateLast last,
     Execution execution = std::execution::seq) {
    auto takenFirst = dropWhileRange(std::move(begin), std::move(end), std::move(first), execution);
    auto takenLast = dropWhile(std::move(lz::reverse(takenFirst)), std::move(last), execution);
    return lz::reverse(std::move(takenLast));
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long as
 * `last` returns true for the trimming of the ending.
 * @param iterable The iterable to trim
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @param execution The execution policy. Must be of type std::execution::*
 * @return An iterator view object.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable, class UnaryPredicateFirst, class UnaryPredicateLast,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<internal::IterTypeFromIterable<Iterable>>>>
trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last, Execution execution = std::execution::seq) {
    return trim(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                std::move(first), std::move(last), execution);
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param begin The beginning of the string to trim.
 * @param end The ending of the string to trim.
 * @param execution The execution policy. Must be of `std::execution::*` tags.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
template<class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(std::string::const_iterator begin, std::string::const_iterator end, Execution execution = std::execution::seq) {
    const auto isSpaceFn = [](const char c) {
        return std::isspace(static_cast<unsigned char>(c));
    };
    return trim(begin, end, isSpaceFn, isSpaceFn, execution);
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param s The string to trim.
 * @param execution The execution policy. Must be of `std::execution::*` tags.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
template<class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(const std::string& s, Execution execution = std::execution::seq) {
    return trimString(s.begin(), s.end(), execution);
}

// clang-format on

/**
 * Gets the median of a sequence.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @param comparer The sequence gets sorted with nth_element. A default operator of < is used, however a custom comparer can be
 * used.
 * @param execution Uses the execution to perform the nth_element algorithm + the std::max element.
 * @return The median of the sequence.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Comparer = std::less<>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double
median(Iterator begin, Iterator end, Comparer comparer = {}, Execution execution = std::execution::seq) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    const internal::DiffType<Iterator> len = distance(begin, end);
    constexpr bool isSequenced = internal::checkForwardAndPolicies<Execution, Iterator>();
    LZ_ASSERT(len > 0, "the length of the sequence cannot be 0");
    const internal::DiffType<Iterator> mid = len / 2;
    const Iterator midIter = next(begin, mid);
    if constexpr (isSequenced) {
        static_cast<void>(execution);
        std::nth_element(begin, midIter, end, comparer);
    }
    else {
        std::nth_element(execution, begin, midIter, end, comparer);
    }
    if (internal::isEven(len)) {
        if constexpr (isSequenced) {
            const Iterator leftHalf = std::max_element(begin, midIter, comparer);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
        else {
            const Iterator leftHalf = std::max_element(execution, begin, midIter, comparer);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
    }
    return *midIter;
}

/**
 * Gets the median of a sequence.
 * @param iterable The container/sequence by reference.
 * @param comparer The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be
 * used.
 * @param execution Uses the execution to perform the nth_element algorithm + the std::max element.
 * @return The median of the sequence.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class Comparer = std::less<>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double
median(Iterable& iterable, Comparer comparer = {}, Execution execution = std::execution::seq) {
    return median(std::begin(iterable), std::end(iterable), std::move(comparer), execution);
}

/**
 * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param toFind The value to find.
 * @param defaultValue The value to return if `toFind` is not found.
 * @param execution Uses the execution to perform the find.
 * @return Either `toFind` or `defaultValue`.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class T, class U>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator>
findFirstOrDefault(Iterator begin, Iterator end, const T& toFind, const U& defaultValue,
                   Execution execution = std::execution::seq) {
    using ValueType = internal::ValueType<Iterator>;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        return static_cast<ValueType>(std::find(std::move(begin), end, toFind) == end ? defaultValue : toFind);
    }
    else {
        return static_cast<ValueType>(std::find(execution, std::move(begin), end, toFind) == end ? defaultValue : toFind);
    }
}

/**
 * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
 * @param iterable The iterable to search.
 * @param toFind The value to find.
 * @param defaultValue The value to return if `toFind` is not found.
 * @param execution Uses the execution to perform the find.
 * @return Either `toFind` or `defaultValue`.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T, class U>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable>
findFirstOrDefault(const Iterable& iterable, const T& toFind, const U& defaultValue, Execution execution = std::execution::seq) {
    return findFirstOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
}

/**
 * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its
 * corresponding value is returned. If not, `defaultValue` is returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
 * (see `std::find_if`)
 * @param defaultValue The default value to return if predicate is `false` for all elements.
 * @param execution Uses the execution to perform the find.
 * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator>
findFirstOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, const T& defaultValue,
                     Execution execution = std::execution::seq) {
    using ValueType = internal::ValueType<Iterator>;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        const Iterator pos = std::find_if(std::move(begin), end, std::move(predicate));
        return static_cast<ValueType>(pos == end ? defaultValue : *pos);
    }
    else {
        const Iterator pos = std::find_if(execution, std::move(begin), end, std::move(predicate));
        return static_cast<ValueType>(pos == end ? defaultValue : *pos);
    }
}

/**
 * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its
 * corresponding value is returned. If not, `defaultValue` is returned.
 * @param iterable The sequence to search.
 * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
 * (see `std::find_if`)
 * @param defaultValue The default value to return if predicate is `false` for all elements.
 * @param execution Uses the execution to perform the find.
 * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable>
findFirstOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate, const T& defaultValue,
                     Execution execution = std::execution::seq) {
    return findFirstOrDefaultIf(std::begin(iterable), std::end(iterable), std::move(predicate), defaultValue, execution);
}

/**
 * Searches for the last occurrence for `toFind` in (end, begin]. If no such such element exists, `defaultValue` is returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param toFind The value to find.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence of `toFind` in [begin, end) or `defaultValue` if no such element exists.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator, class T, class U, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator>
findLastOrDefault(Iterator begin, Iterator end, const T& toFind, const U& defaultValue,
                  Execution execution = std::execution::seq) {
    using CastType = internal::ValueType<Iterator>;
    auto endReverse = std::make_reverse_iterator(std::move(end));
    auto beginReverse = std::make_reverse_iterator(std::move(begin));
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        const auto pos = std::find(std::move(endReverse), beginReverse, toFind);
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }
    else {
        const auto pos = std::find(execution, std::move(endReverse), beginReverse, toFind);
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }
}

/**
 * Searches for the last occurrence for `toFind` in `iterable`. If no such such element exists, `defaultValue` is returned.
 * @param iterable The sequence to search.
 * @param toFind The value to find.
 * @param defaultValue The value to return when no element `toFind` exists in `iterable`
 * @return Either the last occurrence of `toFind` in `iterable` or `defaultValue` if no such element exists.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable, class T, class U, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable>
findLastOrDefault(const Iterable& iterable, const T& toFind, const U& defaultValue, Execution execution = std::execution::seq) {
    return findLastOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
}

/**
 * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is
 * returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate The search predicate in [begin, end). Must return bool.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element
 * exists.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator, class T, class UnaryPredicate,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueType<Iterator>
findLastOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, const T& defaultValue,
                    Execution execution = std::execution::seq) {
    using CastType = internal::ValueType<Iterator>;
    auto endReverse = std::make_reverse_iterator(std::move(end));
    auto beginReverse = std::make_reverse_iterator(std::move(begin));
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        const auto pos = std::find_if(std::move(endReverse), beginReverse, std::move(predicate));
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }
    else {
        const auto pos = std::find_if(execution, std::move(endReverse), beginReverse, std::move(predicate));
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }
}

/**
 * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is
 * returned.
 * @param iterable The sequence to search.
 * @param predicate The search predicate in [begin, end). Must return bool.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element
 * exists.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable, class T, class UnaryPredicate,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ValueTypeIterable<Iterable>
findLastOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate, const T& defaultValue,
                    Execution execution = std::execution::seq) {
    return findLastOrDefaultIf(std::begin(iterable), std::end(iterable), std::move(predicate), defaultValue, execution);
}

/**
 * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param val The value to search.
 * @param execution Uses the execution to perform the find.
 * @return The index of `val` or lz::npos of no such value exists.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::size_t
indexOf(Iterator begin, Iterator end, const T& val, Execution execution = std::execution::seq) {
    using lz::distance;
    using std::distance;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        const Iterator pos = std::find(begin, end, val);
        return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
    }
    else {
        const Iterator pos = std::find(execution, begin, end, val);
        return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
    }
}

/**
 * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
 * @param iterable The iterable to search.
 * @param val The value to search.
 * @param execution Uses the execution to perform the find.
 * @return The index of `val` or lz::npos of no such value exists.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::size_t
indexOf(const Iterable& iterable, const T& val, Execution execution = std::execution::seq) {
    return indexOf(std::begin(iterable), std::end(iterable), val, execution);
}

/**
 * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value
 * exists.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate The search predicate. Uses `std::find_if`.
 * @param execution Uses the execution to perform the find.
 * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::size_t
indexOfIf(Iterator begin, Iterator end, UnaryFunc predicate, Execution execution = std::execution::seq) {
    using lz::distance;
    using std::distance;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        const Iterator pos = std::find_if(begin, end, std::move(predicate));
        return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
    }
    else {
        const Iterator pos = std::find_if(execution, begin, end, std::move(predicate));
        return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
    }
}

/**
 * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
 * @param iterable The sequence to search.
 * @param predicate The search predicate. Uses `std::find_if`.
 * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::size_t
indexOfIf(const Iterable& iterable, UnaryFunc predicate, Execution execution = std::execution::seq) {
    return indexOfIf(std::begin(iterable), std::end(iterable), std::move(predicate), execution);
}

/**
 * Check whether `iterable` contains `value`.
 * @param begin The beginning of the sequence to search.
 * @param end The ending of the sequence to search.
 * @param value The value to find.
 * @return True if `iterable` contains `value`, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class T, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
contains(Iterator begin, Iterator end, const T& value, Execution execution = std::execution::seq) {
    return indexOf(std::move(begin), std::move(end), value, execution) != npos;
}

/**
 * Check whether `iterable` contains `value`.
 * @param iterable The iterable to search.
 * @param value The value to find.
 * @return True if `iterable` contains `value`, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
contains(const Iterable& iterable, const T& value, Execution execution = std::execution::seq) {
    return contains(std::begin(iterable), std::end(iterable), value, execution);
}

/**
 * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false
 * otherwise.
 * @param begin The beginning of the sequence to search.
 * @param end The ending of the sequence to search.
 * @param predicate The search predicate.
 * @return Returns true if `predicate` returns true, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
containsIf(Iterator begin, Iterator end, BinaryPredicate predicate, Execution execution = std::execution::seq) {
    return indexOfIf(std::move(begin), std::move(end), std::move(predicate), execution) != npos;
}

/**
 * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
 * @tparam BinaryPredicate A binary predicate functor.
 * @param iterable The iterable to search.
 * @param predicate The search predicate.
 * @return Returns true if `predicate` returns true, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
containsIf(const Iterable& iterable, BinaryPredicate predicate, Execution execution = std::execution::seq) {
    return containsIf(std::begin(iterable), std::end(iterable), std::move(predicate), execution);
}
#    else // ^^^ Lz has execution vvv !Lz has execution

/**
 * Gets the mean of a sequence.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @return The mean of the sequence.
 */
#        ifdef LZ_HAS_CXX_11
template<class Iterator, class BinaryOp = std::plus<internal::ValueType<Iterator>>>
#        else
template<class Iterator, class BinaryOp = std::plus<>>
#        endif // LZ_HAS_CXX_11
double mean(Iterator begin, Iterator end, BinaryOp binOp = {}) {
    using lz::distance;
    using std::distance;
    using ValueType = internal::ValueType<Iterator>;
    const internal::DiffType<Iterator> dist = distance(begin, end);
    const ValueType sum = std::accumulate(begin, end, ValueType{ 0 }, std::move(binOp));
    return static_cast<double>(sum) / dist;
}

/**
 * Gets the mean of a sequence.
 * @param container The container to calculate the mean of.
 * @return The mean of the container.
 */
#        ifdef LZ_HAS_CXX_11
template<class Iterable, class BinaryOp = std::plus<internal::ValueTypeIterable<Iterable>>>
#        else
template<class Iterable, class BinaryOp = std::plus<>>
#        endif // LZ_HAS_CXX_11
double mean(const Iterable& container, BinaryOp binOp = {}) {
    return mean(std::begin(container), std::end(container), std::move(binOp));
}

/**
 * Gets the median of a sequence.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @param comparer The sequence gets sorted with nth_element. A default operator of < is used, however a custom comparer can be
 * used.
 * @return The median of the sequence.
 */
#        ifdef LZ_HAS_CXX_11
template<class Iterator, class Comparer = std::less<internal::ValueType<Iterator>>>
#        else
template<class Iterator, class Comparer = std::less<>>
#        endif // LZ_HAS_CXX_11
double median(Iterator begin, Iterator end, Comparer comparer = {}) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    const internal::DiffType<Iterator> len = distance(begin, end);
    if (len == 0) {
        return 0.;
    }
    const internal::DiffType<Iterator> mid = len >> 1;
    const Iterator midIter = next(begin, mid);
    std::nth_element(begin, midIter, end, comparer);
    if (internal::isEven(len)) {
        const Iterator leftHalf = std::max_element(begin, midIter, comparer);
        return (static_cast<double>(*leftHalf) + *midIter) / 2.;
    }
    return *midIter;
}

/**
 * Gets the median of a sequence.
 * @param iterable The container/sequence by reference.
 * @param comparer The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be
 * used.
 * @return The median of the sequence.
 */
#        ifdef LZ_HAS_CXX_11
template<class Iterable, class Comparer = std::less<internal::ValueTypeIterable<Iterable>>>
#        else
template<class Iterable, class Comparer = std::less<>>
#        endif // LZ_HAS_CXX_11
double median(Iterable& iterable, Comparer comparer = {}) {
    return median(std::begin(iterable), std::end(iterable), std::move(comparer));
}

/**
 * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param toFind The value to find.
 * @param defaultValue The value to return if `toFind` is not found.
 * @return Either `toFind` or `defaultValue`.
 */
template<class Iterator, class T, class U>
internal::ValueType<Iterator> findFirstOrDefault(Iterator begin, Iterator end, const T& toFind, const U& defaultValue) {
    return static_cast<internal::ValueType<Iterator>>(std::find(std::move(begin), end, toFind) == end ? defaultValue : toFind);
}

/**
 * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
 * @param iterable The iterable to search.
 * @param toFind The value to find.
 * @param defaultValue The value to return if `toFind` is not found.
 * @return Either `toFind` or `defaultValue`.
 */
template<class Iterable, class T, class U>
internal::ValueTypeIterable<Iterable> findFirstOrDefault(const Iterable& iterable, const T& toFind, const U& defaultValue) {
    return findFirstOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
}

/**
 * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its
 * corresponding value is returned. If not, `defaultValue` is returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
 * (see `std::find_if`)
 * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
 * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
 */
template<class Iterator, class T, class UnaryPredicate>
internal::ValueType<Iterator>
findFirstOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, const T& defaultValue) {
    using CastType = internal::ValueType<Iterator>;
    const Iterator pos = std::find_if(std::move(begin), end, std::move(predicate));
    return static_cast<CastType>(pos == end ? defaultValue : *pos);
}

/**
 * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its
 * corresponding value is returned. If not, `defaultValue` is returned.
 * @param iterable The sequence to search.
 * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
 * (see `std::find_if`)
 * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
 * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
 */
template<class Iterable, class T, class UnaryPredicate>
internal::ValueTypeIterable<Iterable>
findFirstOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate, const T& defaultValue) {
    return findFirstOrDefaultIf(std::begin(iterable), std::end(iterable), std::move(predicate), defaultValue);
}

/**
 * Searches for the last occurrence for `toFind` in (end, begin]. If no such such element exists, `defaultValue` is returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param toFind The value to find.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence of `toFind` in [begin, end) or `defaultValue` if no such element exists.
 */
template<class Iterator, class T, class U>
internal::ValueType<Iterator> findLastOrDefault(Iterator begin, Iterator end, const T& toFind, const U& defaultValue) {
#        ifdef LZ_HAS_CXX_11
    auto endReverse = std::reverse_iterator<Iterator>(std::move(end));
    auto beginReverse = std::reverse_iterator<Iterator>(std::move(begin));
#        else
    auto endReverse = std::make_reverse_iterator(std::move(end));
    auto beginReverse = std::make_reverse_iterator(std::move(begin));
#        endif
    const auto pos = std::find(std::move(endReverse), beginReverse, toFind);
    return static_cast<internal::ValueType<Iterator>>(pos == beginReverse ? defaultValue : *pos);
}

/**
 * Searches for the last occurrence for `toFind` in `iterable`. If no such such element exists, `defaultValue` is returned.
 * @param iterable The sequence to search.
 * @param toFind The value to find.
 * @param defaultValue The value to return when no element `toFind` exists in `iterable`
 * @return Either the last occurrence of `toFind` in `iterable` or `defaultValue` if no such element exists.
 */
template<class Iterable, class T, class U>
internal::ValueTypeIterable<Iterable> findLastOrDefault(const Iterable& iterable, const T& toFind, const U& defaultValue) {
    return findLastOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
}

/**
 * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is
 * returned.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate The search predicate in [begin, end). Must return bool.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element
 * exists.
 */
template<class Iterator, class T, class UnaryPredicate>
internal::ValueType<Iterator> findLastOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, const T& defaultValue) {
    auto endReverse = std::reverse_iterator<Iterator>(std::move(end));
    auto beginReverse = std::reverse_iterator<Iterator>(std::move(begin));
    const auto pos = std::find_if(std::move(endReverse), beginReverse, std::move(predicate));
    return static_cast<internal::ValueType<Iterator>>(pos == beginReverse ? defaultValue : *pos);
}

/**
 * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is
 * returned.
 * @param iterable The sequence to search.
 * @param predicate The search predicate in [begin, end). Must return bool.
 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
 * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element
 * exists.
 */
template<class Iterable, class T, class UnaryPredicate>
internal::ValueTypeIterable<Iterable>
findLastOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate, const T& defaultValue) {
    return findLastOrDefaultIf(std::begin(iterable), std::end(iterable), std::move(predicate), defaultValue);
}

/**
 * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param val The value to search.
 * @return The index of `val` or lz::npos of no such value exists.
 */
template<class Iterator, class T>
std::size_t indexOf(Iterator begin, Iterator end, const T& val) {
    using lz::distance;
    using std::distance;
    const Iterator pos = std::find(begin, end, val);
    return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
}

/**
 * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
 * @param iterable The iterable to search.
 * @param val The value to search.
 * @return The index of `val` or lz::npos of no such value exists.
 */
template<class Iterable, class T>
std::size_t indexOf(const Iterable& iterable, const T& val) {
    return indexOf(std::begin(iterable), std::end(iterable), val);
}

/**
 * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value
 * exists.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate The search predicate. Uses `std::find_if`.
 * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
 */
template<class Iterator, class UnaryFunc>
std::size_t indexOfIf(Iterator begin, Iterator end, UnaryFunc predicate) {
    using lz::distance;
    using std::distance;
    const Iterator pos = std::find_if(begin, end, std::move(predicate));
    return pos == end ? npos : static_cast<std::size_t>(distance(begin, pos));
}

/**
 * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
 * @param iterable The sequence to search.
 * @param predicate The search predicate. Uses `std::find_if`.
 * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
 */
template<class Iterable, class UnaryFunc>
std::size_t indexOfIf(const Iterable& iterable, UnaryFunc predicate) {
    return indexOfIf(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * Check whether `iterable` contains `value`.
 * @param begin The beginning of the sequence to search.
 * @param end The ending of the sequence to search.
 * @param value The value to find.
 * @return True if `iterable` contains `value`, false otherwise.
 */
template<class Iterator, class T>
bool contains(Iterator begin, Iterator end, const T& value) {
    return std::find(std::move(begin), end, value) != end;
}

/**
 * Check whether `iterable` contains `value`.
 * @param iterable The iterable to search.
 * @param value The value to find.
 * @return True if `iterable` contains `value`, false otherwise.
 */
template<class Iterable, class T>
bool contains(const Iterable& iterable, const T& value) {
    return contains(std::begin(iterable), std::end(iterable), value);
}

/**
 * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false
 * otherwise.
 * @tparam BinaryPredicate A binary predicate functor.
 * @param begin The beginning of the sequence to search.
 * @param end The ending of the sequence to search.
 * @param predicate The search predicate.
 * @return Returns true if `predicate` returns true, false otherwise.
 */
template<class Iterator, class BinaryPredicate>
bool containsIf(Iterator begin, Iterator end, BinaryPredicate predicate) {
    return std::find_if(std::move(begin), end, std::move(predicate)) != end;
}

/**
 * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
 * @tparam BinaryPredicate A binary predicate functor.
 * @param iterable The iterable to search.
 * @param predicate The search predicate.
 * @return Returns true if `predicate` returns true, false otherwise.
 */
template<class Iterable, class BinaryPredicate>
bool containsIf(const Iterable& iterable, BinaryPredicate predicate) {
    return containsIf(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
 * passed to the `mapFunc`.
 * @param mapFunc The function that returns the (new) type.
 * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
 * `mapFunc`.
 */
template<class UnaryFilterFunc, class UnaryMapFunc, class Iterator>
Map<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>
filterMap(Iterator begin, Iterator end, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
    Filter<Iterator, UnaryFilterFunc> filterView = filterRange(std::move(begin), std::move(end), std::move(filterFunc));
    return map(std::move(filterView), std::move(mapFunc));
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
template<class UnaryFilterFunc, class UnaryMapFunc, class Iterable>
Map<internal::FilterIterator<internal::IterTypeFromIterable<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
    return filterMap(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                     std::move(filterFunc), std::move(mapFunc));
}

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param begin The iterator beginning to select values from.
 * @param end The iterator ending to select values from.
 * @param beginSelector The selector beginning iterator that specifies to select an item in [begin, end) yes or no.
 * @param endSelector The selector ending iterator that specifies to select an item in [begin, end) yes or no.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<class Iterator, class SelectorIterator>
Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>, internal::GetFn<1>>, internal::GetFn<0>>
select(Iterator begin, Iterator end, SelectorIterator beginSelector, SelectorIterator endSelector) {
    auto zipper = zipRange(std::make_tuple(std::move(begin), std::move(beginSelector)),
                           std::make_tuple(std::move(end), std::move(endSelector)));
    return filterMap(std::move(zipper), internal::GetFn<1>(), internal::GetFn<0>());
}

/**
 * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
 * @param iterable The iterable to select values from.
 * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
 * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
 */
template<class Iterable, class SelectorIterable, class Iterator = internal::IterTypeFromIterable<Iterable>,
         class SelectorIterator = internal::IterTypeFromIterable<SelectorIterable>>
Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>, internal::GetFn<1>>, internal::GetFn<0>>
select(Iterable&& iterable, SelectorIterable&& selectors) {
    return select(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                  internal::begin(std::forward<SelectorIterable>(selectors)),
                  internal::end(std::forward<SelectorIterable>(selectors)));
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long as
 * `last` returns true for the trimming of the ending.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @return An iterator view object.
 */
template<class Iterator, class UnaryPredicateFirst, class UnaryPredicateLast>
internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<Iterator>>>
trim(Iterator begin, Iterator end, UnaryPredicateFirst first, UnaryPredicateLast last) {
    auto takenFirst = dropWhileRange(std::move(begin), std::move(end), std::move(first));
    auto takenLast = dropWhile(lz::reverse(std::move(takenFirst)), std::move(last));
    return lz::reverse(std::move(takenLast));
}

/**
 * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the beginning and as long as
 * `last` returns true for the trimming of the ending.
 * @param iterable The iterable to trim
 * @param first The unary predicate that trims the first elements while it returns `true`
 * @param last The unary predicate that trims the last elements while it returns `true`
 * @return An iterator view object.
 */
template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<internal::IterTypeFromIterable<Iterable>>>>
trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
    return trim(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                std::move(first), std::move(last));
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param begin The beginning of the string to trim.
 * @param end The ending of the string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
LZ_NODISCARD
LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(std::string::const_iterator begin, std::string::const_iterator end) {
    const auto isSpaceFn = [](const char c) {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    };
    return trim(begin, end, isSpaceFn, isSpaceFn);
}

/**
 * Trims a string and returns an iterator that skips whitespaces at the front and end.
 * @param s The string to trim.
 * @return The string, with trimmed spaces/tabs/newlines at the front and end.
 */
LZ_NODISCARD
LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<std::reverse_iterator<std::reverse_iterator<std::string::const_iterator>>>
trimString(const std::string& s) {
    return trimString(s.begin(), s.end());
}

#    endif // End LZ_HAS_EXECUTION
} // End namespace lz

#endif // End LZ_FUNCTION_TOOLS_HPP