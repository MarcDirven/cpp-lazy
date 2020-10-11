#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include "detail/JoinIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Join final : public internal::BasicIteratorView<internal::JoinIterator<Iterator>> {
    public:
        using iterator = internal::JoinIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

        /**
         * @brief Creates a Join object.
         * @details Combines the iterator values followed by the delimiter. It is evaluated in a
         * `"[value][delimiter][value][delimiter]..."`-like fashion.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param delimiter The delimiter to separate the previous and the next values in the sequence.
         * @param difference The difference between `begin` and `end`
         */
        Join(const Iterator begin, const Iterator end, std::string delimiter, typename iterator::difference_type difference) :
            internal::BasicIteratorView<iterator>(iterator(begin, delimiter, true, difference),
                                                  iterator(end, delimiter, false, difference))
        {
        }

        Join() = default;

        /**
         * Because a join iterator already has a delimiter, an additional overload is necessary, where the delimiter is defaulted to blank.
         * @param o The output stream.
         * @param it The Join object to print.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& o, const Join<Iterator>& it) {
            // Join already has a delimiter, default to blank string
            return o << it.toString();
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
     * @param iterable The iterable to join with the delimiter.
     * @param delimiter The delimiter to separate the previous and the next values in the sequence.
     * @return A Join iterator view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    Join<internal::IterTypeFromIterable<Iterable>> join(Iterable&& iterable, std::string delimiter) {
        return joinRange(std::begin(iterable), std::end(iterable), std::move(delimiter));
    }

    // End of group
    /**
     * @}
     */
}

#endif