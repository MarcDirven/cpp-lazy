#pragma once


#include <Lz/detail/ConcatenateIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator1, class Iterator2>
    class Concatenate final : public detail::BasicIteratorView<detail::ConcatenateIterator<Iterator1, Iterator2>> {
    public:
        using iterator = detail::ConcatenateIterator<Iterator1, Iterator2>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Concatenate constructor.
         * @param beginIterator1 The beginning of the first iterator.
         * @param endIterator1 The ending of the first iterator.
         * @param beginIterator2 The beginning of the second iterator.
         * @param endIterator2 The ending of the second iterator.
         */
        Concatenate(const Iterator1 beginIterator1, const Iterator1 endIterator1, const Iterator2 beginIterator2,
                    const Iterator2 endIterator2) :
            _begin(beginIterator1, endIterator1, beginIterator2, endIterator2),
            _end(endIterator1, endIterator1, endIterator2, endIterator2) {
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
     * @brief Creates a Concatenate view object, which contains the random access iterator.
     * @tparam Iterator1 Is automatically deduced.
     * @tparam Iterator2 Is automatically deduced.
     * @param begin1 The beginning of the first iterator.
     * @param end1 The ending of the first iterator.
     * @param begin2 The beginning of the second iterator.
     * @param end2 The ending of the second iterator.
     * @return A concatenate view object which can be iterated over.
     */
    template<class Iterator1, class Iterator2>
    auto concatrange(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2) {
        return Concatenate<Iterator1, Iterator2>(begin1, end1, begin2, end2);
    }

    /**
     * @brief Creates a Concatenate view object, which contains the random access iterator.
     * @param a The first container.
     * @param b The second container.
     * @tparam Iterable1 Is automatically deduced.
     * @tparam Iterable2 Is automatically deduced.
     * @return A concatenate view object which can be iterated over.
     */
    template<class Iterable1, class Iterable2>
    auto concat(Iterable1&& a, Iterable2&& b) {
        return concatrange(std::begin(a), std::end(a), std::begin(b), std::end(b));
    }
}