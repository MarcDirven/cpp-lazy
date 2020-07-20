#pragma once

#include <iterator>
#include <Lz/detail/TakeEveryIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator>
    class TakeEvery final : public detail::BasicIteratorView<detail::TakeEveryIterator<Iterator>> {
    public:
        using iterator = detail::TakeEveryIterator<Iterator>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin;
        iterator _end;

    public:
        /**
         * @brief TakeEvery constructor.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param offset The offset to add each iteration, aka the amount of elements to skip.
         */
        TakeEvery(Iterator begin, Iterator end, const size_t offset) :
            _begin(begin, offset),
            _end(end, offset) {
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

    /**
     * @brief This random access iterator can be used to select elements with `offset` amount. If MSVC and the type is
     * an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @details If one would like to select every 2nd argument one can use this iterator. Example (psuedocode):
     * `takeevery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
     * one can use: `takeevery({1, 2, 3}, 2, 2)` the second `2` is the start position, making it select only `3`.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param offset The index to add every iteration, aka the index to 'select'.
     * @param start The start position, optional. Can be used to skip the first element as well.
     * @return A TakeEvery object.
     */
    template<class Iterator>
    auto takeeveryrange(Iterator begin, Iterator end, const size_t offset, const size_t start = 0) {
        return TakeEvery<Iterator>(std::next(begin, start), end, offset);
    }

    /**
     * @brief This random access iterator can be used to select elements with `offset` amount.
     * @details If one would like to select every 2nd argument one can use this iterator. Example (psuedocode):
     * `takeevery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
     * one can use: `takeevery({1, 2, 3}, 2, 2)` the second `2` is the start position, making it select only `3`.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An object that can be iterated over.
     * @param offset The index to add every iteration, aka the index to 'select'.
     * @param start The start position, optional. Can be used to skip the first element as well.
     * @return A TakeEvery object.
     */
    template<class Iterable>
#ifdef _MSC_VER
    typename std::enable_if<detail::IsContiguousContainer<Iterable>::value,
        decltype(takeeveryrange(&*std::begin(std::declval<Iterable>()),
                                &*std::begin(std::declval<Iterable>()),
                                0))>::type
#else
    auto
#endif
    takeevery(Iterable&& iterable, const size_t offset, const size_t start = 0) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);

        if (begin == end) {  // Prevent UB when subtracting 1 and dereference it
            return takeeveryrange(&(*begin), &(*end), offset, start);
        }

        --end;
        return takeeveryrange(&(*begin), &(*end) + 1, offset, start);
#else
        return takeeveryrange(iterable.begin(), iterable.end(), offset, start);
#endif
    }

#ifdef _MSC_VER
    template<class Iterable>
    typename std::enable_if<!detail::IsContiguousContainer<Iterable>::value,
        decltype(takeeveryrange(std::begin(std::declval<Iterable>()),
                                std::begin(std::declval<Iterable>()),
                                0))>::type
    takeevery(Iterable&& iterable, const size_t offset, const size_t start = 0) {
        return takeevery(std::begin(iterable), std::end(iterable), offset, start);
    }
#endif
}