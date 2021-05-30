#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include <numeric>
#include "LzTools.hpp"

namespace lz {
namespace internal {
// Edited version of https://github.com/mirandaconrado/product-iterator
template<class... Iterators>
class CartesianProductIterator {
public:
	using value_type = std::tuple<ValueType<Iterators>...>;
	using reference = std::tuple<RefType<Iterators>...>;
	using pointer = FakePointerProxy<reference>;
	using iterator_category = typename std::common_type<std::forward_iterator_tag, IterCat<Iterators>...>::type;
	using difference_type = typename std::common_type<DiffType<Iterators>...>::type;

private:
	std::tuple<Iterators...> _begin{};
	std::tuple<Iterators...> _iterator{};
	std::tuple<Iterators...> _end{};

	template<std::size_t I>
	constexpr EnableIf<I == 0> next() const {}

#ifdef LZ_MSVC
  #pragma warning(push)
  #pragma warning(disable:4127)
#endif // LZ_MSVC

	template<std::size_t I>
	LZ_CONSTEXPR_CXX_17 EnableIf<(I > 0)> next() {
		auto& prev = std::get<I - 1>(_iterator);
		++prev;

		if (prev == std::get<I - 1>(_end)) {
			if LZ_CONSTEXPR_IF (I != 1) {
				prev = std::get<I - 1>(_begin);
				next<I - 1>();
			}
		}
	}

#ifdef LZ_MSVC
  #pragma warning(pop)
#endif // LZ_MSVC

	template<std::size_t I, EnableIf<I == 0, bool> = true>
	LZ_CONSTEXPR_CXX_17 void operatorPlusImpl(CartesianProductIterator& tmp, const difference_type offset) const {
		using lz::distance; using std::distance; using std::next; using lz::next;
		auto& iterator = std::get<I>(tmp._iterator);
		iterator = next(std::move(iterator), offset);
	}

	template<std::size_t I, EnableIf<(I > 0), bool> = true>
	void operatorPlusImpl(CartesianProductIterator& tmp, const difference_type offset) const {
		using lz::distance; using std::distance; using std::next; using lz::next;
		auto& iterator = std::get<I>(tmp._iterator);
		const auto dist = distance(iterator, std::get<I>(tmp._end));
		const auto offsets = std::lldiv(offset, dist);
		iterator = next(std::move(iterator), offsets.rem);
		operatorPlusImpl<I - 1>(tmp, offsets.quot);
	}

	template<std::size_t... Is>
	LZ_CONSTEXPR_CXX_17 reference dereference(IndexSequence<Is...>) noexcept {
		return {*std::get<Is>(_iterator)...};
	}

	template<std::size_t... Is>
	LZ_CONSTEXPR_CXX_20 difference_type distanceImpl(IndexSequence<Is...>, const CartesianProductIterator& c) const {
		using lz::distance; using std::distance;
#ifdef LZ_HAS_CXX_11
		auto mulFn = std::multiplies<difference_type>());
#else
		const auto mulFn = std::multiplies<>();
#endif // LZ_HAS_CXX_11
		const difference_type distances[] =
			{static_cast<difference_type>(distance(std::get<Is>(_iterator), std::get<Is>(c._iterator)))...};
		return std::accumulate(std::begin(distances), std::end(distances), static_cast<difference_type>(1), mulFn);
	}

	using IndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;

public:
	constexpr CartesianProductIterator() = default;

	LZ_CONSTEXPR_CXX_14 CartesianProductIterator(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
		_begin(begin),
		_iterator(begin),
		_end(std::move(end)) {}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator*()  {
		return dereference(IndexSequenceForThis());
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 pointer operator->()  {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_17 CartesianProductIterator& operator++() {
		if (std::get<0>(_iterator) == std::get<0>(_end)) {
			return *this;
		}
		next<sizeof...(Iterators)>();
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 CartesianProductIterator operator++(int) {
		CartesianProductIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool
	operator==(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
		return std::get<0>(lhs._iterator) == std::get<0>(rhs._iterator);
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool
	operator!=(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
		return !(lhs == rhs); // NOLINT
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type
	operator-(const CartesianProductIterator& a, const CartesianProductIterator& b) {
		return b.distanceImpl(IndexSequenceForThis(), a);
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 CartesianProductIterator operator+(const difference_type offset) const {
		CartesianProductIterator tmp(*this);
		operatorPlusImpl<sizeof...(Iterators) - 1>(tmp, offset);
		return tmp;
	}
};
} // internal

/**
 * Calculates the distance of a cartesian product iterator. If all iterators are random access iterators, time complexity is O(I),
 * where I is the amount of iterators (`sizeof...(Iterators)`).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence
 * @return The difference between a and b.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::CartesianProductIterator<Iterators...>::difference_type
distance(const internal::CartesianProductIterator<Iterators...>& begin, const internal::CartesianProductIterator<Iterators...>& end) {
	return end - begin;
}

/**
 * Gets the nth value from a cartesian product iterator. If all iterators are random access iterators, time complexity is O(I),
 * where I is the amount of iterators (`sizeof...(Iterators)`).
 * @param iter Cartesian product iterator instance.
 * @param offset The amount to add.
 * @return An iterator that contains `iter + offset` value.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 internal::CartesianProductIterator<Iterators...>
next(const internal::CartesianProductIterator<Iterators...>& iter,
	 const internal::DiffType<internal::CartesianProductIterator<Iterators...>> offset) {
	LZ_ASSERT(offset >= 0, "Cartesian product iterator is not random access, offset must be >= 0");
	return iter + offset;
}
}// lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
