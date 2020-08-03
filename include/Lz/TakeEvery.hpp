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
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief TakeEvery constructor.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param offset The offset to add each iteration, aka the amount of elements to skip.
         */
        TakeEvery(const Iterator begin, const Iterator end, const size_t offset) :
            _begin(begin, end, offset, std::distance(begin, end)),
            _end(end, end, offset, _begin._distance) {
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
    TakeEvery<Iterator> takeeveryrange(Iterator begin, Iterator end, const size_t offset, const size_t start = 0) {
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
    auto takeevery(Iterable&& iterable, const size_t offset, const size_t start = 0) -> TakeEvery<decltype(std::begin(iterable))> {
        return takeeveryrange(std::begin(iterable), std::end(iterable), offset, start);
    }

}