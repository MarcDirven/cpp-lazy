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
	StringSplitter(const String& str, std::string&& delimiter) :
		internal::BasicIteratorView<iterator>(iterator(0, str, delimiter),
											  iterator(str.size(), str, delimiter)) {
	}

	StringSplitter() = default;
};


template
class StringSplitter<std::string, std::string>;


#ifdef LZ_HAS_STRING_VIEW


template
class StringSplitter<std::string_view, std::string_view>;


template<class SubString = std::string_view, class String = std::string_view>
#else
template<class SubString = fmt::string_view, class String = std::string>
#endif
// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This is a lazy evaluated string splitter function. If not using C++17 or higher, you can use `std::move`
 * to safely move the substring, that is returned by the
 * `StringSplitter<SubString>::const_iterator::operator*`. Its `begin()` and `end()` return an input iterator.
 * The StringSplitter view object may not outlive its iterator i.e. they must have the same lifetime.
 * @attention This object keeps a reference to a copy of `str`. This is illegal C++:
 * ```
 * lz::StringSplitter<std::string, std::string> f() {
 * 		return lz::split(std::string("hello, world!"), ", ");
 * }
 * std::cout << *f().begin() << '\n';
 * ```
 * This will most likely crash your application.
 * @tparam SubString The type that gets returned when the `StringSplitter<SubString>::const_iterator::operator*` is
 * called. Can be specified, but if C++17 or higher is defined, `std::string_view` is used, otherwise `std::string`.
 * @param str The string to split.
 * @param delimiter The delimiter to split on.
 * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::split(...))`.
 */
StringSplitter<SubString, String> split(const String& str, std::string delimiter) {
	return StringSplitter<SubString, String>(str, std::move(delimiter));
}

// End of group
/**
 * @}
 */
}

#endif