#pragma once

#include "detail/BasicIteratorView.hpp"
#include "detail/UniqueIterator.hpp"


namespace lz {
    template<class Iterator>
    class Unique final : public detail::BasicIteratorView<detail::UniqueIterator<Iterator>> {
    public:
        using iterator = detail::UniqueIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        Iterator _begin{};
        Iterator _end{};

    public:
        /**
         * @brief Creates an Unique iterator view object.
         * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be
         * implemented.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         */
        Unique(const Iterator begin, const Iterator end):
            _begin(begin),
            _end(end)
        {
        }

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const {
            return iterator(_begin, _end);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const {
            return iterator(_end, _end);
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniquerange(...))` fashion.
     */
    template<class Iterator>
    Unique<Iterator> uniquerange(const Iterator begin, const Iterator end) {
        return Unique<Iterator>(begin, end);
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The iterable sequence.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
     */
    template<class Iterable>
    auto unique(Iterable&& iterable) -> Unique<decltype(std::begin(iterable))> {
        return uniquerange(std::begin(iterable), std::end(iterable));
    }

    // End of group
    /**
     * @}
     */
}