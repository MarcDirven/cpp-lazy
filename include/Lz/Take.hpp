#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include "detail/BasicIteratorView.hpp"
#include <cassert>

namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Take final : public internal::BasicIteratorView<Iterator> {
    public:
        using iterator = Iterator;
        using const_iterator = Iterator;

        using value_type = internal::ValueType<Iterator>;

        /**
         * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
         * returns false, the iterator stops.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         * @param predicate Function that must contain a the value type in its arguments and must return a bool. If the
         * function returns false, the iterator stops.
         */
        template<class Function>
        Take(Iterator begin, Iterator end, Function predicate) :
            internal::BasicIteratorView<iterator>(begin != end ? (!predicate(*begin) ? end : begin) : end, end)
        {
        }

        /**
         * @brief Extra constructor overload with `std::nullptr_t`. Takes elements from an iterator from [begin, ...).
         * Takes no function as argument. Rather a `nullptr` to indicate that we are just taking a sequence.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         */
        Take(Iterator begin, Iterator end, std::nullptr_t) :
            internal::BasicIteratorView<iterator>(std::move(begin), std::move(end))
        {
        }

        Take() = default;
    };

    // Start of group
    /**
     * @defgroup ItFns Iterator free functions.
     * These are the iterator functions and can all be used to iterate over in a
     * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
     * `toVector<Allocator>`, `toArray<N>`, `to<container>. toMap, toUnorderedMap` (specifying its value type of the container is not
     *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
     *  typedefs.
     * @{
     */

    /**
     * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
     * returns false, the iterator stops. Its `begin()` function returns an iterator.
     * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @param begin The beginning of the iterator.
     * @param end The beginning of the iterator.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takeWhileRange(...))`.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
    Take<Iterator> takeWhileRange(Iterator begin, Iterator end, Function predicate) {
        return Take<Iterator>(std::move(begin), std::move(end), std::move(predicate));
    }

    /**
     * @brief This function does the same as `lz::takeWhileRange` except that it takes an iterable as parameter.
     * Its `begin()` function returns an iterator.
     * @param iterable An object that has methods `begin()` and `end()`.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takeWhile(...))`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Function>
    Take<internal::IterTypeFromIterable<Iterable>> takeWhile(Iterable&& iterable, Function predicate) {
        return takeWhileRange(std::begin(iterable), std::end(iterable), std::move(predicate));
    }

    /**
     * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
     * an iterator. If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual
     * iterator object.
     * @param begin The beginning of the 'view'.
     * @param end The ending of the 'view'.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takeRange(...))`.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    Take<Iterator> takeRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> amount) {
        assert(amount <= std::distance(begin, end) && "cannot access elements after end");
        static_cast<void>(end);
        return takeWhileRange(begin, std::next(begin, amount), nullptr);
    }

    /**
     * @brief This function takes an iterable and slices `amount` from the beginning of the array. Essentially it is
     * equivalent to [`iterable.begin(), iterable.begin() + amount`). Its `begin()` function returns a random
     * access iterator.
     * @param iterable An iterable with method `begin()`.
     * @param amount The amount of elements to take from the beginning of the `iterable`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::take(...))`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
    Take<IterType> take(Iterable&& iterable, const internal::DiffType<IterType> amount) {
        const auto begin = std::begin(iterable);
        return takeRange(begin, std::end(iterable), amount);
    }

    /**
     * Drops an amount of items, starting from begin.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param amount The amount of items to drop, which is equivalent to next(begin, amount)
     * @return A Take iterator where the first `amount` items have been dropped.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    Take<Iterator> dropRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> amount) {
        return takeRange(std::next(begin, amount), end, std::distance(begin, end) - amount);
    }

    /**
     * Drops an amount of items, starting from begin.
     * @param iterable The iterable to drop from.
     * @param amount The amount of items to drop, which is equivalent to next(begin, amount)
     * @return A Take iterator where the first `amount` items have been dropped.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
    Take<IterType> drop(Iterable&& iterable, const internal::DiffType<IterType> amount) {
        return dropRange(std::begin(iterable), std::end(iterable), amount);
    }

    /**
     * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
     * Its `begin()` function returns an iterator.
     * @param iterable An iterable with method `begin()`.
     * @param from The offset from the beginning of the iterable.
     * @param to The offset from the beginning to take. `from` must be higher than `to`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::slice(...))`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
    Take<internal::IterTypeFromIterable<Iterable>> slice(Iterable&& iterable, const internal::DiffType<IterType> from,
                                                         const internal::DiffType<IterType> to) {
        assert(to >= from && "parameter `to` cannot be more than `from`");
        const auto begin = std::begin(iterable);
        return takeRange(std::next(begin, from), std::end(iterable), to - from);
    }

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief Creates a Take iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @param exec The execution policy. Must be one of std::execution::*
     * @return A Take iterator view object.
     */
	template<LZ_CONCEPT_ITERATOR Iterator, class Function, class Execution = std::execution::sequenced_policy>
	Take<Iterator> dropWhileRange(Iterator begin, Iterator end, Function predicate, const Execution exec = std::execution::seq) {
        using ValueType = internal::ValueType<Iterator>;
        constexpr bool isSequenced = internal::checkForwardAndPolicies<Execution, Iterator>();

        if constexpr (isSequenced) {
            static_cast<void>(exec);
            begin = std::find_if(std::move(begin), end, [pred = std::move(predicate)](const ValueType& value) {
                return !pred(value);
            });
        }
        else {
            begin = std::find_if(exec, std::move(begin), end, [pred = std::move(predicate)](const ValueType& value) {
                return !pred(value);
            });
        }
        return takeRange(begin, end, std::distance(begin, end));
    }

    /**
     * @brief Creates a Take iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @param iterable The sequence with the values that can be iterated over.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @param exec The execution policy. Must be one of std::execution::*
     * @return A Take iterator view object.
     */
	template<LZ_CONCEPT_ITERABLE Iterable, class Function, class Execution = std::execution::sequenced_policy>
	Take<internal::IterTypeFromIterable<Iterable>> dropWhile(Iterable&& iterable, Function predicate,
                                                             const Execution exec = std::execution::seq) {
        return dropWhileRange(std::begin(iterable), std::end(iterable), std::move(predicate), exec);
    }
#else // ^^^ lz has execution vvv lz ! has execution
    /**
     * @brief Creates a Take iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @return A Take iterator view object.
     */
	template<LZ_CONCEPT_ITERATOR Iterator, class Function>
	Take<Iterator> dropWhileRange(Iterator begin, Iterator end, Function predicate) {
        using ValueType = internal::ValueType<Iterator>;
#ifdef LZ_HAS_CXX11
        begin = std::find_if(std::move(begin), end, std::bind([](const ValueType& value, Function pred) {
            return !pred(value);
		}, std::placeholders::_1, std::move(predicate)));
#else // ^^^lz has cxx 11 vvv lz cxx > 11
        begin = std::find_if(std::move(begin), end, [pred = std::move(predicate)](const ValueType& value) {
            return !pred(value);
		});
#endif // end lz has cxx 11
        return takeRange(begin, end, std::distance(begin, end));
    }

    /**
     * @brief Creates a Take iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @param iterable The sequence with the values that can be iterated over.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @return A Take iterator view object.
     */
	template<LZ_CONCEPT_ITERABLE Iterable, class Function>
	Take<internal::IterTypeFromIterable<Iterable>> dropWhile(Iterable&& iterable, Function predicate) {
        return dropWhileRange(std::begin(iterable), std::end(iterable), std::move(predicate));
    }
	
#endif // end lz has execution
    // End of group
    /**
     * @}
     */
}

#endif