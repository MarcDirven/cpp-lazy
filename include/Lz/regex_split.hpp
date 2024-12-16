#pragma once

#ifndef LZ_REGEX_SPLIT_HPP
#define LZ_REGEX_SPLIT_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/RegexSplitIterator.hpp"

#include <regex>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class RegexTokenIter>
class RegexSplit final : public detail::BasicIterable<detail::RegexSplitIterator<RegexTokenIter>, default_sentinel> {
public:
    using iterator = detail::RegexSplitIterator<RegexTokenIter>;
    using const_iterator = iterator;
    using value_type = typename RegexTokenIter::value_type;

    constexpr RegexSplit(RegexTokenIter first, RegexTokenIter last) :
        detail::BasicIterable<iterator, default_sentinel>(iterator(std::move(first), last)) {
    }

    constexpr RegexSplit() = default;
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
 * @return A `RegexSplit` object that can be used to iterate over the parts of the string.
 */
template<class String>
RegexSplit<std::regex_token_iterator<typename String::const_iterator>>
regexSplit(const String& s, const std::basic_regex<typename String::value_type>& regex) {
    using TokenIter = std::regex_token_iterator<typename String::const_iterator>;
    TokenIter first(s.begin(), s.end(), regex, -1);
    return { first, TokenIter{} };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif