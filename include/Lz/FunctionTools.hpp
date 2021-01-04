#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#define LZ_FUNCTION_TOOLS_HPP


#include <numeric>
#include <iterator>
#include <algorithm>

#include "StringSplitter.hpp"
#include "Join.hpp"
#include "Zip.hpp"
#include "Map.hpp"
#include "Filter.hpp"
#include "Take.hpp"
#include "Concatenate.hpp"

#ifdef LZ_HAS_CXX_17
  #define LZ_INLINE_VAR inline
#else // ^^^ has cxx 17 vvv !has cxx 17
  #define LZ_INLINE_VAR
#endif // lz has cxx17

#include <cassert>

namespace lz {
	// ReSharper disable once CppUnnamedNamespaceInHeaderFile
	namespace internal { namespace {
        bool stringReplaceImpl(std::string& string, const std::string& oldString, const std::string& newString, const std::size_t startPos,
                               std::false_type /* replaceAll */) {
            const std::size_t oldStringSize = oldString.length();
            const std::size_t newStringSize = newString.length();

            if (oldStringSize == 0 || newStringSize == 0) {
                return false;
            }

            if (startPos == std::string::npos) {
                return false;
            }

            string.replace(startPos, oldStringSize, newString);
            return true;
        }

        bool stringReplaceImpl(std::string& string, const std::string& oldString, const std::string& newString, std::size_t startPos,
                               std::true_type /* replaceAll */) {
            if (!stringReplaceImpl(string, oldString, newString, startPos, std::false_type())) {
                return false;
            }

            const std::size_t oldStringSize = oldString.length();
            const std::size_t newStringSize = newString.length();
            startPos = string.find(oldString, newStringSize + startPos);

            while (startPos != std::string::npos) {
                string.replace(startPos, oldStringSize, newString);
                startPos = string.find(oldString, newStringSize + startPos);
            }
            return true;
        }
		
        template<class To>
        struct ConvertFn {
            template<class From>
            To operator()(From&& f) const {
                return static_cast<To>(f);
            }

			template<class From>
			To operator()(From&& f) {
				return static_cast<To>(f);
			}
        };

        const char* begin(const char* s) {
            return s;
        }

        const char* end(const char* s) {
            return s + std::strlen(s);
        }

        template<class Tuple, class Fn, std::size_t... I>
        auto makeExpandFn(Fn fn, lz::internal::IndexSequence<I...>)
#ifdef LZ_HAS_CXX_11
        -> std::function<lz::internal::FunctionReturnType<Fn, decltype(std::get<I>(std::declval<Tuple>()))...>(Tuple)>
#endif
        {
#ifdef LZ_HAS_CXX_11
            return std::bind([](Fn fn, Tuple tuple) {
                return fn(std::get<I>(std::forward<Tuple>(tuple))...);
            }, std::move(fn), std::placeholders::_1);
#else
            return [fn = std::move(fn)](Tuple tuple) {
                return fn(std::get<I>(std::forward<Tuple>(tuple))...);
            };
#endif
        }
    }} // namespace internal::<anonymous>

    /**
     * This value is returned when indexOf(If) does not find the value specified.
     */
    constexpr LZ_INLINE_VAR std::size_t npos = std::numeric_limits<std::size_t>::max();

    /**
      * Returns a StringSplitter iterator, that splits the string on `'\n'`.
      * @tparam SubString The string type that the `StringSplitter::value_type` must return. Must either be std::string or std::string_view.
      * @tparam String The string type. `std::string` is assumed but can be specified.
      * @param string The string to split on.
      * @return Returns a StringSplitter iterator, that splits the string on `'\n'`.
      */
#ifdef LZ_HAS_STRING_VIEW
    template<class SubString = std::string_view, class String = std::string>
#else // ^^^ Lz has string view vvv !lz has string view
    template<class SubString = std::string, class String = std::string>
#endif // end has string view
    StringSplitter<SubString, String> lines(const String& string) {
        return lz::split<SubString, String>(string, "\n");
    }

    /**
     * Replaces one occurrence of `oldString` in `string` and replaces it with `newString`, and returns whether there was any newString
     * at all.
     * @param string The string to modify.
     * @param oldString The string to replace.
     * @param newString The new string.
     * @return `true` if replacing has taken place, `false` otherwise.
     */
    inline bool strReplace(std::string& string, const std::string& oldString, const std::string& newString) {
        return internal::stringReplaceImpl(string, oldString, newString, string.find(oldString), std::false_type());
    }

    /**
     * Replaces all occurrences of `oldString` in `string` to `newString`.
     * @param string The string to modify.
     * @param oldString The string to replace.
     * @param newString The new string.
     * @return `true` if replacing has taken place, `false` otherwise.
     */
    inline bool strReplaceAll(std::string& string, const std::string& oldString, const std::string& newString) {
        return internal::stringReplaceImpl(string, oldString, newString, string.find(oldString), std::true_type());
    }

#ifdef LZ_HAS_STRING_VIEW
    template<class... Strings>
    auto concatAsStringView(Strings&&... strings) -> lz::Concatenate<decltype(std::begin(std::string_view(strings)))...> {
        static_assert(
            internal::IsAllSame<char, typename std::iterator_traits<decltype(internal::begin(strings))>::value_type...>::value,
            "the character type of the strings must use `char`"
        );
        return lz::concat(static_cast<std::string_view>(strings)...);
    }
#endif // end lz has string view

    /**
     * Sums all the values from [from, upToAndIncluding]
     * @tparam T An integral value.
     * @param from The start to sum from
     * @param upToAndIncluding The end of the sum
     * @return The result of the sum from [from, upToAndIncluding]
     */
    template<class T>
    T sumTo(const T from, const T upToAndIncluding) {
        static_assert(std::is_integral<T>::value, "To must be integral type");

        const T fromAbs = std::abs(from);
        const T toAbs = std::abs(upToAndIncluding);
        const T error = ((fromAbs - 1) * (fromAbs)) / 2;
        const T sum = (toAbs * (toAbs + 1)) / 2;

        if (from < 0) {
            if (upToAndIncluding < 0) {
                return error - sum;
            }
            return -(error + fromAbs - sum);
        }
        if (upToAndIncluding < 0) {
            return error - sum;
        }
        assert(from < upToAndIncluding && "'from' cannot be smaller than 'upToAndIncluding' if both are positive");
        return sum - error;
    }

    /**
     * Sums all the values from [0, upToAndIncluding]
     * @tparam T An integral value.
     * @param upToAndIncluding The end of the sum
     * @return The result of the sum from [0, upToAndIncluding]
     */
    template<class T>
    T sumTo(const T upToAndIncluding) {
        return lz::sumTo(0, upToAndIncluding);
    }

    /**
     * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
     * @param strings The container of `std::string` or `std::string_view`.
     * @return A Join iterator that joins the strings in the container on `'\n'`.
     */
    template<class Strings, LZ_CONCEPT_ITERATOR Iterator = internal::IterTypeFromIterable<Strings>>
    Join<Iterator> unlines(Strings&& strings) {
        static_assert(std::is_same<std::string, internal::ValueType<Iterator>>::value
#ifdef LZ_HAS_STRING_VIEW
            || std::is_same<std::string_view, internal::ValueType<Iterator>>::value
#endif // Lz has string view
            , "the type of the container should be std::string or std::string_view");
        return lz::join(strings, "\n");
    }

    /**
     * This function is defined when C++ version is lower than 17.
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transAccumulate(s.begin(), s.end(), 0, [](size_t i, const std::string& rhs) {
     *      return i + rhs.size();
     * }); // totalSize = 11
     * ```
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param init The starting value.
     * @param binaryPredicate Function that specifies what to add to `init`.
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Init, class BinaryPredicate>
#ifdef LZ_HAS_CXX_17
    [[deprecated("a similar method is defined in <algorithm>; use std::transform_reduce instead")]]
#endif // end lz has cxx 17
    Init transAccumulate(Iterator begin, Iterator end, Init init, BinaryPredicate binaryPredicate) {
        for (; begin != end; ++begin) {
            init = binaryPredicate(std::move(init), *begin);
        }
        return init;
    }

    /**
     * This function is defined when C++ version is lower than 17.
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transAccumulate(s, 0, [](size_t i, const std::string& rhs) {
     *      return i + rhs.size();
     * }); // totalSize = 11
     * ```
     * @param it The container to iterate over.
     * @param init The starting value.
     * @param binaryPredicate Function that specifies what to add to `init`.
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Init, class BinaryPredicate>
    Init transAccumulate(const Iterable& it, Init init, BinaryPredicate binaryPredicate) {
        return lz::transAccumulate(std::begin(it), std::end(it), std::move(init), std::move(binaryPredicate));
    }

    /**
     * Returns a view object of which its iterators are reversed.
     * @param begin The beginning of the sequence. Must have at least std::bidirectional_iterator_tag.
     * @param end The ending of the sequence. Must have at least std::bidirectional_iterator_tag.
     * @return A Take view object contains the reverse order of [begin end)
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator>
    Take<std::reverse_iterator<Iterator>> reverse(Iterator begin, Iterator end) {
#ifndef LZ_HAS_CONCEPTS
        static_assert(internal::IsBidirectional<Iterator>::value, "the type of the iterator must be bidirectional or stronger");
#endif // !Lz has concepts
        using ReverseIterator = std::reverse_iterator<Iterator>;
        return lz::takeRange(ReverseIterator(end), ReverseIterator(begin), std::distance(begin, end));
    }
    /**
     * Returns a view object of which its iterators are reversed.
     * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
     * @return A Take view object contains the reverse order of [begin end)
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
    Take<std::reverse_iterator<internal::IterTypeFromIterable<Iterable>>> reverse(Iterable&& iterable) {
        return lz::reverse(internal::begin(std::forward<Iterable>(iterable)),
                           internal::end(std::forward<Iterable>(iterable))); // ADL std::reverse
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
    Map<Iterator, internal::ConvertFn<T>> as(Iterator begin, Iterator end) {
        return lz::mapRange(std::move(begin), std::move(end), internal::ConvertFn<T>());
    }

    /**
     * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterable A container/iterable.
     * @return A map iterator that constructs To from each of the elements in the given container.
     */
    template<class T, LZ_CONCEPT_ITERABLE Iterable>
    Map<internal::IterTypeFromIterable<Iterable>, internal::ConvertFn<T>> as(Iterable&& iterable) {
        return lz::as<T>(internal::begin(std::forward<Iterable>(iterable)),
                         internal::end(std::forward<Iterable>(iterable)));
    }

