#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/SplitIterator.hpp"

namespace lz {
template<class SubString, class String, class StringType>
class StringSplitter final : public internal::BasicIteratorView<internal::SplitIterator<SubString, String, StringType>> {
public:
    using const_iterator = internal::SplitIterator<SubString, String, StringType>;
    using iterator = const_iterator;

public:
    using value_type = SubString;

    /**
     * @brief Creates a String splitter object. Its `begin()` and `end()` return an input iterator.
     * @param str The string to split.
     * @param delimiter The delimiter to split on.
     */
    LZ_CONSTEXPR_CXX_20 StringSplitter(const String& str, StringType delimiter) :
        internal::BasicIteratorView<iterator>(iterator(0, str, std::move(delimiter)), iterator(str.size(), str, StringType())) {
    }

    StringSplitter() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

#if defined(LZ_HAS_STRING_VIEW)
template<class SubString = std::string_view>
#elif defined(LZ_STANDALONE)
template<class SubString = std::string>
#else
template<class SubString = fmt::string_view>
#endif
/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * @tparam SubString The string type of the substring. If C++17, this will default to `std::string_view`. If `LZ_STANDALONE` is
 * not defined and C++17 is not defined, this will default to `std::string`. Otherwise it will default to `fmt::string_view`.
 * Furthermore, `SubString` should have a constructor which looks like `SubString([const]char*, std::size_t length)`:
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringSplitter<SubString, std::string, std::string>
split(const std::string& str, std::string delimiter) {
    return StringSplitter<SubString, std::string, std::string>(str, std::move(delimiter));
}

#if defined(LZ_HAS_STRING_VIEW)
template<class SubString = std::string_view>
#elif defined(LZ_STANDALONE)
template<class SubString = std::string>
#else
template<class SubString = fmt::string_view>
#endif
/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * @tparam SubString The string type of the substring. If C++17, this will default to `std::string_view`. If `LZ_STANDALONE` is
 * not defined and C++17 is not defined, this will default to `std::string`. Otherwise it will default to `fmt::string_view`.
 * Furthermore, `SubString` should have a constructor which looks like `SubString([const]char*, std::size_t length)`:
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringSplitter<SubString, std::string, char> split(const std::string& str, char delimiter) {
    return StringSplitter<SubString, std::string, char>(str, delimiter);
}

#if defined(LZ_HAS_STRING_VIEW)
template<class SubString = std::string_view>
#elif defined(LZ_STANDALONE)
template<class SubString = std::string>
#else
template<class SubString = fmt::string_view>
#endif
StringSplitter<SubString, std::string, char> split(std::string&& str, char delimiter) = delete;

#if defined(LZ_HAS_STRING_VIEW)
template<class SubString = std::string_view>
#elif defined(LZ_STANDALONE)
template<class SubString = std::string>
#else
template<class SubString = fmt::string_view>
#endif
StringSplitter<SubString, std::string, std::string> split(std::string&& str, std::string delimiter) = delete;

#ifdef LZ_HAS_STRING_VIEW
/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * @tparam SubString The string type of the substring.
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
template<class SubString = std::string_view>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringSplitter<SubString, std::string_view, std::string>
split(const std::string_view& str, std::string delimiter) {
    return StringSplitter<SubString, std::string_view, std::string>(str, std::move(delimiter));
}

template<class SubString = std::string_view>
LZ_NODISCARD constexpr StringSplitter<SubString, std::string_view, std::string>
split(const std::string_view& str, char delimiter) {
    return StringSplitter<SubString, std::string_view, char>(str, delimiter);
}

template<class SubString = std::string_view>
StringSplitter<SubString, std::string_view, std::string> split(std::string_view&& str, char delimiter) = delete;

template<class SubString = std::string_view>
StringSplitter<SubString, std::string_view, std::string> split(std::string_view&& str, std::string delimiter) = delete;
#endif // LZ_HAS_STRING_VIEW

// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP