#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include "detail/JoinIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Join final : public detail::BasicIteratorView<detail::JoinIterator<Iterator>> {
    public:
        using iterator = detail::JoinIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Creates a Join object.
         * @details Combines the iterator values followed by the delimiter. It is evaluated in a
         * `"[value][delimiter][value][delimiter]..."`-like fashion.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param delimiter The delimiter to separate the previous and the next values in the sequence.
         */
        Join(const Iterator begin, const Iterator end, std::string&& delimiter, typename iterator::difference_type difference) :
            _begin(begin, delimiter, true, difference),
            _end(end, std::move(delimiter), false, difference){
        }

        Join() = default;

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
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

    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Creates a Join object.
     * @details Combines the iterator values followed by the delimiter. It is evaluated in a
     * `"[value][delimiter][value][delimiter]..."`-like fashion.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param delimiter The delimiter to separate the previous and the next values in the sequence.
     * @return A Join iterator view object.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    Join<Iterator> joinRange(const Iterator begin, const Iterator end, std::string delimiter) {
        return Join<Iterator>(begin, end, std::move(delimiter), std::distance(begin, end) * 2 - 1);
    }

    /**
     * @brief Creates a Join object.
     * @details Combines the iterator values followed by the delimiter. It is evaluated in a
     * `"[value][delimiter][value][delimiter]..."`-like fashion.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The iterable to join with the delimiter.
     * @param delimiter The delimiter to separate the previous and the next values in the sequence.
     * @return A Join iterator view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    Join<detail::IterType<Iterable>> join(Iterable&& iterable, std::string delimiter) {
        return joinRange(std::begin(iterable), std::end(iterable), std::move(delimiter));
    }

    // End of group
    /**
     * @}
     */
}

#endif