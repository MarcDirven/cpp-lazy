#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <vector>
#include <array>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>

#if defined(LZ_STANDALONE)
  #ifdef LZ_HAS_FORMAT
	#include <format>
  #else
	#include <sstream>
  #endif // LZ_HAS_FORMAT
#else
  #include "fmt/ostream.h"
#endif // LZ_STANDALONE

#include "LzTools.hpp"

namespace lz {
namespace internal {
#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
template<class Iterator>
  #if defined(LZ_HAS_STRING_VIEW)
std::string toStringImplSpecialized(Iterator begin, Iterator end, const std::string_view delimiter, std::true_type /* isArithmetic */) {
  #else
std::string toStringImplSpecialized(Iterator begin, Iterator end, const std::string& delimiter, std::true_type /* isArithmetic */) {
  #endif // defined(LZ_HAS_STRING_VIEW)
	std::string result;
	using Arithmetic = ValueType<Iterator>;
	std::for_each(begin, end, [&result, &delimiter](const Arithmetic a) {
		result += std::to_string(a);
		result += delimiter;
	});
	return result;
}

template<class Iterator>
#if defined(LZ_HAS_STRING_VIEW)
std::string toStringImplSpecialized(Iterator begin, Iterator end, const std::string_view delimiter, std::false_type /* isArithmetic */) {
#else
std::string toStringImplSpecialized(Iterator begin, Iterator end, const std::string& delimiter, std::false_type /* isArithmetic */) {
#endif // defined(LZ_HAS_STRING_VIEW)
	std::ostringstream oss;
	std::for_each(begin, end, [&oss, &delimiter](const ValueType<Iterator>& t) {
		oss << t << delimiter;
	});
	return oss.str();
}

#endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)

#ifdef LZ_HAS_EXECUTION
template<class Iterator, class Execution>
LZ_CONSTEXPR_CXX_20 std::string toStringImpl(Iterator begin, Iterator end, const std::string_view delimiter, Execution execution) {
#else

template<class Iterator>
std::string toStringImpl(Iterator begin, Iterator end, const std::string& delimiter) {
#endif // LZ_HAS_EXECUTION
	using TValueType = ValueType<Iterator>;
	if (begin == end) { return ""; }
	std::string result;
#if defined(LZ_HAS_FORMAT) || (!defined(LZ_STANDALONE)) // std::format_to or fmt::format_to backInserter
	auto backInserter = std::back_inserter(result);
#endif // LZ_STANDALONE

#ifdef LZ_HAS_EXECUTION
	if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT)) // std::ostringstream, std::to_string is used
		result = toStringImplSpecialized(begin, end, delimiter, std::is_arithmetic<TValueType>());
#elif defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT) // std::format_to is used
		std::for_each(begin, end, [delimiter, backInserter](const TValueType& v) {
			std::format_to(backInserter, "{}{}", v, delimiter);
		});
#else // fmt::format_to is used
		std::for_each(begin, end, [delimiter, backInserter](const TValueType& v) {
			fmt::format_to(backInserter, FMT_STRING("{}{}"), v, delimiter);
		});
#endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
	} // end if check if std::execution::seq
	else {
		std::mutex m;
#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT) // Use std::ostringstream, std::to_string
		if constexpr (std::is_arithmetic_v<TValueType>) {
			std::for_each(execution, begin, end, [&result, delimiter, &m](const TValueType v) {
				std::lock_guard guard(m);
				result += std::to_string(v);
				result += delimiter;
			});
		}
		else {
			std::ostringstream oss;
			std::for_each(execution, begin, end, [delimiter, &oss, &m](const TValueType& v) {
				std::lock_guard guard(m);
				oss << v << delimiter;
			});
			result = oss.str();
		}
#elif defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT) // std::format_to is used
		std::for_each(execution, begin, end, [delimiter, backInserter, &m](const TValueType& v) {
			std::lock_guard guard(m);
			std::format_to(backInserter, "{}{}", v, delimiter);
		});
#else // fmt::format_to is used
		std::for_each(execution, begin, end, [delimiter, &m, backInserter](const TValueType& v) {
			std::lock_guard guard(m);
			fmt::format_to(backInserter, FMT_STRING("{}{}"), v, delimiter);
		});
#endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
	} // end if check if higher than std::execution::seq
#else // ^^^ LZ_HAS_EXECUTION vvv !LZ_HAS_EXECUTION
  #if !defined(LZ_STANDALONE)
	std::for_each(begin, end, [&delimiter, backInserter](const TValueType& v) {
		fmt::format_to(backInserter, FMT_STRING("{}{}"), v, delimiter);
	});
  #else // ^^^ !LZ_STANDALONE vvv LZ_STANDALONE
	result = toStringImplSpecialized(begin, end, delimiter, std::is_arithmetic<TValueType>());
  #endif // !defined(LZ_STANDALONE)
#endif // LZ_HAS_EXECUTION
	const auto resultEnd = result.end();
	result.erase(resultEnd - static_cast<std::ptrdiff_t>(delimiter.length()), resultEnd);
	return result;
}

using One = char;
struct Two {
	char x[2];
};

template<class Container>
class HasResize {
	template<class C>
	static constexpr One test(decltype(void(std::declval<C&>().resize(0)))*) { return {}; }

	template<class C>
	static constexpr Two test(...) { return {}; }

public:
	enum {
		value = sizeof(test<Container>(nullptr)) == sizeof(One)
	};
};

template<class Container>
class HasReserve {
	template<class C>
	static constexpr One test(decltype(void(std::declval<C&>().reserve(0)))*) { return {}; }

	template<class C>
	static constexpr Two test(...) { return {}; }

public:
	enum {
		value = sizeof(test<Container>(nullptr)) == sizeof(One)
	};
};

template<class LzIterator>
class BasicIteratorView {
protected:
	LzIterator _begin{};
	LzIterator _end{};

public:
	using value_type = internal::ValueType<LzIterator>;
	using iterator = LzIterator;
	using const_iterator = iterator;

private:
	template<class KeySelectorFunc>
	using KeyType = FunctionReturnType<KeySelectorFunc, internal::RefType<LzIterator>>;

	template<class Container, class It = LzIterator, EnableIf<!HasReserve<Container>::value || !IsForward<It>::value, bool> = true>
	LZ_CONSTEXPR_CXX_20 void tryReserve(Container&) const {}

	template<class Container, class It = LzIterator, EnableIf<HasReserve<Container>::value && IsForward<It>::value, bool> = true>
	LZ_CONSTEXPR_CXX_20 void tryReserve(Container& container) const {
		using lz::distance; using std::distance;
		container.reserve(distance(begin(), end()));
	}

#if defined(LZ_GCC_VERSION) && (__GNUC__ == 4) && (__GNUC__MINOR__ < 9)
	template<class MapType, class KeySelectorFunc>
	MapType createMap(const KeySelectorFunc keyGen) const {
		MapType map;
#else
	template<class MapType, class Allocator, class KeySelectorFunc>
	LZ_CONSTEXPR_CXX_20 MapType createMap(const KeySelectorFunc keyGen, const Allocator& allocator) const {
		MapType map(allocator);
		tryReserve(map);
#endif // END LZ_GCC_VERSION
#ifdef LZ_HAS_CXX_11
		std::transform(begin(), end(), std::inserter(map, map.end()), [keyGen](internal::RefType<LzIterator> value) {
			return std::make_pair(keyGen(value), value);
#else
		std::transform(begin(), end(), std::inserter(map, map.end()), [keyGen](auto&& value) {
			return std::make_pair(keyGen(std::forward<decltype(value)>(value)), value);
#endif // LZ_HAS_CXX_11
		});
		return map;
	}

  #ifdef LZ_HAS_EXECUTION

	template<class Container, class... Args, class Execution>
	LZ_CONSTEXPR_CXX_20 Container copyContainer(Execution execution, Args&& ... args) const {
		using lz::distance; using std::distance;
		Container cont(std::forward<Args>(args)...);
		// Prevent static assertion
		if constexpr (internal::checkForwardAndPolicies<Execution, LzIterator>()) {
			static_cast<void>(execution);
			tryReserve(cont);
			// If parallel execution, compilers throw an error if it's std::execution::seq. Use an output iterator to fill the contents.
			// Here we can freely use std::inserter (output iterator)
			std::copy(begin(), end(), std::inserter(cont, cont.begin()));
		}
		else {
			static_assert(HasResize<Container>::value, "Container needs to have a method resize() in order to use parallel algorithms."
													   " Use std::execution::seq instead");
			const auto iterLength = static_cast<std::size_t>(distance(begin(), end()));
			if (iterLength > cont.size()) {
				// Container's iterator needs to be a bidirectional iterator at least, std::inserter will not suffice
				cont.resize(iterLength);
			}
			std::copy(execution, begin(), end(), cont.begin());
		}
		return cont;
	}

	template<std::size_t N, class Execution>
	LZ_CONSTEXPR_CXX_20 std::array<value_type, N> copyArray(Execution execution) const {
		LZ_ASSERT(std::distance(begin(), end()) <= static_cast<internal::DiffType<LzIterator>>(N),
				  "the iterator size is too large and/or array size is too small");
		std::array<value_type, N> array{};
		if constexpr (internal::checkForwardAndPolicies<Execution, LzIterator>()) {
			static_cast<void>(execution);
			std::copy(begin(), end(), array.begin());
		}
		else {
			std::copy(execution, begin(), end(), array.begin());
		}
		return array;
	}

  #else // ^^^ has execution vvv ! has execution

	template<class Container, class Iter>
	Container makeContainer(Container& container, Iter where) const {
		std::copy(begin(), end(), std::move(where));
		return container;
	}

	template<class Container, class... Args>
	Container copyContainer(Args&& ... args) const {
		Container cont(std::forward<Args>(args)...);
		tryReserve(cont);
		std::copy(begin(), end(), std::inserter(cont, cont.begin()));
		return cont;
	}

	template<std::size_t N>
	std::array<value_type, N> copyArray() const {
		LZ_ASSERT(std::distance(begin(), end()) <= static_cast<internal::DiffType<LzIterator>>(N),
				  "the iterator size is too large and/or array size is too small");
		auto array = std::array<value_type, N>();
		std::copy(begin(), end(), array.begin());
		return array;
	}

  #endif // LZ_HAS_EXECUTION
public:
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator begin() LZ_CONST_REF_QUALIFIER noexcept {
		return _begin;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator end() LZ_CONST_REF_QUALIFIER noexcept {
		return _end;
	}

  #ifdef LZ_HAS_REF_QUALIFIER

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator begin()&& noexcept {
		return std::move(_begin);
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 virtual LzIterator end()&& noexcept {
		return std::move(_end);
	}

  #endif // LZ_HAS_REF_QUALIFIER

	constexpr BasicIteratorView() = default;

	constexpr BasicIteratorView(LzIterator begin, LzIterator end) :
		_begin(std::move(begin)),
		_end(std::move(end)) {}

	virtual ~BasicIteratorView() = default;

  #ifdef LZ_HAS_EXECUTION

	/**
	 * @brief Returns an arbitrary container type, of which its constructor signature looks like:
	 * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
	 * the typedef `value_type`.
	 * @details Use this function to convert the iterator to a container. Example:
	 * ```cpp
	 * auto list = lazyIterator.to<std::list>();
	 * auto allocator = std::allocator<int>();
	 * auto set = lazyIterator.to<std::set>(allocator);
	 * ```
	 * @param execution The execution policy. Must be one of `std::execution`'s tags.
	 * @tparam Args Additional arguments, automatically deduced.
	 * @param args Additional arguments, for e.g. an allocator.
	 * @return An arbitrary container specified by the entered template parameter.
	 */
	template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container<value_type, Args...> to(Execution execution = std::execution::seq, Args&& ... args) const {
		using Cont = Container<value_type, Args...>;
		return copyContainer<Cont>(execution, std::forward<Args>(args)...);
	}

	/**
	* @brief Creates a new `std::vector<value_type>` of the sequence.
	* @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
	* @param execution The execution policy. Must be one of `std::execution`'s tags.
	* @return A `std::vector<value_type>` with the sequence.
	*/
	template<class Execution = std::execution::sequenced_policy>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type> toVector(Execution execution = std::execution::seq) const {
		return to<std::vector>(execution);
	}

	/**
	 * @brief Creates a new `std::vector<value_type, Allocator>`.
	 * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
	 * by this function.
	 * @param execution The execution policy. Must be one of `std::execution`'s tags.
	 * @param alloc The allocator.
	 * @return A new `std::vector<value_type, Allocator>`.
	 */
	template<class Allocator, class Execution = std::execution::sequenced_policy>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type, Allocator> toVector(const Allocator& alloc, Execution execution) const {
		return to<std::vector>(execution, alloc);
	}

	/**
	 * @brief Creates a new `std::vector<value_type, N>`.
	 * @tparam N The size of the array.
	 * @param execution The execution policy. Must be one of `std::execution`'s tags.
	 * @return A new `std::array<value_type, N>`.
	 * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
	 */
	template<std::size_t N, class Execution = std::execution::sequenced_policy>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::array<value_type, N> toArray(Execution execution = std::execution::seq) const {
		return copyArray<N>(execution);
	}

	/**
	 * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
	 * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
	 * @param delimiter The delimiter between the previous value and the next.
	 * @param execution The execution policy. Must be one of `std::execution`'s tags.
	 * @return The converted iterator in string format.
	 */
	template<class Execution = std::execution::sequenced_policy>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
	toString(const std::string_view delimiter = "", Execution execution = std::execution::seq) const {
		return toStringImpl(begin(), end(), delimiter, execution);
	}

  #else

	/**
	 * @brief Returns an arbitrary container type, of which its constructor signature looks like:
	 * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the sequence is equal to
	 * the typedef `value_type`.
	 * @details Use this function to convert the iterator to a container. Example:
	 * ```cpp
	 * auto list = lazyIterator.to<std::list>();
	 * auto allocator = std::allocator<int>();
	 * auto set = lazyIterator.to<std::set>(allocator);
	 * ```
	 * @tparam Args Additional arguments, automatically deduced
	 * @param args Additional arguments, for e.g. an allocator.
	 * @return An arbitrary container specified by the entered template parameter.
	 */
	template<template<class, class...> class Container, class... Args>
	Container<value_type, Args...> to(Args&& ... args) const {
		using Cont = Container<value_type, Args...>;
		return copyContainer<Cont>(std::forward<Args>(args)...);
	}

	/**
	 * @brief Creates a new `std::vector<value_type>` of the sequence.
	 * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
	 * @return A `std::vector<value_type>` with the sequence.
	 */
	std::vector<value_type> toVector() const {
		return to<std::vector>();
	}

	/**
	 * @brief Creates a new `std::vector<value_type, Allocator>`.
	 * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
	 * by this function.
	 * @param alloc The allocator
	 * @return A new `std::vector<value_type, Allocator>`.
	 */
	template<class Allocator>
	std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
		return to<std::vector, Allocator>(alloc);
	}

	/**
	 * @brief Creates a new `std::vector<value_type, N>`.
	 * @tparam N The size of the array
	 * @return A new `std::array<value_type, N>`.
	 * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
	 */
	template<std::size_t N>
	std::array<value_type, N> toArray() const {
		return copyArray<N>();
	}

	/**
	 * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
	 * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
	 * @param delimiter The delimiter between the previous value and the next.
	 * @return The converted iterator in string format.
	 */
	std::string toString(const std::string& delimiter = "") const {
		return toStringImpl(begin(), end(), delimiter);
	}

  #endif // LZ_HAS_EXECUTION

	/**
	 * @brief Creates a new `std::map<Key, value_type[, Compare[, Allocator]]>`.
	 * @details Creates a new `std::map<Key, value_type[, Compare[, Allocator]]>`. Example:
	 * ```cpp
	 * std::vector<std::string> sequence = { "abc", "def", "ghi" };
	 * auto someLazyViewIterator = lz::SomeLazyViewIterator(sequence); // value_type = std::string
	 * std::map<char, std::string> map = someLazyViewIterator.toMap([](const std::string& s) {
	 *      return s[0]; // Return the dict key, first char of the string
	 * });
	 * // map yields:
	 * // 'a' : "abc"
	 * // 'd' : "def"
	 * // 'g' : "ghi"
	 * ```
	 * @tparam Compare Can be used for the STL `std::map` ordering, default is `std::less<Key>`.
	 * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
	 * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
	 * @param allocator Optional, can be used for using a custom allocator.
	 * @return A `std::map<Key, value_type[, Compare[, Allocator]]>`
	 */
	template<class KeySelectorFunc,
		class Compare = std::less<KeyType<KeySelectorFunc>>,
		class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>
#if defined(LZ_GCC_VERSION) && LZ_GCC_VERSION < 5
	toMap(const KeySelectorFunc keyGen) const {
		using Map = std::map<KeyType<KeySelectorFunc>, T, Compare, Allocator>;
		return createMap<Map>(keyGen);
#else // ^^^gcc < 5 vvv gcc >= 5
	toMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const {
		using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
		return createMap<Map>(keyGen, allocator);
#endif // end lz gcc version < 5
	}

	/**
	 * @brief Creates a new `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`.
	 * @details Creates a new `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`. Example:
	 * ```cpp
	 * std::vector<std::string> sequence = { "abc", "def", "ghi" };
	 * auto someLazyViewIterator = lz::SomeLazyViewIterator(sequence); // value_type = std::string
	 * std::unordered_map<char, std::string> uMap = someLazyViewIterator.toMap([](const std::string& s) {
	 *      return s[0]; // Return the dict key, first char of the string
	 * });
	 * // uMap yields:
	 * // 'a' : "abc"
	 * // 'd' : "def"
	 * // 'g' : "ghi"
	 * ```
	 * @tparam Hasher The hash function, `std::hash<Key>` is used by default
	 * @tparam KeyEquality Key equality checker. `std::equal_to<Key>` is used by default.
	 * @tparam Allocator Can be used for the STL `std::map` allocator. Default is `std::allocator`.
	 * @param keyGen The function that returns the key for the dictionary, and takes a `value_type` as parameter.
	 * @param allocator Optional, can be used for using a custom allocator.
	 * @return A `std::unordered_map<Key, value_type[, Hasher[, KeyEquality[, Allocator]]]>`
	 */
	template<class KeySelectorFunc,
		class Hasher = std::hash<KeyType<KeySelectorFunc>>,
		class KeyEquality = std::equal_to<KeyType<KeySelectorFunc>>,
		class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>
#if defined(LZ_GCC_VERSION) && LZ_GCC_VERSION < 5
	toUnorderedMap(const KeySelectorFunc keyGen) const {
		using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality>;
		return createMap<UnorderedMap>(keyGen);
#else // ^^^gcc < 5 vvv gcc >= 5
	toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& allocator = Allocator()) const {
		using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>;
		return createMap<UnorderedMap>(keyGen, allocator);
#endif // end lz gcc version < 5
	}

	/**
	 * Function to stream the iterator to an output stream e.g. `std::cout`.
	 * @param o The stream object.
	 * @param it The iterator to print.
	 * @return The stream object by reference.
	 */
	friend std::ostream& operator<<(std::ostream& o, const BasicIteratorView<LzIterator>& it) {
		return o << it.toString(" ");
	}
};
} // namespace internal
#ifndef LZ_HAS_EXECUTION

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<internal::ValueTypeIterable<IterableA>>>
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
	return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<internal::ValueTypeIterable<IterableA>>>
bool notEqual(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
	return !lz::equal(a, b, std::move(predicate));
}

#else // ^^^ !LZ_HAS_EXECUTION vvv LZ_HAS_EXECUTION

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
	if constexpr (internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableA>>() &&
				  internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableB>>()) {
		static_cast<void>(execution);
		return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
	}
	else {
		return std::equal(execution, std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
	}
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
notEqual(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
	return !lz::equal(a, b, std::move(predicate), execution);
}

#endif
} // Namespace lz


#endif // LZ_BASIC_ITERATOR_VIEW_HPP