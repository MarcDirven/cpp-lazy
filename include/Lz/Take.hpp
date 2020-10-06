#pragma once

#include <vector>
#include <array>

#include <Lz/detail/TakeIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Take final : public internal::BasicIteratorView<Iterator> {
    public:
        using iterator = detail::TakeIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
         * returns false, the iterator stops.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         * @param function Function that must contain a the value type in its arguments and must return a bool. If the
         * function returns false, the iterator stops.
         */
        Take(Iterator begin, Iterator end, Function function) :
            _begin(begin, end, function),
            _end(end, end, function) {
        }

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const override {
            return _end;
        }
    };

    // Start of group
    /**
     * @defgroup ItFns Iterator free functions.
     * These are the iterator functions and can all be used to iterate over in a
     * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
     * `toVector<Allocator>`, `toArray<N>`, `to<container>` (specifying its value type of the container is not
     *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
     *  typedefs.
     * @{
     */

    /**
     * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
     * returns false, the iterator stops. Its `begin()` function returns a random access iterator.
     * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @param begin The beginning of the iterator.
     * @param end The beginning of the iterator.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhilerange(...))`.
     */
    template<class Iterator, class Function>
    auto takewhilerange(Iterator begin, Iterator end, Function predicate) {
        return Take<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief This function does the same as `lz::takewhilerange` except that it takes an iterable as parameter.
     * Its `begin()` function returns a random access iterator.
     * @param iterable An object that has methods `begin()` and `end()`.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhile(...))`.
     */
    template<class Iterable, class Function>
    auto takewhile(Iterable&& iterable, Function predicate) {
        return takewhilerange(std::begin(iterable), std::end(iterable), predicate);
    }

    /**
     * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
     * random access iterator. If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual
     * iterator object.
     * @param begin The beginning of the 'view'.
     * @param end The ending of the 'view'.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takerange(...))`.
     */
    template<class Iterator>
    auto takerange(Iterator begin, Iterator end) {
        return takewhilerange(begin, end, [](const auto&) { return true; });
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
    template<LZ_CONCEPT_ITERABLE Iterable>
    Take<internal::IterType < Iterable>> take(Iterable&& iterable, const std::size_t amount) {
        auto begin = std::begin(iterable);
        return takerange(begin, std::next(begin, amount));
    }

    /**
     * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
     * Its `begin()` function returns a random access iterator.
     * @param iterable An iterable with method `begin()`.
     * @param from The offset from the beginning of the iterable.
     * @param to The offset from the beginning to take. `from` must be higher than `to`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::slice(...))`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    Take<internal::IterType<Iterable>> slice(Iterable&& iterable, const std::size_t from, const std::size_t to) {
        auto begin = std::begin(iterable);
        return takerange(std::next(begin, from), std::next(begin, to));
    }

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
	Take<Iterator> dropWhileRange(Iterator begin, const Iterator end, Function&& predicate) {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        begin = std::find_if(begin, end, [&predicate](const ValueType& value) {
            return !predicate(value);
		});
        return takeRange(begin, end);
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
	Take<internal::IterType<Iterable>> dropWhile(Iterable&& iterable, Function&& predicate) {
        return dropWhileRange(std::begin(iterable), std::end(iterable), predicate);
    }
	

    // End of group
    /**
     * @}
     */
}
