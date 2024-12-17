#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/join.hpp"

// clang-format off
#if defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT)
#include <format>
#elif defined(LZ_STANDALONE)
#include <ostream>
#else
#include <fmt/core.h>
#endif
// clang-format on

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S>
class join_iterable final
    : public detail::basic_iterable<detail::join_iterator<Iterator, S>, typename detail::join_iterator<Iterator, S>::sentinel> {

#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
    LZ_CONSTEXPR_CXX_20 join_iterable(Iterator begin, S end, std::string delimiter, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, S>(iterator(std::move(begin), std::move(delimiter), true), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 join_iterable(Iterator begin, S end, std::string delimiter, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(std::move(begin), delimiter, true),
                                         iterator(std::move(end), delimiter, false)) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    join_iterable(Iterator begin, S end, std::string delimiter, std::string fmt, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, S>(iterator(std::move(begin), std::move(delimiter), std::move(fmt), true),
                                            std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20
    join_iterable(Iterator begin, S end, std::string delimiter, std::string fmt, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(std::move(begin), delimiter, fmt, true),
                                         iterator(std::move(end), delimiter, fmt, false)) {
    }
#endif // has format

public:
    using iterator = detail::join_iterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
    LZ_CONSTEXPR_CXX_20 join_iterable(Iterator begin, S end, std::string delimiter) :
        join_iterable(std::move(begin), std::move(end), std::move(delimiter), iter_cat_t<Iterator>{}) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    join_iterable(Iterator begin, S end, std::string delimiter, std::string fmt) :
        join_iterable(std::move(begin), std::move(end), std::move(delimiter), std::move(fmt), iter_cat_t<Iterator>{}) {
    }
#endif // has format

    join_iterable() = default;

    friend std::ostream& operator<<(std::ostream& o, const join_iterable<Iterator, S>& join_iter) {
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
        detail::stringify(join_iter, std::ostream_iterator<typename join_iterable<Iterator, S>::value_type::value_type>(o), "",
                          "{}");
#else
        detail::stringify(join_iter, o, "");
#endif
        return o;
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
/**
 * @brief Creates a join_iterable object.
 * @note If you're going to call .to_string() on this, it is better to use `str_join` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param iterable The iterable to join with the delimiter.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @return A join_iterable iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
join(Iterable&& iterable, std::string delimiter) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(delimiter) };
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator join_iterable object.
 * @param iterable The iterable to convert to string
 * @param delimiter The delimiter to separate each value from the sequence.
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
std::string str_join(Iterable&& iterable, const char* delimiter = "") {
    // clang-format off
    return detail::basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>(
        detail::begin(std::forward<Iterable>(iterable)),detail::end(std::forward<Iterable>(iterable)))
        .to_string(delimiter);
    // clang-format on
}

#else

/**
 * @brief Creates a join_iterable object.
 * @note If you're going to call .to_string() on this, it is better to use `str_join` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param iterable The iterable to join with the delimiter.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @param fmt The std:: or fmt:: formatting args (`"{}"` is default).
 * @return A join_iterable iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
join(Iterable&& iterable, std::string delimiter, std::string fmt = "{}") {
    return { std::begin(iterable), std::end(iterable), std::move(delimiter), std::move(fmt) };
}

/**
 * Converts a sequence to a `std::string` without creating an iterator join_iterable object.
 * @param iterable The iterable to convert to string
 * @param delimiter The delimiter to separate each value from the sequence.
 * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
std::string str_join(Iterable&& iterable, std::string delimiter = "", std::string fmt = "{}") {
    // clang-format off
    return detail::basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>(
        detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)))
            .to_string(std::move(delimiter), std::move(fmt));
    // clang-format on
}
#endif // has format

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif