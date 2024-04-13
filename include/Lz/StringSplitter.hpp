#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/SplitIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class SubString, class String, class DelimiterString>
class StringSplitter final : public internal::BasicIteratorView<internal::SplitIterator<SubString, String, DelimiterString>> {
public:
    using const_iterator = internal::SplitIterator<SubString, String, DelimiterString>;
    using iterator = const_iterator;

public:
    using value_type = SubString;

    LZ_CONSTEXPR_CXX_20 StringSplitter(const String& str, DelimiterString delimiter, std::size_t delimiterLength) :
        internal::BasicIteratorView<iterator>(iterator(0, str, delimiter, delimiterLength),
                                              iterator(str.size(), str, delimiter, delimiterLength)) {
    }

    StringSplitter() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * @tparam SubString The string type of the substring. (default lz::StringView)
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
template<class SubString = StringView, class String, class DelimiterString>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringSplitter<SubString, String, DelimiterString>
split(const String& str, DelimiterString delimiter) {
    const auto delimiterLength = internal::getDelimiterLength(delimiter);
    return { str, std::move(delimiter), delimiterLength };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP