#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/JoinIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator>
class Join final : public internal::BasicIteratorView<internal::JoinIterator<Iterator>> {
public:
    using iterator = internal::JoinIterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Join(Iterator begin, Iterator end, std::string delimiter, const internal::DiffType<Iterator> difference) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), delimiter, true, difference),
                                              iterator(std::move(end), delimiter, false, difference)) {
    }

    Join() = default;

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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<Iterator> joinRange(Iterator begin, Iterator end, std::string delimiter) {
    using lz::distance;
    using std::distance;
    return { std::move(begin), std::move(end), std::move(delimiter), distance(begin, end) * 2 - 1 };
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<internal::IterTypeFromIterable<Iterable>> join(Iterable&& iterable, std::string delimiter) {
    return joinRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                     std::move(delimiter));
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator Join object.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @param delimiter The delimiter to separate each value from the sequence.
 * @param execution The execution policy. Must be one of std::execution::* policies.
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<class Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
#ifdef LZ_HAS_STRING_VIEW
strJoinRange(Iterator begin, Iterator end, const std::string_view delimiter = "") {
#else
strJoinRange(Iterator begin, Iterator end, const std::string& delimiter = "") {
#endif // LZ_HAS_STRING_VIEW
    return internal::BasicIteratorView<Iterator>(std::move(begin), std::move(end)).toString(delimiter);
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator Join object.
 * @param iterable The iterable to convert to string
 * @param delimiter The delimiter to separate each value from the sequence.
 * @param execution The execution policy. Must be one of std::execution::* policies.
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
#ifdef LZ_HAS_STRING_VIEW
strJoin(Iterable&& iterable, const std::string_view delimiter = "") {
#else
strJoin(Iterable&& iterable, const std::string& delimiter = "") {
#endif // LZ_HAS_STRING_VIEW
    return strJoinRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        delimiter);
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif