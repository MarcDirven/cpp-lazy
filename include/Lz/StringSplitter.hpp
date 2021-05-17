#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include "detail/SplitIterator.hpp"
#include "detail/BasicIteratorView.hpp"

namespace lz {
template<class SubString, class String>
class StringSplitter final : public internal::BasicIteratorView<internal::SplitIterator<SubString, String>> {
public:
	using const_iterator = internal::SplitIterator<SubString, String>;
	using iterator = const_iterator;

public:
	using value_type = SubString;

	/**
	 * @brief Creates a String splitter object. Its `begin()` and `end()` return an input iterator.
	 * @param str The string to split.
	 * @param delimiter The delimiter to split on.
	 */
	StringSplitter(String& str, std::string delimiter) :
		internal::BasicIteratorView<iterator>(iterator(0, str, std::move(delimiter)), iterator(str.size(), str, "")) {
	}

	StringSplitter() = default;
};

#ifdef LZ_HAS_STRING_VIEW

template
class StringSplitter<std::string_view, std::string>;

template
class StringSplitter<std::string_view, std::string_view>;

template<class SubString = std::string_view>
#elif !defined(LZ_STANDALONE)
template class StringSplitter<fmt::string_view, std::string>;

template<class SubString = fmt::string_view>
#else
template class StringSplitter<std::string, std::string>;

template<class SubString = std::string>
#endif

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * Its `begin()` and `end()` return an forward iterator.
 * This will most likely crash your application.
 * @tparam SubString The string type of the substring. If C++17, this will default to `std::string_view`. If `LZ_STANDALONE` is not defined
 * and C++17 is not defined, this will default to `std::string`. Otherwise it will default to `fmt::string_view`. Furthermore, `SubString`
 * should have a constructor which looks like `SubString([const]char*, std::size_t length)`:
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
StringSplitter<SubString, std::string> split(std::string& str, std::string delimiter) {
	return StringSplitter<SubString, std::string>(str, std::move(delimiter));
}

#ifdef LZ_HAS_STRING_VIEW

/**
 * @brief This is a lazy evaluated string splitter function. It splits a string using `delimiter`.
 * Its `begin()` and `end()` return an forward iterator.
 * This will most likely crash your application.
 * @tparam SubString The string type of the substring.
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
template<class SubString = std::string_view>
StringSplitter<SubString, std::string_view> split(std::string_view& str, std::string delimiter) {
	return StringSplitter<SubString, std::string_view>(str, std::move(delimiter));
}

#endif


// End of group
/**
 * @}
 */
}

#endif