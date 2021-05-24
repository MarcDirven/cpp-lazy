#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP

#include "LzTools.hpp"

namespace lz { namespace internal {
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_INTEGRAL Arithmetic>
class EnumerateIterator {
	Arithmetic _index;
	Iterator _iterator;

	using IterTraits = std::iterator_traits<Iterator>;
public:
	using iterator_category = typename IterTraits::iterator_category;
	using value_type = std::pair<Arithmetic, typename IterTraits::value_type>;
	using difference_type = typename IterTraits::difference_type;
	using reference = std::pair<Arithmetic, typename IterTraits::reference>;
	using pointer = FakePointerProxy<reference>;

	constexpr EnumerateIterator(const Arithmetic start, Iterator iterator) :
		_index(start),
		_iterator(std::move(iterator)) {
	}

	constexpr EnumerateIterator() = default;

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator*() const {
		return {_index, *_iterator};
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 FakePointerProxy<reference> operator->() {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator& operator++() {
		++_index;
		++_iterator;
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator operator++(int) {
		EnumerateIterator tmp = *this;
		++*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator& operator--() {
		--_index;
		--_iterator;
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator operator--(int) {
		EnumerateIterator tmp(*this);
		--*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator& operator+=(const difference_type offset) {
		_index += static_cast<Arithmetic>(offset);
		_iterator += offset;
		return *this;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 EnumerateIterator operator+(const difference_type offset) const {
		EnumerateIterator tmp(*this);
		tmp += offset;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 EnumerateIterator& operator-=(const difference_type offset) {
		_index -= static_cast<Arithmetic>(offset);
		_iterator -= offset;
		return *this;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 EnumerateIterator operator-(const difference_type offset) const {
		EnumerateIterator tmp(*this);
		tmp -= offset;
		return tmp;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend difference_type operator-(const EnumerateIterator& a, const EnumerateIterator& b) {
		return a._iterator - b._iterator;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator[](const difference_type offset) const {
		return *(*this + offset);
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator==(const EnumerateIterator& a, const EnumerateIterator& b) {
		return !(a != b); // NOLINT
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator!=(const EnumerateIterator& a, const EnumerateIterator& b) {
		return a._iterator != b._iterator;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator<(const EnumerateIterator& a, const EnumerateIterator& b) {
		return a._iterator < b._iterator;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator>(const EnumerateIterator& a, const EnumerateIterator& b) {
		return b < a; // NOLINT
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator<=(const EnumerateIterator& a, const EnumerateIterator& b) {
		return !(b < a); // NOLINT
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator>=(const EnumerateIterator& a, const EnumerateIterator& b) {
		return !(a < b); // NOLINT
	}
};
}}

#endif