    /**
     * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
     * Signature must look like: `fn(ValueTypeIterable1[, ValueTypeIterable2[, ValueTypeIterable3[, ValueTypeIterable-n[, ...]]]])`
     * @param fn The function to apply to each elements in all containers.
     * @param begin The beginning of all the iterables
     * @param end The ending of all the iterables
     * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
     */
    template<class Fn, LZ_CONCEPT_ITERATOR... Iterators,
        class Zipper = lz::Zip<Iterators...>,
        class ValueType = decltype(*std::declval<Zipper>().begin())>
    auto zipWith(Fn fn, std::tuple<Iterators...> begin, std::tuple<Iterators...> end)
#ifdef LZ_HAS_CXX_11
    -> lz::Map<typename Zipper::iterator,
               decltype(internal::makeExpandFn<ValueType>(fn, internal::MakeIndexSequence<sizeof...(Iterators)>()))>
#endif
    {
        Zipper zipper = lz::zipRange(std::move(begin), std::move(end));
        constexpr std::size_t size = sizeof...(Iterators);
        auto tupleExpanderFunc = internal::makeExpandFn<ValueType>(std::move(fn), internal::MakeIndexSequence<size>());
        return lz::map(std::move(zipper), std::move(tupleExpanderFunc));
    }

    /**
     * Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers' value types.
     * Signature must look like: `fn(ValueTypeIterable1[, ValueTypeIterable2[, ValueTypeIterable3[, ValueTypeIterable-n[, ...]]]])`
     * @param fn The function to apply to each elements in all containers.
     * @param iterables The iterables.
     * @return A Map<Zip> object that applies fn over each expanded tuple elements from [begin, end)
     */
    template<class Fn, class... Iterables
#ifdef LZ_HAS_CXX_11
        , class Zipper = lz::Zip<lz::internal::IterTypeFromIterable<Iterables>...>,
        class ValueType = decltype(*std::declval<Zipper>().begin())
#endif
    >
    auto zipWith(Fn fn, Iterables&&... iterables)
#ifdef LZ_HAS_CXX_11
    -> lz::Map<typename Zipper::iterator,
               decltype(internal::makeExpandFn<ValueType>(fn, lz::internal::MakeIndexSequence<sizeof...(Iterables)>()))>
#endif
    {
        return lz::zipWith(std::move(fn), std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                                          std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
    }

    /**
     * Returns the length of [begin, end)
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The length of the iterator with type Iterator::difference_type
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::DiffType<Iterator> length(Iterator begin, Iterator end) {
        return std::distance(begin, end);
    }

    /**
     * Gets the length of the sequence.
     * @param iterable The sequence to get the length of.
     * @return The length of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    internal::DiffType<internal::IterTypeFromIterable<Iterable>> length(const Iterable& iterable) {
        return lz::length(std::begin(iterable), std::end(iterable));
    }

    /**
     * Checks whether [begin, end) is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if it is empty, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool isEmpty(const Iterator begin, const Iterator end) {
        return begin == end;
    }

    /**
     * Checks whether `iterable` is empty.
     * @param iterable The sequence to check whether it is empty yes/no.
     * @return True if it is empty, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool isEmpty(const Iterable& iterable) {
        return lz::isEmpty(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns whether the sequence holds exactly one element.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if it has one element exactly, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool hasOne(Iterator begin, const Iterator end) {
        return !lz::isEmpty(begin, end) && ++begin == end;
    }

    /**
     * Returns whether the sequence holds exactly one element.
     * @param iterable The sequence to check whether is has exactly one element.
     * @return True if it has one element exactly, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool hasOne(const Iterable& iterable) {
        return lz::hasOne(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns whether the sequence holds >= 2 elements.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if the amount of values are >= 2, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool hasMany(Iterator begin, Iterator end) {
        return !lz::isEmpty(begin, end) && !lz::hasOne(begin, end);
    }

    /**
     * Returns whether the sequence holds >= 2 elements.
     * @param iterable The sequence to check whether it has many (>= 2) elements.
     * @return True if the amount of values are >= 2, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool hasMany(const Iterable& iterable) {
        return !lz::isEmpty(iterable) && !lz::hasOne(iterable);
    }

    /**
     * Returns the first element. Asserts if the sequence is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return The first element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::RefType<Iterator> first(Iterator begin, Iterator end) {
        assert(!lz::isEmpty(begin, end) && "sequence cannot be empty in order to get the first element");
        static_cast<void>(end);
        return *begin;
    }

    /**
     * Returns the first element. Asserts if the sequence is empty.
     * @param iterable The sequence to get the first element of.
     * @return The first element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    internal::RefType<internal::IterTypeFromIterable<Iterable>> first(Iterable&& iterable) {
        return lz::first(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns the last element. Asserts if the sequence is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return The last element of the sequence (by reference i.e. type of *iterator).
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::RefType<Iterator> last(Iterator begin, Iterator end) {
        assert(!lz::isEmpty(begin, end) && "sequence cannot be empty in order to get the last element");
        const internal::DiffType<Iterator> len = lz::length(begin, end);
        std::advance(begin, len - 1);
        return *begin;
    }

    /**
     * Returns the last element. Asserts if the sequence is empty.
     * @param iterable The sequence to get the last element of.
     * @return The last element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    typename internal::IterTypeFromIterable<Iterable>::reference last(Iterable&& iterable) {
        return lz::last(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns the last element. Asserts if the sequence is empty.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence.
     * @param defaultValue The value to return if `iterable` is empty.
     * @return Either the first element of `iterable` or `value` if the sequence is empty.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    internal::ValueType<Iterator> firstOr(Iterator begin, Iterator end, const T& defaultValue) {
        return lz::isEmpty(begin, end) ? static_cast<internal::ValueType<Iterator>>(defaultValue) : lz::first(begin, end);
    }

    /**
     * Gets the first element of the sequence, or `value` if the sequence is empty.
     * @param iterable The iterable to get the first value of, or `value` in case it is empty.
     * @param defaultValue The value to return if `iterable` is empty.
     * @return Either the first element of `iterable` or `value` if the sequence is empty.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> firstOr(const Iterable& iterable, const T& defaultValue) {
        return lz::firstOr(std::begin(iterable), std::end(iterable), defaultValue);
    }

    /**
     * This function returns the last element. If the sequence is empty, it returns `value`.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence.
     * @param value The value to return if `iterable` is empty.
     * @return Either the last element of `iterable` or `value` if the sequence is empty.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    internal::ValueType<Iterator> lastOr(Iterator begin, Iterator end, const T& value) {
        return lz::isEmpty(begin, end) ? static_cast<internal::ValueType<Iterator>>(value) : lz::last(begin, end);
    }

    /**
     * This function returns the last element. If the sequence is empty, it returns `value`.
     * @param iterable The iterable to get the last value of, or `value` in case it is empty.
     * @param value The value to return if `iterable` is empty.
     * @return Either the last element of `iterable` or `value` if the sequence is empty.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> lastOr(const Iterable& iterable, const T& value) {
        return lz::lastOr(std::begin(iterable), std::end(iterable), value);
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    Zip<Iterator, Iterator> pairwise(Iterator begin, Iterator end) {
        assert(lz::hasMany(begin, end) && "length of the sequence must be greater than or equal to 2");
        return lz::zipRange(std::make_tuple(begin, std::next(begin)), std::make_tuple(end, end));
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
     * @param iterable A container/iterable.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERATOR Iterator = internal::IterTypeFromIterable<Iterable>>
    Zip<Iterator, Iterator> pairwise(Iterable&& iterable) {
        return lz::pairwise(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
    }

#ifdef LZ_HAS_EXECUTION

    /**
     * Gets the mean of a sequence.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param exec The execution policy.
     * @return The mean of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Execution = std::execution::sequenced_policy>
    double mean(Iterator begin, Iterator end, const Execution exec = std::execution::seq) {
        using ValueType = internal::ValueType<Iterator>;
        const internal::DiffType<Iterator> distance = std::distance(begin, end);
        ValueType sum{};
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            sum = std::reduce(begin, end);
        }
        else {
            sum = std::reduce(exec, begin, end);
        }
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @param container The container to calculate the mean of.
     * @param exec The execution policy
     * @return The mean of the container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Execution = std::execution::sequenced_policy>
    double mean(const Iterable& container, const Execution exec = std::execution::seq) {
        return lz::mean(std::begin(container), std::end(container), exec);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execPolicy TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
    Map<internal::FilterIterator<Execution, Iterator, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterator begin, Iterator end, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc,
              const Execution execPolicy = std::execution::seq) {
        Filter<Execution, Iterator, UnaryFilterFunc> filterView = lz::filterRange(std::move(begin), std::move(end),
                                                                                  std::move(filterFunc), execPolicy);
        return lz::map(std::move(filterView), std::move(mapFunc));
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execution TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<internal::FilterIterator<Execution, internal::IterTypeFromIterable<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc, const Execution execution = std::execution::seq) {
        return lz::filterMap(internal::begin(std::forward<Iterable>(iterable)),
                             internal::end(std::forward<Iterable>(iterable)), std::move(filterFunc), std::move(mapFunc),
                             execution);
    }

    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR SelectorIterator, class Execution = std::execution::sequenced_policy>
    auto select(Iterator begin, Iterator end, SelectorIterator beginSelector, SelectorIterator endSelector,
                const Execution execution = std::execution::seq) {
        static_assert(std::is_copy_assignable<SelectorIterator>::value, "selector iterator/iterable must be copy assignable");

        using Zipper = lz::Zip<Iterator, SelectorIterator>;
        using ZipIter = typename Zipper::iterator;
        using RefTuple = internal::RefType<ZipIter>;

        Zipper zipper = lz::zipRange(std::make_tuple(std::move(begin), std::move(beginSelector)),
                                     std::make_tuple(std::move(end), std::move(endSelector)));
        return lz::filterMap(std::move(zipper),
                             [](const RefTuple& tuple) -> bool { return std::get<1>(tuple); },
                             [](const RefTuple& tuple) -> internal::RefType<Iterator> { return std::get<0>(tuple); }, execution);
    }

    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE SelectorIterable, class Execution = std::execution::sequenced_policy>
    auto select(Iterable&& iterable, SelectorIterable&& selectors, const Execution execution = std::execution::seq) {
        return select(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                      internal::begin(std::forward<SelectorIterable>(selectors)), internal::end(std::forward<SelectorIterable>(selectors)),
                      execution);
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
    lz::Take<std::reverse_iterator<std::reverse_iterator<Iterator>>>
    trim(Iterator begin, Iterator end, UnaryPredicateFirst first, UnaryPredicateLast last, Execution execution = std::execution::seq) {
        auto takenFirst = lz::dropWhileRange(std::move(begin), std::move(end), std::move(first), execution);
        auto takenLast = lz::dropWhile(std::move(lz::reverse(takenFirst)), std::move(last), execution);
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
    lz::Take<std::reverse_iterator<std::reverse_iterator<lz::internal::IterTypeFromIterable<Iterable>>>>
    trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last, Execution execution = std::execution::seq) {
        return lz::trim(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(first), std::move(last), execution);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(Iterator begin, Iterator end, Compare compare, const Execution execution = std::execution::seq) {
        const internal::DiffType<Iterator> len = std::distance(begin, end);
        constexpr bool isSequenced = internal::checkForwardAndPolicies<Execution, Iterator>();

        if (len == 0) {
            throw std::invalid_argument(LZ_FILE_LINE ": the length of the sequence cannot be 0");
        }

        const internal::DiffType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);

        if constexpr (isSequenced) {
            static_cast<void>(execution);
            std::nth_element(begin, midIter, end, compare);
        }
        else {
            std::nth_element(execution, begin, midIter, end, compare);
        }

        if (internal::isEven(len)) {
            if constexpr (isSequenced) {
                const Iterator leftHalf = std::max_element(begin, midIter);
                return (static_cast<double>(*leftHalf) + *midIter) / 2.;
            }
            else {
                const Iterator leftHalf = std::max_element(execution, begin, midIter);
                return (static_cast<double>(*leftHalf) + *midIter) / 2.;
            }
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class Compare>
    double median(Iterable& iterable, Compare compare, const Execution execution = std::execution::seq) {
        return lz::median(std::begin(iterable), std::end(iterable), compare, execution);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator>
    double median(Iterator begin, Iterator end, const Execution execution = std::execution::seq) {
        return lz::median(begin, end, std::less<internal::ValueType<Iterator>>(), execution);
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable, const Execution execution = std::execution::seq) {
        return lz::median(std::begin(iterable), std::end(iterable),
                          std::less<internal::ValueType<internal::IterTypeFromIterable<Iterable>>>(), execution);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class T, class U>
    internal::ValueType<Iterator>
	firstOrDefault(Iterator begin, Iterator end, T&& toFind, U&& defaultValue, const Execution execution = std::execution::seq) {
        using ValueType = internal::ValueType<Iterator>;
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return static_cast<ValueType>(std::find(begin, end, toFind) == end ? defaultValue : toFind);
        }
        else {
            return static_cast<ValueType>(std::find(execution, begin, end, toFind) == end ? defaultValue : toFind);
        }
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T, class U>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> firstOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue,
                                                                                 const Execution execution = std::execution::seq) {
        return lz::firstOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class T>
    internal::ValueType<Iterator> firstOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate,
                                                   T&& defaultValue, const Execution execution) {
        using ValueType = internal::ValueType<Iterator>;
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            const Iterator pos = std::find_if(begin, end, predicate);
            return static_cast<ValueType>(pos == end ? defaultValue : *pos);
        }
        else {
            const Iterator pos = std::find_if(execution, begin, end, predicate);
            return static_cast<ValueType>(pos == end ? defaultValue : *pos);
        }
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>>
    firstOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, T&& defaultValue,
                     const Execution execution = std::execution::seq) {
        return lz::firstOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue, execution);
    }

    /**
	 * Searches for the last occurrence for `toFind` in (end, begin]. If no such such element exists, `defaultValue` is returned.
	 * @param begin The beginning of the sequence.
	 * @param end The ending of the sequence.
	 * @param toFind The value to find.
	 * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
	 * @return Either the last occurrence of `toFind` in [begin, end) or `defaultValue` if no such element exists.
	 */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class U, class Execution = std::execution::sequenced_policy>
    internal::ValueType<Iterator> lastOrDefault(Iterator begin, Iterator end, T&& toFind, U&& defaultValue,
												const Execution execution = std::execution::seq) {
        using CastType = internal::ValueType<Iterator>;
        using ReverseIterator = std::reverse_iterator<Iterator>;

        ReverseIterator endReverse(end);
        ReverseIterator beginReverse(begin);

        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            const ReverseIterator pos = std::find(endReverse, beginReverse, toFind);
            return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
        }
        else {
            const ReverseIterator pos = std::find(execution, endReverse, beginReverse, toFind);
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
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class U, class Execution = std::execution::sequenced_policy>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> lastOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue,
																				const Execution execution = std::execution::seq) {
        return lz::lastOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
    }

    /**
     * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate The search predicate in [begin, end). Must return bool.
     * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
     * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    internal::ValueType<Iterator> lastOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate,
												  T&& defaultValue, const Execution execution = std::execution::seq) {
        using CastType = internal::ValueType<Iterator>;
        using ReverseIterator = std::reverse_iterator<Iterator>;

        ReverseIterator endReverse(end);
        ReverseIterator beginReverse(begin);

        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            const ReverseIterator pos = std::find_if(endReverse, beginReverse, predicate);
            return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
        }
        else {
            const ReverseIterator pos = std::find_if(execution, endReverse, beginReverse, predicate);
            return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
        }
    }

    /**
     * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate The search predicate in [begin, end). Must return bool.
     * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
     * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> lastOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate,
																				  T&& defaultValue, 
																				  const Execution execution = std::execution::seq) {
        return lz::lastOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue, execution);
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
    std::size_t indexOf(Iterator begin, Iterator end, const T& val, const Execution execution = std::execution::seq) {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            const Iterator pos = std::find(begin, end, val);
            return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
        }
        else {
            const Iterator pos = std::find(execution, begin, end, val);
            return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
        }
    }

    /**
     * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @param iterable The iterable to search.
     * @param val The value to search.
     * @param execution Uses the execution to perform the find.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class To>
    std::size_t indexOf(const Iterable& iterable, const To& val, const Execution execution = std::execution::seq) {
        return lz::indexOf(std::begin(iterable), std::end(iterable), val, execution);
    }

    /**
    * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param begin The beginning of the sequence.
    * @param end The ending of the sequence.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @param execution Uses the execution to perform the find.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
    std::size_t indexOfIf(Iterator begin, Iterator end, UnaryFunc predicate, const Execution execution = std::execution::seq) {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            const Iterator pos = std::find_if(begin, end, predicate);
            return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
        }
        else {
            const Iterator pos = std::find_if(execution, begin, end, predicate);
            return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
        }
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, UnaryFunc predicate, const Execution execution = std::execution::seq) {
        return lz::indexOfIf(std::begin(iterable), std::end(iterable), predicate, execution);
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class Execution = std::execution::sequenced_policy>
    bool contains(Iterator begin, Iterator end, const T& value, const Execution exec = std::execution::seq) {
        return lz::indexOf(begin, end, value, exec) != lz::npos;
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param iterable The iterable to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class Execution = std::execution::sequenced_policy>
    bool contains(const Iterable& iterable, const T& value, const Execution exec = std::execution::seq) {
        return lz::contains(std::begin(iterable), std::end(iterable), value, exec);
    }

    /**
     * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
    bool containsIf(Iterator begin, Iterator end, BinaryPredicate predicate, const Execution exec = std::execution::seq) {
        return lz::indexOfIf(begin, end, predicate, exec) != lz::npos;
    }

    /**
     * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param iterable The iterable to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
    bool containsIf(const Iterable& iterable, BinaryPredicate predicate, const Execution exec = std::execution::seq) {
        return lz::containsIf(std::begin(iterable), std::end(iterable), predicate, exec);
    }
#else // ^^^ Lz has execution vvv !Lz has execution

    /**
     * Gets the mean of a sequence.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The mean of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    double mean(Iterator begin, Iterator end) {
        using ValueType = internal::ValueType<Iterator>;
        const internal::DiffType<Iterator> distance = std::distance(begin, end);
        const ValueType sum = std::accumulate(begin, end, ValueType(0));
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @param container The container to calculate the mean of.
     * @return The mean of the container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double mean(const Iterable& container) {
        return lz::mean(std::begin(container), std::end(container));
    }


    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(Iterator begin, Iterator end, Compare compare) {
        const internal::DiffType<Iterator> len = std::distance(begin, end);
        assert(len > 0 && "the length of the sequence cannot be 0");

        const internal::DiffType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (internal::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Compare>
    double median(Iterable& iterable, Compare compare) {
        return lz::median(std::begin(iterable), std::end(iterable), compare);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    double median(Iterator begin, Iterator end) {
        return lz::median(begin, end, std::less<internal::ValueType<Iterator>>());
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable) {
        return lz::median(std::begin(iterable), std::end(iterable),
                          std::less<internal::ValueType<internal::IterTypeFromIterable<Iterable>>>());
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class U>
    internal::ValueType<Iterator> firstOrDefault(Iterator begin, Iterator end, T&& toFind, U&& defaultValue) {
        using CastType = internal::ValueType<Iterator>;
        return static_cast<CastType>(std::find(begin, end, toFind) == end ? defaultValue : toFind);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class U>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> firstOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue) {
        return lz::firstOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class UnaryPredicate>
    internal::ValueType<Iterator> firstOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, T&& defaultValue) {
        using CastType = internal::ValueType<Iterator>;
        const Iterator pos = std::find_if(begin, end, predicate);
        return static_cast<CastType>(pos == end ? defaultValue : *pos);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> firstOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate,
                                                                                   T&& defaultValue) {
        return lz::firstOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue);
    }

    /**
     * Searches for the last occurrence for `toFind` in (end, begin]. If no such such element exists, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find.
     * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
     * @return Either the last occurrence of `toFind` in [begin, end) or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class U>
    internal::ValueType<Iterator> lastOrDefault(Iterator begin, Iterator end, T&& toFind, U&& defaultValue) {
        using CastType = internal::ValueType<Iterator>;
        using ReverseIterator = std::reverse_iterator<Iterator>;

        ReverseIterator endReverse(end);
        ReverseIterator beginReverse(begin);
        const ReverseIterator pos = std::find(endReverse, beginReverse, toFind);
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }

    /**
     * Searches for the last occurrence for `toFind` in `iterable`. If no such such element exists, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param toFind The value to find.
     * @param defaultValue The value to return when no element `toFind` exists in `iterable`
     * @return Either the last occurrence of `toFind` in `iterable` or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class U>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> lastOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue) {
        return lz::lastOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
    }

    /**
     * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate The search predicate in [begin, end). Must return bool.
     * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
     * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class UnaryPredicate>
    internal::ValueType<Iterator> lastOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, T&& defaultValue) {
        using CastType = internal::ValueType<Iterator>;
        using ReverseIterator = std::reverse_iterator<Iterator>;

        ReverseIterator endReverse(end);
        ReverseIterator beginReverse(begin);
        const ReverseIterator pos = std::find_if(endReverse, beginReverse, predicate);
        return static_cast<CastType>(pos == beginReverse ? defaultValue : *pos);
    }

    /**
     * Searches for the last occurrence of predicate `predicate` in `iterable`. If no such such element exists, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate The search predicate in [begin, end). Must return bool.
     * @param defaultValue The value to return when no element `toFind` exists in [begin, end)
     * @return Either the last occurrence where `predicate` returns `true` in [begin, end) or `defaultValue` if no such element exists.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate>
    internal::ValueType<internal::IterTypeFromIterable<Iterable>> lastOrDefaultIf(const Iterable& iterable, UnaryPredicate predicate,
                                                                                  T&& defaultValue) {
        return lz::lastOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue);
    }

    /**
     * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param val The value to search.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    std::size_t indexOf(Iterator begin, Iterator end, const T& val) {
        const Iterator pos = std::find(begin, end, val);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
    * @param iterable The iterable to search.
    * @param val The value to search.
    * @return The index of `val` or lz::npos of no such value exists.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    std::size_t indexOf(const Iterable& iterable, const T& val) {
        return lz::indexOf(std::begin(iterable), std::end(iterable), val);
    }

    /**
    * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param begin The beginning of the sequence.
    * @param end The ending of the sequence.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
    std::size_t indexOfIf(Iterator begin, Iterator end, UnaryFunc predicate) {
        const Iterator pos = std::find_if(begin, end, predicate);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, UnaryFunc predicate) {
        return lz::indexOfIf(std::begin(iterable), std::end(iterable), predicate);
    }


    /**
     * Check whether `iterable` contains `value`.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    bool contains(Iterator begin, Iterator end, const T& value) {
        return std::find(begin, end, value) != end;
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param iterable The iterable to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    bool contains(const Iterable& iterable, const T& value) {
        return contains(std::begin(iterable), std::end(iterable), value);
    }

    /**
     * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class BinaryPredicate>
    bool containsIf(Iterator begin, Iterator end, BinaryPredicate predicate) {
        return std::find_if(begin, end, predicate) != end;
    }

    /**
     * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param iterable The iterable to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate>
    bool containsIf(const Iterable& iterable, BinaryPredicate predicate) {
        return containsIf(std::begin(iterable), std::end(iterable), predicate);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
    Map<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterator begin, Iterator end, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
        Filter<Iterator, UnaryFilterFunc> filterView = filterRange(std::move(begin), std::move(end), std::move(filterFunc));
        return lz::map(std::move(filterView), std::move(mapFunc));
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<internal::FilterIterator<internal::IterTypeFromIterable<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterable&& iterable, UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) {
        return lz::filterMap(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
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
    template<class Iterator, class SelectorIterator, class Zipper = lz::Zip<Iterator, SelectorIterator>
#ifdef LZ_HAS_CXX_11
        , class ZipIter = typename Zipper::iterator,
        class RefTuple = internal::RefType<ZipIter>
#endif // end lz has cxx11
        >
    auto select(Iterator begin, Iterator end, SelectorIterator beginSelector, SelectorIterator endSelector)
#ifdef LZ_HAS_CXX_11
     -> lz::Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>,  std::function<bool(RefTuple)>>,
                std::function<internal::RefType<Iterator>(RefTuple)>>
#endif // end lz has cxx11
    {
        static_assert(std::is_copy_assignable<SelectorIterator>::value, "selector iterator/iterable must be copy assignable");
        Zipper zipper = lz::zipRange(std::make_tuple(std::move(begin), std::move(beginSelector)),
                                     std::make_tuple(std::move(end), std::move(endSelector)));

#ifndef LZ_HAS_CXX_11
        using RefTuple = internal::RefType<typename Zipper::iterator>;

        return lz::filterMap(std::move(zipper),
                             [](const RefTuple& tuple) -> bool { return std::get<1>(tuple); },
                             [](const RefTuple& tuple) -> internal::RefType<Iterator> { return std::get<0>(tuple); });
#else // ^^^ cxx > 11, vvv cxx == 11
        std::function<bool(RefTuple)> f = [](const RefTuple& tuple) { return static_cast<bool>(std::get<1>(tuple)); };
        std::function<internal::RefType<Iterator>(RefTuple)> m =
            [](const RefTuple& tuple) -> internal::RefType<Iterator> { return std::get<0>(tuple); };
        return lz::filterMap(zipper, std::move(f), std::move(m));
#endif
    }

    /**
     * Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
     * @param iterable The iterable to select values from.
     * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
     * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
     */
    template<class Iterable, class SelectorIterable
#ifdef LZ_HAS_CXX_11
        , class Iterator = internal::IterTypeFromIterable<Iterable>,
        class SelectorIterator = internal::IterTypeFromIterable<SelectorIterable>,
        class ZipIter = typename lz::Zip<Iterator, SelectorIterator>::iterator,
        class RefTuple = internal::RefType<ZipIter>
#endif // end lz has cxx11
        >
    auto select(Iterable&& iterable, SelectorIterable&& selectors)
#ifdef LZ_HAS_CXX_11
    -> lz::Map<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>,  std::function<bool(RefTuple)>>,
               std::function<internal::RefType<Iterator>(RefTuple)>>
#endif // end lz has cxx11
    {
        return select(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                      internal::begin(std::forward<SelectorIterable>(selectors)), internal::end(std::forward<SelectorIterable>(selectors)));
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
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator, class UnaryPredicateFirst, class UnaryPredicateLast>
    lz::Take<std::reverse_iterator<std::reverse_iterator<Iterator>>>
    trim(Iterator begin, Iterator end, UnaryPredicateFirst first, UnaryPredicateLast last) {
        auto takenFirst = lz::dropWhileRange(std::move(begin), std::move(end), std::move(first));
        auto takenLast = lz::dropWhile(std::move(lz::reverse(takenFirst)), std::move(last));
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
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
    lz::Take<std::reverse_iterator<std::reverse_iterator<lz::internal::IterTypeFromIterable<Iterable>>>>
    trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
        return lz::trim(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(first), std::move(last));
    }
#endif // End LZ_HAS_EXECUTION
} // End namespace lz

#endif // End LZ_FUNCTION_TOOLS_HPP