#pragma once

#ifndef LZ_REGEX_SPLIT_HPP
#define LZ_REGEX_SPLIT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/RegexSplitIterator.hpp"

#include <regex>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class RegexTokenIter>
class RegexSplit final : public internal::BasicIteratorView<internal::RegexSplitIterator<RegexTokenIter>> {
public:
    using iterator = internal::RegexSplitIterator<RegexTokenIter>;
    using const_iterator = iterator;
    using value_type = typename RegexTokenIter::value_type;

    constexpr RegexSplit(RegexTokenIter first, RegexTokenIter last) :
        internal::BasicIteratorView<iterator>(iterator(std::move(first), last), iterator(last, last)) {
    }

    constexpr RegexSplit() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Splits a range of elements defined by the iterators using a regular expression.
 *
 * @param first An iterator pointing to the first element of the range.
 * @param last An iterator pointing to the past-the-end element of the range.
 * @return A `RegexSplit` object representing the split range.
 * @note @p RegexTokenIter be compatible with `std::regex_token_iterator`.
 */
template<class RegexTokenIter>
LZ_NODISCARD constexpr RegexSplit<RegexTokenIter> regexSplitRange(RegexTokenIter first, RegexTokenIter last) {
    return { std::move(first), std::move(last) };
}

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
    return regexSplitRange(first, TokenIter());
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif