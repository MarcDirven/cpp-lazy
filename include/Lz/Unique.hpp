#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/UniqueIterator.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Unique final : public detail::BasicIteratorView<detail::UniqueIterator<Iterator>> {
    public:
        using iterator = detail::UniqueIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Creates an Unique iterator view object.
         * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be
         * implemented.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         */
        Unique(const Iterator begin, const Iterator end) :
            _begin(begin, end),
            _end(end, end) {
        }

        Unique() = default;

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const override {
            return _end;
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
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    LZ_REQUIRES_LESS_THAN(Iterator, Iterator)
    Unique<Iterator> uniqueRange(const Iterator begin, const Iterator end) {
        return Unique<Iterator>(begin, end);
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The iterable sequence.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class It = detail::IterType<Iterable>>
    LZ_REQUIRES_LESS_THAN(It, It)
    Unique<It> unique(Iterable&& iterable) {
        return uniqueRange(std::begin(iterable), std::end(iterable));
    }

    // End of group
    /**
     * @}
     */
}

#endif