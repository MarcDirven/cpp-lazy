#pragma once

#ifndef LZ_REGEX_SPLIT_HPP
#define LZ_REGEX_SPLIT_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/regex_split.hpp"

#include <regex>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class RegexTokenIter>
class regex_split_iterable final : public detail::basic_iterable<detail::regex_split_iterator<RegexTokenIter>, default_sentinel> {
public:
    using iterator = detail::regex_splitIterator<RegexTokenIter>;
    using const_iterator = iterator;
    using value_type = typename RegexTokenIter::value_type;

    constexpr regex_split_iterable(RegexTokenIter first, RegexTokenIter last) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(first), last)) {
    }

    constexpr regex_split_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Splits a string into parts based on a regular expression.
 *
 * @param s The string to split.
 * @param regex The regular expression to use for splitting the string.
 * @return A `regex_split_iterable` object that can be used to iterate over the parts of the string.
 */
template<class String>
regex_split_iterable<std::regex_token_iterator<typename String::const_iterator>>
regex_split(const String& s, const std::basic_regex<typename String::value_type>& regex) {
    using token_iter = std::regex_token_iterator<typename String::const_iterator>;
    token_iter first(s.begin(), s.end(), regex, -1);
    return { first, token_iter{} };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif